#include <QFileDialog>
#include <QMessageBox>
#include "installwizard_locatepage.h"
#include "ui_installwizard_locatepage.h"
#include "settings.h"

InstallWizard_LocatePage::InstallWizard_LocatePage(QWidget *parent, Settings *settings) :
    QWizardPage(parent),
    ui(new Ui::InstallWizard_LocatePage),
    settings(settings)
{
    ui->setupUi(this);
    registerField("quake3Path*", ui->txtLocation);
}

InstallWizard_LocatePage::~InstallWizard_LocatePage()
{
    delete ui;
}

void InstallWizard_LocatePage::initializePage()
{
    ui->txtLocation->setText(settings->getQuakePath());
}

void InstallWizard_LocatePage::on_btnBrowse_clicked()
{
    const QString location = QFileDialog::getExistingDirectory(this, tr("Select Quake 3: Arena Location"), settings->getQuakePath());

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

    return true;
}
