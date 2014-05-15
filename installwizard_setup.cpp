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
    if (ui->stackPages->currentIndex() == Page_Install)
    {
        if (!QFileInfo::exists(ui->txtInstallSource->text()))
        {
            QMessageBox::warning(wizard(), "Missing source", QString("Source file '%1' does not exist.").arg(ui->txtInstallSource->text()));
            return false;
        }

        // CD doesn't contain latest patch.
        ((InstallWizard *)wizard())->setIsQuake3PatchRequired(true);

        // Copy page will copy baseq3/pak0.pk3.
        ((InstallWizard *)wizard())->addCopyFile(ui->txtInstallSource->text(), ui->txtInstallDest->text() + QString("/baseq3/pak0.pk3"));
        registerField("quake3Path", ui->txtInstallDest);
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

        for (int i = 0; i < pakFiles.size(); i++)
        {
            ((InstallWizard *)wizard())->addCopyFile(pakFiles.at(i).absoluteFilePath(), ui->txtInstallSteamDest->text() + QString("/baseq3/") + pakFiles.at(i).fileName());
        }

        registerField("quake3Path", ui->txtInstallSteamDest);
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
            registerField("quake3Path", ui->txtLocatePath);
            return true;
        }

        if (!file.open(QIODevice::ReadOnly))
        {
            QMessageBox::warning(this, "Missing file", QString("Unable to open file '%1'").arg(pakFilename));
            return false;
        }

        const QByteArray hash = QCryptographicHash::hash(file.readAll(), QCryptographicHash::Md5).toHex();
        ((InstallWizard *)wizard())->setIsQuake3PatchRequired(hash != "48911719d91be25adb957f2d325db4a0");
        registerField("quake3Path", ui->txtLocatePath);
    }
#endif

    return true;
}

bool InstallWizard_Setup::isComplete() const
{
    if (ui->stackPages->currentIndex() == Page_Install)
    {
        return !ui->txtInstallSource->text().isEmpty() && !ui->txtInstallDest->text().isEmpty();
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
    const QString location = QFileDialog::getOpenFileName(wizard(), "Select pak0.pk3 location", QString(), "Quake III Arena Pak File (pak0.pk3)");

    if (!location.isEmpty())
    {
        ui->txtInstallSource->setText(location);
    }
}

void InstallWizard_Setup::on_btnInstallBrowseDest_clicked()
{
    const QString location = QFileDialog::getExistingDirectory(this, tr("Select Quake III Arena Location"), ui->txtInstallDest->text());

    if (!location.isEmpty())
    {
        ui->txtInstallDest->setText(location);
    }
}

void InstallWizard_Setup::on_txtInstallSource_textChanged(const QString & /*arg1*/)
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
