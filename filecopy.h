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
#ifndef FILECOPY_H
#define FILECOPY_H

#include <QList>
#include <QObject>
#include <QMutex>

struct FileOperation
{
    QString source;
    QString dest;
};

class FileUtils
{
public:
    static QString uniqueFilename(const QString &filename);

    static QString completeTransaction(const QList<FileOperation> &renameOperations);
};

/*
Input: list of files to copy (ctor).
Output: list of files to rename in order to complete the transaction (copyFinished signal).

Example (using a single file):

Input source: F:\QUAKE3\baseq3\pak0.pk3
Input dest: C:\Program Files (x86)\Quake III Arena\baseq3\pak0.pk3

Source is copied to dest, but the destination file is renamed by prepending a random prefix, e.g. 34d8f_pak0.pk3.

Output source: C:\Program Files (x86)\Quake III Arena\baseq3\34d8f_pak0.pk3
Output dest: C:\Program Files (x86)\Quake III Arena\baseq3\pak0.pk3

The transaction is completed by deleting output dest and renaming output source to output dest.

If the worker is cancelled, all created files are deleted.
*/
class FileCopyWorker : public QObject
{
    Q_OBJECT

public:
    FileCopyWorker(const QList<FileOperation> &files);
    void cancel();

public slots:
    void copy();

signals:
    void fileChanged(const QString &filename);
    void progressChanged(qint64 bytesWritten, qint64 bytesTotal);
    void errorMessage(const QString &message);
    void copyFinished(QList<FileOperation> renameOperations);

private:
    static const int bufferSize = 32 * 1024;
    const QList<FileOperation> files;
    char buffer[bufferSize];
    bool isCancelled;
    QMutex cancelMutex;
    QList<FileOperation> renameOperations;
};

#endif // FILECOPY_H
