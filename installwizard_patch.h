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
#ifndef INSTALLWIZARD_PATCH_H
#define INSTALLWIZARD_PATCH_H

#include <QNetworkAccessManager>
#include <QTemporaryFile>
#include <QThread>
#include <QWizardPage>
#include "filecopy.h"

namespace Ui {
class InstallWizard_Patch;
}

class FileExtractWorker;

class InstallWizard_Patch : public QWizardPage
{
    Q_OBJECT

public:
    explicit InstallWizard_Patch(QWidget *parent = 0);
    ~InstallWizard_Patch();
    virtual void initializePage();
    virtual void cleanupPage();
    virtual bool isComplete() const;
    void cancel();

private slots:
    void downloadRead();
    void downloadFinished();
    void updateProgress(qint64 bytesRead, qint64 bytesTotal);
    void setExtractFilename(const QString &filename);
    void setErrorMessage(const QString &message);
    void finishExtract(QList<FileOperation> renameOperations);

signals:
    void extract();

private:
    Ui::InstallWizard_Patch *ui;
    QTemporaryFile *patchFile;
    QNetworkAccessManager nam;
    QNetworkReply *networkReply;
    bool isCancelled;
    bool isDownloadFinished;
    bool isPatchInstalled;

    QString extractFilename;
    FileExtractWorker *extractWorker;
    QThread extractThread;
    bool isExtractFinished;
};

#endif // INSTALLWIZARD_PATCH_H
