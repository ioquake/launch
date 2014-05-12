#include "installwizard_patch.h"
#include "ui_installwizard_patch.h"

InstallWizard_Patch::InstallWizard_Patch(QWidget *parent) :
    QWizardPage(parent),
    ui(new Ui::InstallWizard_Patch)
{
    ui->setupUi(this);
}

InstallWizard_Patch::~InstallWizard_Patch()
{
    delete ui;
}
