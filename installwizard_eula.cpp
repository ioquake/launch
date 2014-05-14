#include "installwizard_eula.h"
#include "ui_installwizard_eula.h"
#include "installwizard.h"

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

int InstallWizard_Eula::nextId() const
{
    if (!field("pak0").toString().isEmpty())
    {
        return InstallWizard::Page_Copy;
    }

    return InstallWizard::Page_Patch;
}
