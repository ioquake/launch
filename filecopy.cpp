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
#include <time.h>
#include <QFile>
#include <QFileInfo>
#include "filecopy.h"

QString FileUtils::uniqueFilename(const QString &filename)
{
    QString unique;

    for (;;)
    {
        QFileInfo fi(filename);
        unique = fi.path() + QString("/") + QString::number(qrand(), 16) + QString("_") + fi.fileName();

        if (!QFile::exists(unique))
            break;
    }

    return unique;
}

FileCopyWorker::FileCopyWorker(const QList<FileOperation> &files) : files(files), isCancelled(false)
{
}

void FileCopyWorker::copy()
{
    qsrand(time(NULL));

    for (int i = 0; i < files.size(); i++)
    {
        QFile sourceFile(files.at(i).source);

        if (!sourceFile.open(QIODevice::ReadOnly))
        {
            emit errorMessage(QString("'%1': %2").arg(files.at(i).source).arg(sourceFile.errorString()));
            goto cleanup;
        }

        // Write to the destination file if it doesn't exist, otherwise write to a uniquely named file.
        const bool destExists = QFile::exists(files.at(i).dest);
        QString destFilename;

        if (destExists)
        {
            destFilename = FileUtils::uniqueFilename(files.at(i).dest);
        }
        else
        {
            destFilename = files.at(i).dest;
        }

        QFile destinationFile(destFilename);

        if (!destinationFile.open(QIODevice::WriteOnly))
        {
            emit errorMessage(QString("'%1': %2").arg(destFilename).arg(destinationFile.errorString()));
            goto cleanup;
        }

        // Don't add a rename operation if the destination file doesn't exist - not trying to overwrite, don't need to rename anything to complete the transaction.
        if (destExists)
        {
            FileOperation fo;
            fo.source = destFilename;
            fo.dest = files.at(i).dest;
            renameOperations.append(fo);
        }

        emit fileChanged(QFileInfo(sourceFile.fileName()).fileName());
        const qint64 totalBytes = sourceFile.size();
        qint64 totalBytesWritten = 0;

        for (;;)
        {
            const qint64 bytesRead = sourceFile.read(buffer, bufferSize);

            if (bytesRead == 0)
                break;

            const qint64 bytesWritten = destinationFile.write(buffer, bytesRead);
            totalBytesWritten += bytesWritten;
            emit progressChanged(totalBytesWritten, totalBytes);

            QMutexLocker locker(&cancelMutex);

            if (isCancelled)
                goto cleanup;
        }

    }

    emit copyFinished(renameOperations);
    return;

    // Delete all the destination files.
cleanup:
    for (int i = 0; i < renameOperations.size(); i++)
    {
        QFile::remove(renameOperations.at(i).source);
    }
}

void FileCopyWorker::cancel()
{
    QMutexLocker locker(&cancelMutex);
    isCancelled = true;
}
