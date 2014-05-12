#include "installwizard.h"
#include "ui_installwizard.h"
#include "installwizard_installtype.h"
#include "installwizard_locate.h"
#include "installwizard_patch.h"
#include "installwizard_finished.h"
#include "settings.h"

InstallWizard::InstallWizard(QWidget *parent, Settings *settings) :
    QWizard(parent),
    ui(new Ui::InstallWizard),
    settings(settings)
{
    ui->setupUi(this);
    setPage(Page_InstallType, new InstallWizard_InstallType(this));

#ifdef Q_OS_WIN32
    setPage(Page_Locate, new InstallWizard_LocatePage(this, settings));
#endif

    setPage(Page_Patch, new InstallWizard_Patch());
    setPage(Page_Finished, new InstallWizard_Finished());
}

InstallWizard::~InstallWizard()
{
    delete ui;
}

void InstallWizard::on_InstallWizard_customButtonClicked(int which)
{
    if (which == QWizard::FinishButton)
    {
#ifdef Q_OS_WIN32
        settings->setQuakePath(field("quake3Path").toString());
#endif
    }
}
