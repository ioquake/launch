/*
The MIT License (MIT)

Copyright (c) 2013 The ioquake Group

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
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
    connect(this, SIGNAL(finished(int)), this, SLOT(finished(int)));

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

void InstallWizard::finished(int result)
{
    if (result == QDialog::Accepted)
    {
        settings->setQuakePath(quakePath);
    }
}
