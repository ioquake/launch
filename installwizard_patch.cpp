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
#include <QNetworkRequest>
#include <QNetworkReply>
#include "installwizard_patch.h"
#include "ui_installwizard_patch.h"
#include "installwizard.h"
#include "fileextract.h"

InstallWizard_Patch::InstallWizard_Patch(QWidget *parent) :
    QWizardPage(parent),
    ui(new Ui::InstallWizard_Patch),
    patchFile(NULL),
    networkReply(NULL),
    isCancelled(false),
    isDownloadFinished(false),
    isPatchInstalled(false),
    extractWorker(NULL),
    isExtractFinished(false)
{
    ui->setupUi(this);
}

InstallWizard_Patch::~InstallWizard_Patch()
{
    extractThread.quit();
    extractThread.wait();
    delete patchFile;
    delete ui;
}

void InstallWizard_Patch::initializePage()
{
    isCancelled = isDownloadFinished = isPatchInstalled = isExtractFinished = false;
    extractWorker = NULL;

    patchFile = new QTemporaryFile;
    patchFile->open();

#ifdef QT_DEBUG
    QNetworkRequest networkRequest(QUrl("http://localhost:8080/quake3-latest-pk3s.zip"));
#else
    QNetworkRequest networkRequest(QUrl("http://ioquake3.org/data/quake3-latest-pk3s.zip"));
    networkRequest.setRawHeader("Referer", "http://ioquake3.org/extras/patch-data/");
    //networkRequest.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 6.1; WOW64; rv:29.0) Gecko/20100101 Firefox/29.0");
#endif

    networkReply = nam.get(networkRequest);
    connect(networkReply, &QNetworkReply::readyRead, this, &InstallWizard_Patch::downloadRead);
    connect(networkReply, &QNetworkReply::downloadProgress, this, &InstallWizard_Patch::updateProgress);
    connect(networkReply, &QNetworkReply::finished, this, &InstallWizard_Patch::downloadFinished);
}

void InstallWizard_Patch::cleanupPage()
{
    cancel();
}

bool InstallWizard_Patch::isComplete() const
{
    return isDownloadFinished && isExtractFinished && isPatchInstalled;
}

void InstallWizard_Patch::cancel()
{
    if (isCancelled)
        return;

    delete patchFile;
    patchFile = NULL;

    if (!isDownloadFinished)
        networkReply->abort();

    if (!isExtractFinished)
    {
        extractWorker->cancel();
    }

    isCancelled = true;
}

void InstallWizard_Patch::downloadRead()
{
    patchFile->write(networkReply->readAll());
}

void InstallWizard_Patch::downloadFinished()
{
    Q_ASSERT(networkReply);

    if (!isCancelled && networkReply->error())
    {
        ui->lblStatus->setText(networkReply->errorString());
        networkReply->abort();
        networkReply->deleteLater();
        isCancelled = true;
        return;
    }

    networkReply->deleteLater();

    if (isCancelled)
        return;

    patchFile->flush();
    isDownloadFinished = true;

    // Build a list of pak files to extract.
    QList<FileOperation> filesToExtract;

    for (int i = 1; i <= 8; i++)
    {
        FileOperation fo;
        fo.source = QString("quake3-latest-pk3s/baseq3/pak%1.pk3").arg(i);
        fo.dest = QString("%1/baseq3/pak%2.pk3").arg(((InstallWizard *)wizard())->getQuakePath()).arg(i);
        filesToExtract.append(fo);
    }

    // Start extract thread.
    qRegisterMetaType<QList<FileOperation> >("QList<FileOperation>");
    extractWorker = new FileExtractWorker(patchFile->fileName(), filesToExtract);
    extractWorker->moveToThread(&extractThread);
    connect(&extractThread, &QThread::finished, extractWorker, &QObject::deleteLater);
    connect(this, &InstallWizard_Patch::extract, extractWorker, &FileExtractWorker::extract);
    connect(extractWorker, &FileExtractWorker::fileChanged, this, &InstallWizard_Patch::setExtractFilename);
    connect(extractWorker, &FileExtractWorker::progressChanged, this, &InstallWizard_Patch::updateProgress);
    connect(extractWorker, &FileExtractWorker::errorMessage, this, &InstallWizard_Patch::setErrorMessage);
    connect(extractWorker, &FileExtractWorker::finished, this, &InstallWizard_Patch::finishExtract);
    extractThread.start();
    emit extract();
}

void InstallWizard_Patch::updateProgress(qint64 bytesRead, qint64 bytesTotal)
{
    if (!isDownloadFinished)
    {
        ui->lblStatus->setText(QString("Downloading %1MB / %2MB").arg(bytesRead / 1024.0 / 1024.0, 0, 'f', 2).arg(bytesTotal / 1024.0 / 1024.0, 0, 'f', 2));
    }
    else if (!isExtractFinished)
    {
        ui->lblStatus->setText(QString("Extracting %1 %2MB / %3MB").arg(extractFilename).arg(bytesRead / 1024.0 / 1024.0, 0, 'f', 2).arg(bytesTotal / 1024.0 / 1024.0, 0, 'f', 2));
    }

    ui->pbProgress->setMaximum((int)bytesTotal);
    ui->pbProgress->setValue((int)bytesRead);
}

void InstallWizard_Patch::setExtractFilename(const QString &filename)
{
    extractFilename = filename;
}

void InstallWizard_Patch::setErrorMessage(const QString &message)
{
    ui->lblStatus->setText(message);
}

void InstallWizard_Patch::finishExtract(QList<FileOperation> renameOperations)
{
    extractThread.quit();
    extractThread.wait();
    isExtractFinished = true;
    emit completeChanged();

    // Complete the transaction.
    const QString transactionError = FileUtils::completeTransaction(renameOperations);

    if (!transactionError.isEmpty())
    {
        ui->lblStatus->setText(transactionError);
        return;
    }

    isPatchInstalled = true;
    emit completeChanged();
    wizard()->next();
}
