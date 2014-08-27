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
#ifndef INSTALLWIZARD_COPY_H
#define INSTALLWIZARD_COPY_H

#include <QThread>
#include "installwizard.h"

namespace Ui {
class InstallWizard_Copy;
}

class FileCopyWorker;

class InstallWizard_Copy : public QWizardPage
{
    Q_OBJECT

public:
    explicit InstallWizard_Copy(QWidget *parent = 0);
    ~InstallWizard_Copy();
    virtual void initializePage();
    virtual bool isComplete() const;
    virtual int nextId() const;
    void cancel();

private slots:
    void setCopyFilename(const QString &filename);
    void setCopyProgress(qint64 bytesWritten, qint64 bytesTotal);
    void setCopyErrorMessage(const QString &message);
    void finishCopy(QList<FileOperation> renameOperations);

signals:
    void copy();

private:
    Ui::InstallWizard_Copy *ui;
    FileCopyWorker *copyWorker;
    QThread copyThread;
    bool isCopyFinished;

    // The name of the file currently being copied.
    QString copyFilename;
};

#endif // INSTALLWIZARD_COPY_H
