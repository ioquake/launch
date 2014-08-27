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
#include <Qt>

#ifdef Q_OS_WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <QCryptographicHash>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include "installwizard_setup.h"
#include "ui_installwizard_setup.h"
#include "installwizard.h"
#include "settings.h"

InstallWizard_Setup::InstallWizard_Setup(QWidget *parent, Settings *settings) :
    QWizardPage(parent),
    ui(new Ui::InstallWizard_Setup),
    settings(settings)
{
    ui->setupUi(this);

#ifndef Q_OS_WIN32
    ui->rbLocate->setEnabled(false);
    ui->rbInstall->setChecked(true);
    ui->stackPages->setCurrentIndex(Page_Install);
#endif

    // Populate drives list.
    QFileInfoList drives = QDir::drives();
    int selectIndex = -1;

    for (int i = 0; i < drives.size(); i++)
    {
        const QString path(drives.at(i).absoluteFilePath());
        QString text(path);

#ifdef Q_OS_WIN32
        // Only add CD/DVD volumes.
        if (GetDriveType((LPCWSTR)path.utf16()) != DRIVE_CDROM)
            continue;

        // Get the volume name. If it's "Quake3", select this drive.
        WCHAR volumeNameRaw[128];

        if (GetVolumeInformation((LPCWSTR)path.utf16(), volumeNameRaw, sizeof(volumeNameRaw), NULL, NULL, NULL, NULL, 0))
        {
            QString volumeName(QString::fromWCharArray(volumeNameRaw));

            if (!volumeName.isEmpty())
            {
                text = QString("%1 (%2)").arg(path).arg(volumeName);

                if (volumeName == "Quake3")
                {
                    selectIndex = ui->cbInstallSource->count();
                }
            }
        }
#endif

        ui->cbInstallSource->addItem(text, path);
    }

    if (selectIndex != -1)
    {
        ui->cbInstallSource->setCurrentIndex(selectIndex);
    }
}

InstallWizard_Setup::~InstallWizard_Setup()
{
    delete ui;
}

void InstallWizard_Setup::initializePage()
{
    ui->txtLocatePath->setText(settings->getQuakePath());

#ifdef Q_OS_WIN32
    // Use the same default install directory as the Q3A installer.
    const QString windowsInstallDefault(QString(qgetenv("PROGRAMFILES").constData()) + QString("\\Quake III Arena"));
    ui->txtInstallDest->setText(windowsInstallDefault);
    ui->txtInstallSteamDest->setText(windowsInstallDefault);

    // Try to get the Steam path from the registry.
    QSettings registry("HKEY_LOCAL_MACHINE\\SOFTWARE\\Valve\\Steam", QSettings::NativeFormat);

    if (registry.contains("InstallPath"))
    {
        ui->txtInstallSteamSource->setText(registry.value("InstallPath").toString());
    }
#endif
}

bool InstallWizard_Setup::validatePage()
{
    InstallWizard *iw = (InstallWizard *)wizard();

    if (ui->stackPages->currentIndex() == Page_Install)
    {
        QDir dir(ui->cbInstallSource->currentData().toString());

        if (!dir.exists())
        {
            QMessageBox::warning(wizard(), "Missing source", "Source directory does not exist. Please select a valid directory.");
            return false;
        }

        // CD doesn't contain latest patch.
        iw->setIsQuake3PatchRequired(true);

        // Copy page will copy baseq3/pak0.pk3.
        iw->clearFileCopyOperations();
        iw->addFileCopyOperation(ui->cbInstallSource->currentData().toString() + QString("/QUAKE3/baseq3/pak0.pk3"), ui->txtInstallDest->text() + QString("/baseq3/pak0.pk3"));
        iw->setQuakePath(ui->txtInstallDest->text());
    }
    else if (ui->stackPages->currentIndex() == Page_InstallSteam)
    {
        // Check that the selected Steam path is valid.
        QDir dir(ui->txtInstallSteamSource->text());

        if (!dir.exists())
        {
            QMessageBox::warning(this, "Invalid Steam location", "The selected Steam location doesn't exist. Please select a valid directory.");
            return false;
        }

        // Check that Q3A is installed (the baseq3 dir exists).
        QDir steamQuakeDir(ui->txtInstallSteamSource->text() + QString("/steamapps/common/quake 3 arena/baseq3"));

        if (!steamQuakeDir.exists())
        {
            QMessageBox::warning(this, "Steam Quake III Arena not installed", QString("Steam Quake III Arena not installed. Directory '%1' doesn't exist. Please select a valid directory.").arg(steamQuakeDir.path()));
            return false;
        }

        // Copy page will copy baseq3/*.pk3 files.
        QFileInfoList pakFiles = steamQuakeDir.entryInfoList(QStringList("*.pk3"), QDir::Files | QDir::NoSymLinks | QDir::Readable, QDir::Name);
        iw->clearFileCopyOperations();

        for (int i = 0; i < pakFiles.size(); i++)
        {
            iw->addFileCopyOperation(pakFiles.at(i).absoluteFilePath(), ui->txtInstallSteamDest->text() + QString("/baseq3/") + pakFiles.at(i).fileName());
        }

        iw->setQuakePath(ui->txtInstallSteamDest->text());
    }
#ifdef Q_OS_WIN32
    else if (ui->stackPages->currentIndex() == Page_Locate)
    {
        if (ui->txtLocatePath->text().isEmpty())
            return false;

        QDir dir(ui->txtLocatePath->text());

        if (!dir.exists())
        {
            QMessageBox::warning(this, "Invalid location", "The selected location doesn't exist. Please select a valid directory.");
            return false;
        }

        // Check if the Quake 3 installation needs patching.
        const QString pakFilename(ui->txtLocatePath->text() + "/baseq3/pak1.pk3");
        QFile file(pakFilename);

        if (!file.exists())
        {
            // pak1.pk3 doesn't exist, must be a fresh install.
            ((InstallWizard *)wizard())->setIsQuake3PatchRequired(true);
            iw->setQuakePath(ui->txtLocatePath->text());
            return true;
        }

        if (!file.open(QIODevice::ReadOnly))
        {
            QMessageBox::warning(this, "Missing file", QString("Unable to open file '%1'").arg(pakFilename));
            return false;
        }

        const QByteArray hash = QCryptographicHash::hash(file.readAll(), QCryptographicHash::Md5).toHex();
        iw->setIsQuake3PatchRequired(hash != "48911719d91be25adb957f2d325db4a0");
        iw->setQuakePath(ui->txtLocatePath->text());
    }
#endif

    return true;
}

