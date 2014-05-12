#include "installwizard_installtype.h"
#include "ui_installwizard_installtype.h"

InstallWizard_InstallType::InstallWizard_InstallType(QWidget *parent) :
    QWizardPage(parent),
    ui(new Ui::InstallWizard_InstallType)
{
    ui->setupUi(this);

#ifndef Q_OS_WIN32
    ui->rbLocate->setEnabled(false);
    ui->rbCD->setChecked(true);
#endif
}

InstallWizard_InstallType::~InstallWizard_InstallType()
{
    delete ui;
}
