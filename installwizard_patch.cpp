#include <QNetworkRequest>
#include <QNetworkReply>
#include <QtZlib/zlib.h>
#include "installwizard_patch.h"
#include "ui_installwizard_patch.h"
#include "installwizard.h"

InstallWizard_Patch::InstallWizard_Patch(QWidget *parent) :
    QWizardPage(parent),
    ui(new Ui::InstallWizard_Patch),
    patchFile(NULL),
    unzippedPatchFile(NULL),
    networkReply(NULL),
    isCancelled(false),
    isDownloadFinished(false),
    isPatchUnzipped(false),
    usePatchFileBuffer(true)
{
    ui->setupUi(this);
}

InstallWizard_Patch::~InstallWizard_Patch()
{
    delete patchFile;
    delete unzippedPatchFile;
    delete ui;
}

void InstallWizard_Patch::initializePage()
{
    isCancelled = isDownloadFinished = isPatchUnzipped = false;
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
    return isDownloadFinished && isPatchUnzipped;
}

void InstallWizard_Patch::cancel()
{
    if (isCancelled)
        return;

    delete patchFile;
    patchFile = NULL;
    delete unzippedPatchFile;
    unzippedPatchFile = NULL;

    if (!isDownloadFinished)
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

    if (isCancelled)
        return;

    isDownloadFinished = true;

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
        ui->lblStatus->setText("zlib inflateInit2 failed");
        cancel();
        return;
    }

    patchFile->seek(0);
    unzippedPatchFile = new QTemporaryFile;
    unzippedPatchFile->open();

    const int bufferSize = 32 * 1024;
    static char inputBuffer[bufferSize];
    static char outputBuffer[bufferSize];

    do
    {
        qint64 bytesRead = patchFile->read(inputBuffer, bufferSize);

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
                ui->lblStatus->setText(QString("zlib error %1").arg(result));
                inflateEnd(&zstream);
                cancel();
                return;
            }

            unzippedPatchFile->write(outputBuffer, bufferSize - zstream.avail_out);
        }
        while (zstream.avail_out == 0);
    }
    while (result != Z_STREAM_END);

    inflateEnd(&zstream);
    isPatchUnzipped = true;
    emit completeChanged();
}
