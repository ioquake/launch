/*
The MIT License (MIT)

Copyright (c) 2013 The ioquake Group

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include <math.h>
#include <time.h>
#include <QFile>
#include <QFileInfo>
#include <QtZlib/zlib.h>
#include "fileextract.h"

struct TarHeader
{
    char name[100];
    char mode[8];
    char uid[8];
    char gid[8];
    char size[12];
    char mtime[12];
    char chksum[8];
    char typeflag;
    char linkname[100];
    char magic[6];
    char version[2];
    char uname[32];
    char gname[32];
    char devmajor[8];
    char devminor[8];
    char prefix[155];
};

FileExtractWorker::FileExtractWorker(const QString &archiveFilename, const QList<FileOperation> &filesToExtract) : archiveFilename(archiveFilename), filesToExtract(filesToExtract), isCancelled(false)
{
}

void FileExtractWorker::extract()
{
    qsrand(time(NULL));

    // Extract gzip compressed archive.
    z_stream zstream;
    zstream.zalloc = Z_NULL;
    zstream.zfree = Z_NULL;
    zstream.opaque = Z_NULL;
    zstream.avail_in = 0;
    zstream.next_in = Z_NULL;

    int result = inflateInit2(&zstream, 32 | MAX_WBITS);

    if (result != Z_OK)
    {
        emit errorMessage("zlib inflateInit2 failed");
        return;
    }

    QFile archiveFile(archiveFilename);

    if (!archiveFile.open(QIODevice::ReadOnly))
    {
        emit errorMessage(QString("Error opening '%1': '%2'").arg(archiveFile.fileName()).arg(archiveFile.errorString()));
        return;
    }

    tarFile.open();

    const int bufferSize = 32 * 1024;
    static char inputBuffer[bufferSize];
    static char outputBuffer[bufferSize];

    do
    {
        qint64 bytesRead = archiveFile.read(inputBuffer, bufferSize);

        if (bytesRead == 0)
            break;

        zstream.avail_in = bytesRead;
        zstream.next_in = (z_Bytef *)inputBuffer;

        do
        {
            zstream.avail_out = bufferSize;
            zstream.next_out = (z_Bytef *)outputBuffer;

            result = inflate(&zstream, Z_NO_FLUSH);

            if (result != Z_OK && result != Z_STREAM_END)
            {
                emit errorMessage(QString("zlib error %1").arg(result));
                inflateEnd(&zstream);
                return;
            }

            tarFile.write(outputBuffer, bufferSize - zstream.avail_out);
        }
        while (zstream.avail_out == 0);
    }
    while (result != Z_STREAM_END);

    archiveFile.close();
    inflateEnd(&zstream);

    // Extract the files in the TAR file.
    tarFile.seek(0);

    for (;;)
    {
        // Read TAR file header (padded to 512 bytes).
        TarHeader header;

        if (tarFile.read((char *)&header, sizeof(header)) != sizeof(header))
            break; // EOF

        tarFile.seek(tarFile.pos() + 512 - sizeof(header));

        // Convert size from octal string.
        qint64 size = 0;
        int exponent = 0;

        for (int i = sizeof(header.size) - 1; i >= 0; i--)
        {
            int digit = (int)(header.size[i] - '0');

            if (digit < 0 || digit > 7)
                continue;

            size += qint64(digit * pow(8.0f, exponent));
            exponent++;
        }

        // See if this is one of the files to be extracted.
        int extractFileIndex = -1;

        for (int i = 0; i < filesToExtract.size(); i++)
        {
            if (filesToExtract.at(i).source == header.name)
            {
                extractFileIndex = i;
                break;
            }
        }

        // Extract a file.
        if (extractFileIndex != -1)
        {
            // Write to the destination file if it doesn't exist, otherwise write to a uniquely named file.
            QString filename(filesToExtract.at(extractFileIndex).dest);
            const bool fileExists = QFile::exists(filename);

            if (fileExists)
            {
                filename = FileUtils::uniqueFilename(filesToExtract.at(extractFileIndex).dest);
            }

            QFile file(filename);

            if (!file.open(QIODevice::WriteOnly))
            {
                emit errorMessage(QString("'%1': %2").arg(file.fileName()).arg(file.errorString()));
                goto cleanup;
            }

            // Don't add a rename operation if the destination file doesn't exist - not trying to overwrite, don't need to rename anything to complete the transaction.
            if (fileExists)
            {
                FileOperation fo;
                fo.source = filename;
                fo.dest = filesToExtract.at(extractFileIndex).dest;
                renameOperations.append(fo);
            }

            emit fileChanged(QFileInfo(file.fileName()).fileName());
            qint64 totalBytesRead = 0;

            for (;;)
            {
                qint64 bytesToRead = bufferSize;

                if (totalBytesRead + bytesToRead > size)
                {
                    bytesToRead = size - totalBytesRead;
                }

                const qint64 bytesRead = tarFile.read(outputBuffer, bytesToRead);

                if (bytesRead == 0)
                    break;

                file.write(outputBuffer, bytesRead);
                totalBytesRead += bytesRead;
                emit progressChanged(totalBytesRead, size);

                QMutexLocker locker(&cancelMutex);

                if (isCancelled)
                    goto cleanup;
            }
        }
        else
        {
            tarFile.seek(tarFile.pos() + size);
        }

        // TAR file size is padded to 512 byte blocks.
        tarFile.seek(tarFile.pos() + ((size % 512) == 0 ? 0 : 512 - (size % 512)));
    }

    emit finished(renameOperations);
    return;

    // Delete all the destination files.
cleanup:
    for (int i = 0; i < renameOperations.size(); i++)
    {
        QFile::remove(renameOperations.at(i).source);
    }
}

void FileExtractWorker::cancel()
{
    QMutexLocker locker(&cancelMutex);
    isCancelled = true;
}
