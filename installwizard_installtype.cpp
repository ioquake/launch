#include <QCryptographicHash>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include "installwizard_installtype.h"
#include "ui_installwizard_installtype.h"
#include "installwizard.h"
#include "settings.h"

InstallWizard_InstallType::InstallWizard_InstallType(QWidget *parent, Settings *settings) :
    QWizardPage(parent),
    ui(new Ui::InstallWizard_InstallType),
    settings(settings),
    isQuake3PatchRequired(false)
{
    ui->setupUi(this);

#ifndef Q_OS_WIN32
    ui->rbLocate->setEnabled(false);
    ui->rbInstall->setChecked(true);
    ui->stackPages->setCurrentIndex(Page_Install);
#endif
}

InstallWizard_InstallType::~InstallWizard_InstallType()
{
    delete ui;
}

void InstallWizard_InstallType::initializePage()
{
    ui->txtLocatePath->setText(settings->getQuakePath());

#ifdef Q_OS_WIN32
    // Use the same default install directory as the Q3A installer.
    ui->txtInstallDest->setText(QString(qgetenv("PROGRAMFILES").constData()) + QString("\\Quake III Arena"));
#endif
}

bool InstallWizard_InstallType::validatePage()
{
    if (ui->stackPages->currentIndex() == Page_Install)
    {
        if (!QFileInfo::exists(ui->txtInstallSource->text()))
        {
            QMessageBox::warning(wizard(), "Missing source", QString("Source file '%1' does not exist.").arg(ui->txtInstallSource->text()));
            return false;
        }

        // Copy page will copy pak0.pk3.
        ((InstallWizard *)wizard())->addCopyFile(ui->txtInstallSource->text(), ui->txtInstallDest->text() + QString("/baseq3/pak0.pk3"));
        registerField("quake3Path", ui->txtInstallDest);
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
            isQuake3PatchRequired = true;
            registerField("quake3Path", ui->txtLocatePath);
            return true;
        }

        if (!file.open(QIODevice::ReadOnly))
        {
            QMessageBox::warning(this, "Missing file", QString("Unable to open file '%1'").arg(pakFilename));
            return false;
        }

        const QByteArray hash = QCryptographicHash::hash(file.readAll(), QCryptographicHash::Md5).toHex();
        isQuake3PatchRequired = (hash != "48911719d91be25adb957f2d325db4a0");
        registerField("quake3Path", ui->txtLocatePath);
    }
#endif

    return true;
}

bool InstallWizard_InstallType::isComplete() const
{
    if (ui->stackPages->currentIndex() == Page_Install)
    {
        return !ui->txtInstallSource->text().isEmpty() && !ui->txtInstallDest->text().isEmpty();
    }
#ifdef Q_OS_WIN32
    else if (ui->stackPages->currentIndex() == Page_Locate)
    {
        return !ui->txtLocatePath->text().isEmpty();
    }
#endif

    return true;
}

int InstallWizard_InstallType::nextId() const
{
    if (ui->stackPages->currentIndex() == Page_Install)
    {
        return InstallWizard::Page_Eula;
    }
#ifdef Q_OS_WIN32
    else if (ui->stackPages->currentIndex() == Page_Locate)
    {
        if (isQuake3PatchRequired)
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

void InstallWizard_InstallType::on_rbLocate_clicked()
{
    ui->stackPages->setCurrentIndex(Page_Locate);
}

void InstallWizard_InstallType::on_rbInstall_clicked()
{
    ui->stackPages->setCurrentIndex(Page_Install);
}

void InstallWizard_InstallType::on_btnLocateBrowse_clicked()
{
    const QString location = QFileDialog::getExistingDirectory(this, tr("Select Quake III Arena Location"), settings->getQuakePath());

    if (!location.isEmpty())
    {
        ui->txtLocatePath->setText(location);
        emit completeChanged();
    }
}

void InstallWizard_InstallType::on_txtLocatePath_textChanged(const QString & /*arg1*/)
{
    emit completeChanged();
}

void InstallWizard_InstallType::on_btnInstallBrowseSource_clicked()
{
    const QString location = QFileDialog::getOpenFileName(wizard(), "Select pak0.pk3 location", QString(), "Quake III Arena Pak File (pak0.pk3)");

    if (!location.isEmpty())
    {
        ui->txtInstallSource->setText(location);
    }
}

void InstallWizard_InstallType::on_btnInstallBrowseDest_clicked()
{
    const QString location = QFileDialog::getExistingDirectory(this, tr("Select Quake III Arena Location"), ui->txtInstallDest->text());

    if (!location.isEmpty())
    {
        ui->txtInstallDest->setText(location);
    }
}

void InstallWizard_InstallType::on_txtInstallSource_textChanged(const QString & /*arg1*/)
{
    emit completeChanged();
}

void InstallWizard_InstallType::on_txtInstallDest_textChanged(const QString & /*arg1*/)
{
    emit completeChanged();
}
