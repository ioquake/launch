#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include "installwizard_install.h"
#include "ui_installwizard_install.h"
#include "installwizard.h"

InstallWizard_Install::InstallWizard_Install(QWidget *parent) :
    QWizardPage(parent),
    ui(new Ui::InstallWizard_Install)
{
    ui->setupUi(this);
}

InstallWizard_Install::~InstallWizard_Install()
{
    delete ui;
}

void InstallWizard_Install::initializePage()
{
#ifdef Q_OS_WIN32
    // Use the same default install directory as the Q3A installer.
    ui->txtDest->setText(QString(qgetenv("PROGRAMFILES").constData()) + QString("\\Quake III Arena"));
#endif

    registerField("pak0*", ui->txtSource);
    registerField("quake3Path*", ui->txtDest);
}

bool InstallWizard_Install::validatePage()
{
    if (!QFileInfo::exists(ui->txtSource->text()))
    {
        QMessageBox::warning(wizard(), "Missing source", QString("Source file '%1' does not exist.").arg(ui->txtSource->text()));
        return false;
    }

    return true;
}

bool InstallWizard_Install::isComplete() const
{
    return !ui->txtSource->text().isEmpty() && !ui->txtDest->text().isEmpty();
}

int InstallWizard_Install::nextId() const
{
    return InstallWizard::Page_Finished;
}

void InstallWizard_Install::on_btnBrowseSource_clicked()
{
    const QString location = QFileDialog::getOpenFileName(wizard(), "Select pak0.pk3 location", QString(), "Quake III Arena Pak File (pak0.pk3)");

    if (!location.isEmpty())
    {
        ui->txtSource->setText(location);
    }
}

void InstallWizard_Install::on_btnBrowseDest_clicked()
{
    const QString location = QFileDialog::getExistingDirectory(this, tr("Select Quake III Arena Location"), ui->txtDest->text());

    if (!location.isEmpty())
    {
        ui->txtDest->setText(location);
    }
}
