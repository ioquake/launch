#include <QDir>
#include <QMessageBox>
#include <QThread>
#include "installwizard_copy.h"
#include "ui_installwizard_copy.h"

CopyWorker::CopyWorker() : isCancelled(false)
{
}

void CopyWorker::copy(const QString &source, const QString &destination)
{
    QFile sourceFile(source);

    if (!sourceFile.open(QIODevice::ReadOnly))
    {
        emit errorMessage(QString("'%1': %2").arg(source).arg(sourceFile.errorString()));
        return;
    }

    QFile destinationFile(destination);

    if (!destinationFile.open(QIODevice::WriteOnly))
    {
        emit errorMessage(QString("'%1': %2").arg(destination).arg(destinationFile.errorString()));
        return;
    }

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

        /*if (totalBytesWritten > 1024 * 1024 * 10)
            break;*/

        QMutexLocker locker(&cancelMutex);

        if (isCancelled)
            break;
    }

    emit copyFinished();
}

void CopyWorker::cancel()
{
    QMutexLocker locker(&cancelMutex);
    isCancelled = true;
}

InstallWizard_Copy::InstallWizard_Copy(QWidget *parent) :
    QWizardPage(parent),
    ui(new Ui::InstallWizard_Copy),
    copyWorker(NULL),
    isCopyFinished(false)
{
    ui->setupUi(this);
}

InstallWizard_Copy::~InstallWizard_Copy()
{
    copyThread.quit();
    copyThread.wait();
    delete ui;
}

void InstallWizard_Copy::initializePage()
{
    isCopyFinished = false;

    // Try to create the destination directory and baseq3 subdirectory.
    const QString quake3Path(field("quake3Path").toString() + QString("/baseq3"));
    QDir dir;

    if (!dir.mkpath(quake3Path))
    {
        ui->lblStatus->setText(QString("Error creating directory '%1'").arg(quake3Path));
        return;
    }

    // Start copy thread.
    copyWorker = new CopyWorker;
    copyWorker->moveToThread(&copyThread);
    connect(&copyThread, &QThread::finished, copyWorker, &QObject::deleteLater);
    connect(this, &InstallWizard_Copy::copy, copyWorker, &CopyWorker::copy);
    connect(copyWorker, &CopyWorker::progressChanged, this, &InstallWizard_Copy::setCopyProgress);
    connect(copyWorker, &CopyWorker::errorMessage, this, &InstallWizard_Copy::setCopyErrorMessage);
    connect(copyWorker, &CopyWorker::copyFinished, this, &InstallWizard_Copy::finishCopy);
    copyThread.start();
    emit copy(field("pak0").toString(), quake3Path + QString("/pak0.pk3"));
}

bool InstallWizard_Copy::isComplete() const
{
    return isCopyFinished;
}

void InstallWizard_Copy::cancel()
{
    if (!isCopyFinished)
    {
        copyWorker->cancel();
    }
}

void InstallWizard_Copy::setCopyProgress(qint64 bytesWritten, qint64 bytesTotal)
{
    ui->lblStatus->setText(QString("Copying %1MB / %2MB").arg(bytesWritten / 1024.0 / 1024.0, 0, 'f', 2).arg(bytesTotal / 1024.0 / 1024.0, 0, 'f', 2));
    ui->pbProgress->setMaximum((int)bytesTotal);
    ui->pbProgress->setValue((int)bytesWritten);
}

void InstallWizard_Copy::setCopyErrorMessage(const QString &message)
{
    ui->lblStatus->setText(message);
}

void InstallWizard_Copy::finishCopy()
{
    copyThread.quit();
    copyThread.wait();
    isCopyFinished = true;
    emit completeChanged();
}