bool InstallWizard_Setup::isComplete() const
{
    if (ui->stackPages->currentIndex() == Page_Install)
    {
        return !ui->cbInstallSource->currentData().toString().isEmpty() && !ui->txtInstallDest->text().isEmpty();
    }
    else if (ui->stackPages->currentIndex() == Page_InstallSteam)
    {
        return !ui->txtInstallSteamSource->text().isEmpty() && !ui->txtInstallSteamDest->text().isEmpty();
    }
#ifdef Q_OS_WIN32
    else if (ui->stackPages->currentIndex() == Page_Locate)
    {
        return !ui->txtLocatePath->text().isEmpty();
    }
#endif

    return true;
}

int InstallWizard_Setup::nextId() const
{
    if (ui->stackPages->currentIndex() == Page_Install || ui->stackPages->currentIndex() == Page_InstallSteam)
    {
        return InstallWizard::Page_Eula;
    }
#ifdef Q_OS_WIN32
    else if (ui->stackPages->currentIndex() == Page_Locate)
    {
        if (((InstallWizard *)wizard())->getIsQuake3PatchRequired())
        {
            return InstallWizard::Page_Eula;
        }
        else
        {
            return InstallWizard::Page_Finished;
        }
    }
#endif

    // Shouldn't happen.
    Q_ASSERT(false);
    return InstallWizard::Page_Finished;
}

void InstallWizard_Setup::on_rbLocate_clicked()
{
    ui->stackPages->setCurrentIndex(Page_Locate);
}

void InstallWizard_Setup::on_rbInstall_clicked()
{
    ui->stackPages->setCurrentIndex(Page_Install);
}

void InstallWizard_Setup::on_rbInstallSteam_clicked()
{
    ui->stackPages->setCurrentIndex(Page_InstallSteam);
}

void InstallWizard_Setup::on_btnLocateBrowse_clicked()
{
    const QString location = QFileDialog::getExistingDirectory(this, tr("Select Quake III Arena Location"), settings->getQuakePath());

    if (!location.isEmpty())
    {
        ui->txtLocatePath->setText(location);
        emit completeChanged();
    }
}

void InstallWizard_Setup::on_txtLocatePath_textChanged(const QString & /*arg1*/)
{
    emit completeChanged();
}

void InstallWizard_Setup::on_btnInstallBrowseSource_clicked()
{
    const QString location = QFileDialog::getExistingDirectory(wizard(), "Select Quake III Arena CD-ROM directory", ui->cbInstallSource->currentData().toString());

    if (!location.isEmpty())
    {
        // Find or add.
        int index = ui->cbInstallSource->findText(location, Qt::MatchFixedString);

        if (index == -1)
        {
            ui->cbInstallSource->addItem(location, location);
            index = ui->cbInstallSource->count() - 1;
        }

        ui->cbInstallSource->setCurrentIndex(index);
    }
}

void InstallWizard_Setup::on_btnInstallBrowseDest_clicked()
{
    const QString location = QFileDialog::getExistingDirectory(this, tr("Select Quake III Arena Install Directory"), ui->txtInstallDest->text());

    if (!location.isEmpty())
    {
        ui->txtInstallDest->setText(location);
    }
}

void InstallWizard_Setup::on_cbInstallSource_currentTextChanged(const QString & /*arg1*/)
{
    emit completeChanged();
}

void InstallWizard_Setup::on_txtInstallDest_textChanged(const QString & /*arg1*/)
{
    emit completeChanged();
}

void InstallWizard_Setup::on_btnInstallSteamBrowseSource_clicked()
{
    const QString location = QFileDialog::getExistingDirectory(this, tr("Select Steam Location"), ui->txtInstallSteamSource->text());

    if (!location.isEmpty())
    {
        ui->txtInstallSteamSource->setText(location);
    }
}

void InstallWizard_Setup::on_btnInstallSteamBrowseDest_clicked()
{
    const QString location = QFileDialog::getExistingDirectory(this, tr("Select Quake III Arena Location"), ui->txtInstallSteamDest->text());

    if (!location.isEmpty())
    {
        ui->txtInstallSteamDest->setText(location);
    }
}

void InstallWizard_Setup::on_txtInstallSteamSource_textChanged(const QString & /*arg1*/)
{
    emit completeChanged();
}

void InstallWizard_Setup::on_txtInstallSteamDest_textChanged(const QString & /*arg1*/)
{
    emit completeChanged();
}
