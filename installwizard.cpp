#include "installwizard.h"
#include "ui_installwizard.h"
#include "installwizard_locatepage.h"
#include "settings.h"

InstallWizard::InstallWizard(QWidget *parent, Settings *settings) :
    QWizard(parent),
    ui(new Ui::InstallWizard),
    settings(settings)
{
    ui->setupUi(this);

#ifdef Q_OS_WIN32
    setPage(Page_Locate, new InstallWizard_LocatePage(this, settings));
#endif
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
