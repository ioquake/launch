#include <QPushButton>
#include "installwizard.h"
#include "ui_installwizard.h"
#include "installwizard_setup.h"
#include "installwizard_copy.h"
#include "installwizard_eula.h"
#include "installwizard_patch.h"
#include "installwizard_finished.h"
#include "settings.h"

InstallWizard::InstallWizard(QWidget *parent, Settings *settings) :
    QWizard(parent),
    ui(new Ui::InstallWizard),
    settings(settings),
    isQuake3PatchRequired(false)
{
    setOptions(QWizard::NoCancelButton | QWizard::HaveCustomButton1);
    cancelButton = new QPushButton("Cancel");
    setButton(QWizard::CustomButton1, cancelButton);

    QList<QWizard::WizardButton> layout;
    layout << QWizard::BackButton << QWizard::CustomButton1 << QWizard::Stretch << QWizard::NextButton << QWizard::FinishButton;
    setButtonLayout(layout);

    ui->setupUi(this);
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancel()));

    setPage(Page_Setup, new InstallWizard_Setup(this, settings));
    setPage(Page_Eula, new InstallWizard_Eula(this));
    setPage(Page_Copy, new InstallWizard_Copy(this));
    setPage(Page_Patch, new InstallWizard_Patch(this));
    setPage(Page_Finished, new InstallWizard_Finished(this));
}

InstallWizard::~InstallWizard()
{
    delete ui;
}

void InstallWizard::clearFileCopyOperations()
{
    fileCopyOperations.clear();
}

void InstallWizard::addFileCopyOperation(const QString &source, const QString &dest)
{
    FileOperation fo;
    fo.source = source;
    fo.dest = dest;
    fileCopyOperations.append(fo);
}

const QList<FileOperation> &InstallWizard::getFileCopyOperations() const
{
    return fileCopyOperations;
}

bool InstallWizard::getIsQuake3PatchRequired() const
{
    return isQuake3PatchRequired;
}

void InstallWizard::setIsQuake3PatchRequired(bool value)
{
    isQuake3PatchRequired = value;
}

QString InstallWizard::getQuakePath() const
{
    return quakePath;
}

void InstallWizard::setQuakePath(const QString &path)
{
    quakePath = path;
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
        settings->setQuakePath(quakePath);
    }
#else
    result = result; // Silence warning.
#endif
}
