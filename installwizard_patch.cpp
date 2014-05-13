#include <QNetworkRequest>
#include <QNetworkReply>
#include "installwizard_patch.h"
#include "ui_installwizard_patch.h"
#include "installwizard.h"

InstallWizard_Patch::InstallWizard_Patch(QWidget *parent) :
    QWizardPage(parent),
    ui(new Ui::InstallWizard_Patch),
    patchFile(NULL),
    networkReply(NULL),
    isCancelled(false),
    isDownloadFinished(false),
    usePatchFileBuffer(true)
{
    ui->setupUi(this);
}

InstallWizard_Patch::~InstallWizard_Patch()
{
    delete patchFile;
    delete ui;
}

void InstallWizard_Patch::initializePage()
{
    isCancelled = isDownloadFinished = false;
    patchFileBuffer.clear();
    usePatchFileBuffer = true;

    patchFile = new QTemporaryFile;
    patchFile->open();

    networkReply = nam.get(QNetworkRequest(QUrl("http://localhost:8080/linuxq3apoint-1.32b-3.x86.run")));
    connect(networkReply, SIGNAL(readyRead()), this, SLOT(downloadRead()));
    connect(networkReply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(downloadProgress(qint64,qint64)));
    connect(networkReply, SIGNAL(finished()), this, SLOT(downloadFinished()));
}

void InstallWizard_Patch::cleanupPage()
{
    cancel();
}

bool InstallWizard_Patch::isComplete() const
{
    return isDownloadFinished;
}

void InstallWizard_Patch::cancel()
{
    if (isCancelled)
        return;

    delete patchFile;
    patchFile = NULL;
    networkReply->abort();
    isCancelled = true;
}

void InstallWizard_Patch::downloadRead()
{
    // Skip the first n bytes, giving a valid tar.gz file.
    if (usePatchFileBuffer)
    {
        const qint64 bytesToSkip = 8251;

        patchFileBuffer.append(networkReply->readAll());

        if (patchFileBuffer.length() > bytesToSkip + 1)
        {
            patchFile->write(&patchFileBuffer.data()[bytesToSkip], patchFileBuffer.length() - bytesToSkip);
            usePatchFileBuffer = false;
            patchFileBuffer.clear();
        }
    }
    else
    {
        patchFile->write(networkReply->readAll());
    }
}

void InstallWizard_Patch::downloadProgress(qint64 bytesRead, qint64 bytesTotal)
{
    ui->lblStatus->setText(QString("Downloading %1MB / %2MB").arg(bytesRead / 1024.0 / 1024.0, 0, 'f', 2).arg(bytesTotal / 1024.0 / 1024.0, 0, 'f', 2));
    ui->pbProgress->setMaximum((int)bytesTotal);
    ui->pbProgress->setValue((int)bytesRead);
}

void InstallWizard_Patch::downloadFinished()
{
    Q_ASSERT(networkReply);

    if (!isCancelled && networkReply->error())
    {
        ui->lblStatus->setText(networkReply->errorString());
    }

    networkReply->deleteLater();
    isDownloadFinished = true;
    emit completeChanged();
}
