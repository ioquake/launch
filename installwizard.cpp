#include <QPushButton>
#include "installwizard.h"
#include "ui_installwizard.h"
#include "installwizard_installtype.h"
#include "installwizard_locate.h"
#include "installwizard_copy.h"
#include "installwizard_eula.h"
#include "installwizard_install.h"
#include "installwizard_patch.h"
#include "installwizard_finished.h"
#include "settings.h"

InstallWizard::InstallWizard(QWidget *parent, Settings *settings) :
    QWizard(parent),
    ui(new Ui::InstallWizard),
    settings(settings)
{
    setOptions(QWizard::NoCancelButton | QWizard::HaveCustomButton1);
    cancelButton = new QPushButton("Cancel");
    setButton(QWizard::CustomButton1, cancelButton);

    QList<QWizard::WizardButton> layout;
    layout << QWizard::BackButton << QWizard::CustomButton1 << QWizard::Stretch << QWizard::NextButton << QWizard::FinishButton;
    setButtonLayout(layout);

    ui->setupUi(this);
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancel()));

    setPage(Page_InstallType, new InstallWizard_InstallType(this));

#ifdef Q_OS_WIN32
    setPage(Page_Locate, new InstallWizard_LocatePage(this, settings));
#endif

    setPage(Page_Install, new InstallWizard_Install());
    setPage(Page_Eula, new InstallWizard_Eula());
    setPage(Page_Copy, new InstallWizard_Copy());
    setPage(Page_Patch, new InstallWizard_Patch());
    setPage(Page_Finished, new InstallWizard_Finished());
}

InstallWizard::~InstallWizard()
{
    delete ui;
}

void InstallWizard::cancel()
{
    if (currentId() == Page_Copy)
    {
        ((InstallWizard_Copy *)currentPage())->cancel();
    }
    else if (currentId() == Page_Patch)
    {
        ((InstallWizard_Patch *)currentPage())->cancel();
    }

    reject();
}

void InstallWizard::on_InstallWizard_finished(int result)
{
#ifdef Q_OS_WIN32
    if (result == QDialog::Accepted)
    {
        settings->setQuakePath(field("quake3Path").toString());
    }
#else
    result = result; // Silence warning.
#endif
}
