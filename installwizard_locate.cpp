#include <QCryptographicHash>
#include <QFileDialog>
#include <QMessageBox>
#include "installwizard_locate.h"
#include "ui_installwizard_locate.h"
#include "installwizard.h"
#include "settings.h"

InstallWizard_LocatePage::InstallWizard_LocatePage(QWidget *parent, Settings *settings) :
    QWizardPage(parent),
    ui(new Ui::InstallWizard_LocatePage),
    settings(settings),
    isQuake3PatchRequired(false)
{
    ui->setupUi(this);
}

InstallWizard_LocatePage::~InstallWizard_LocatePage()
{
    delete ui;
}

void InstallWizard_LocatePage::initializePage()
{
    registerField("quake3Path*", ui->txtLocation);
    ui->txtLocation->setText(settings->getQuakePath());
}

void InstallWizard_LocatePage::on_btnBrowse_clicked()
{
    const QString location = QFileDialog::getExistingDirectory(this, tr("Select Quake III Arena Location"), settings->getQuakePath());

    if (!location.isEmpty())
    {
        ui->txtLocation->setText(location);
    }
}

bool InstallWizard_LocatePage::validatePage()
{
    if (ui->txtLocation->text().isEmpty())
        return false;

    QDir dir(ui->txtLocation->text());

    if (!dir.exists())
    {
        QMessageBox::warning(this, "Invalid location", "The selected location doesn't exist. Please select a valid directory.");
        return false;
    }

    // Check if the Quake 3 installation needs patching.
    const QString pakFilename(ui->txtLocation->text() + "/baseq3/pak1.pk3");
    QFile file(pakFilename);

    if (!file.exists())
    {
        // pak1.pk3 doesn't exist, must be a fresh install.
        isQuake3PatchRequired = true;
        return true;
    }

    if (!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(this, "Missing file", QString("Unable to open file '%1'").arg(pakFilename));
        return false;
    }

    const QByteArray hash = QCryptographicHash::hash(file.readAll(), QCryptographicHash::Md5).toHex();
    isQuake3PatchRequired = (hash != "48911719d91be25adb957f2d325db4a0");

    return true;
}

int InstallWizard_LocatePage::nextId() const
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
