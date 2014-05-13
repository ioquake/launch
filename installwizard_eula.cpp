#include "installwizard_eula.h"
#include "ui_installwizard_eula.h"

InstallWizard_Eula::InstallWizard_Eula(QWidget *parent) :
    QWizardPage(parent),
    ui(new Ui::InstallWizard_Eula)
{
    ui->setupUi(this);
}

InstallWizard_Eula::~InstallWizard_Eula()
{
    delete ui;
}
