#include "installwizard_installtype.h"
#include "ui_installwizard_installtype.h"
#include "installwizard.h"

InstallWizard_InstallType::InstallWizard_InstallType(QWidget *parent) :
    QWizardPage(parent),
    ui(new Ui::InstallWizard_InstallType)
{
    ui->setupUi(this);

#ifndef Q_OS_WIN32
    ui->rbLocate->setEnabled(false);
    ui->rbInstall->setChecked(true);
#endif
}

InstallWizard_InstallType::~InstallWizard_InstallType()
{
    delete ui;
}

int InstallWizard_InstallType::nextId() const
{
    if (ui->rbInstall->isChecked())
    {
        return InstallWizard::Page_Install;
    }
#ifdef Q_OS_WIN32
    else if (ui->rbLocate->isChecked())
    {
        return InstallWizard::Page_Locate;
    }
#endif

    // Shouldn't happen.
    Q_ASSERT(false);
    return InstallWizard::Page_Finished;
}
