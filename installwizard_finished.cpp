#include "installwizard_finished.h"
#include "ui_installwizard_finished.h"

InstallWizard_Finished::InstallWizard_Finished(QWidget *parent) :
    QWizardPage(parent),
    ui(new Ui::InstallWizard_Finished)
{
    ui->setupUi(this);
}

InstallWizard_Finished::~InstallWizard_Finished()
{
    delete ui;
}
