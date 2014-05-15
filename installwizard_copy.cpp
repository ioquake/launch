#include <QDir>
#include <QMessageBox>
#include <QThread>
#include "installwizard_copy.h"
#include "ui_installwizard_copy.h"

CopyWorker::CopyWorker(const QList<InstallWizard::CopyFile> &copyFiles) : copyFiles(copyFiles), isCancelled(false)
{
}

void CopyWorker::copy()
{
    for (int i = 0; i < copyFiles.size(); i++)
    {
        QFile sourceFile(copyFiles.at(i).source);

        if (!sourceFile.open(QIODevice::ReadOnly))
        {
            emit errorMessage(QString("'%1': %2").arg(copyFiles.at(i).source).arg(sourceFile.errorString()));
            return;
        }

        QFile destinationFile(copyFiles.at(i).dest);

        if (!destinationFile.open(QIODevice::WriteOnly))
        {
            emit errorMessage(QString("'%1': %2").arg(copyFiles.at(i).dest).arg(destinationFile.errorString()));
            return;
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
                break;
        }
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
    ui->lblStatus->setText("");
    isCopyFinished = false;
    copyFilename = QString();

    // Try to create the destination directory and baseq3 subdirectory.
    const QString quake3Path(((InstallWizard *)wizard())->getQuakePath() + QString("/baseq3"));
    QDir dir;

    if (!dir.mkpath(quake3Path))
    {
        ui->lblStatus->setText(QString("Error creating directory '%1'").arg(quake3Path));
        return;
    }

    // Start copy thread.
    copyWorker = new CopyWorker(((InstallWizard *)wizard())->getCopyFiles());
    copyWorker->moveToThread(&copyThread);
    connect(&copyThread, &QThread::finished, copyWorker, &QObject::deleteLater);
    connect(this, &InstallWizard_Copy::copy, copyWorker, &CopyWorker::copy);
    connect(copyWorker, &CopyWorker::fileChanged, this, &InstallWizard_Copy::setCopyFilename);
    connect(copyWorker, &CopyWorker::progressChanged, this, &InstallWizard_Copy::setCopyProgress);
    connect(copyWorker, &CopyWorker::errorMessage, this, &InstallWizard_Copy::setCopyErrorMessage);
    connect(copyWorker, &CopyWorker::copyFinished, this, &InstallWizard_Copy::finishCopy);
    copyThread.start();

    emit copy();
}

bool InstallWizard_Copy::isComplete() const
{
    return isCopyFinished;
}

int InstallWizard_Copy::nextId() const
{
    if (((InstallWizard *)wizard())->getIsQuake3PatchRequired())
    {
        return InstallWizard::Page_Patch;
    }

    return InstallWizard::Page_Finished;
}

void InstallWizard_Copy::cancel()
{
    if (!isCopyFinished)
    {
        copyWorker->cancel();
    }
}

void InstallWizard_Copy::setCopyFilename(const QString &filename)
{
    copyFilename = filename;
}

void InstallWizard_Copy::setCopyProgress(qint64 bytesWritten, qint64 bytesTotal)
{
    ui->lblStatus->setText(QString("Copying %1 (%2MB / %3MB)").arg(copyFilename).arg(bytesWritten / 1024.0 / 1024.0, 0, 'f', 2).arg(bytesTotal / 1024.0 / 1024.0, 0, 'f', 2));
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
    wizard()->next();
}
