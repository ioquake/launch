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
#include "minizip/unzip.h"
#include "fileextract.h"

FileExtractWorker::FileExtractWorker(const QString &archiveFilename, const QList<FileOperation> &filesToExtract) : archiveFilename(archiveFilename), filesToExtract(filesToExtract), isCancelled(false)
{
}

void FileExtractWorker::extract()
{
    qsrand(time(NULL));

#ifdef Q_OS_WIN32
    archiveFilename = archiveFilename.replace('/', '\\');
#endif

    unzFile zipFile = unzOpen(archiveFilename.toLatin1().constData());

    if (!zipFile)
    {
        emit errorMessage(QString("Error opening '%1' for extraction").arg(archiveFilename));
        return;
    }

    unz_global_info gi;

    if (unzGetGlobalInfo(zipFile, &gi) != UNZ_OK)
    {
        unzClose(zipFile);
        emit errorMessage(QString("Error getting global info from archive '%1'").arg(archiveFilename));
        return;
    }

    const int bufferSize = 32 * 1024;
    char buffer[bufferSize];

    for (unsigned int i = 0; i < gi.number_entry; i++)
    {
        unz_file_info fi;
        char zipFilename[256];

        if (unzGetCurrentFileInfo(zipFile, &fi, zipFilename, sizeof(zipFilename), NULL, 0, NULL, 0) != UNZ_OK)
        {
            unzClose(zipFile);
            emit errorMessage(QString("Error getting file info from archive '%1'").arg(archiveFilename));
            return;
        }

        // See if this is one of the files to be extracted.
        int extractFileIndex = -1;

        for (int j = 0; j < filesToExtract.size(); j++)
        {
            if (filesToExtract.at(j).source == zipFilename)
            {
                extractFileIndex = j;
                break;
            }
        }

        // Extract a file.
        if (extractFileIndex != -1)
        {
            if (unzOpenCurrentFile(zipFile) != UNZ_OK)
            {
                unzClose(zipFile);
                emit errorMessage(QString("'%1': error opening '%2'").arg(archiveFilename).arg(zipFilename));
                goto cleanup;
            }

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
                unzCloseCurrentFile(zipFile);
                unzClose(zipFile);
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
                const int bytesRead = unzReadCurrentFile(zipFile, buffer, bufferSize);

                if (bytesRead < 0)
                {
                    unzCloseCurrentFile(zipFile);
                    unzClose(zipFile);
                    emit errorMessage(QString("'%1': error %2 extracting '%3'").arg(archiveFilename).arg(bytesRead).arg(zipFilename));
                    goto cleanup;
                }
                else if (bytesRead == 0)
                {
                    break;
                }
                else
                {
                    file.write(buffer, bytesRead);
                    totalBytesRead += bytesRead;
                    emit progressChanged(totalBytesRead, fi.uncompressed_size);
                }

                QMutexLocker locker(&cancelMutex);

                if (isCancelled)
                {
                    unzCloseCurrentFile(zipFile);
                    unzClose(zipFile);
                    goto cleanup;
                }
            }

            unzCloseCurrentFile(zipFile);
        }

        if (i + 1 < gi.number_entry && unzGoToNextFile(zipFile) != UNZ_OK)
        {
            emit errorMessage(QString("'%1': error getting next file'").arg(archiveFilename));
            unzClose(zipFile);
            goto cleanup;
        }
    }

    unzClose(zipFile);
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
