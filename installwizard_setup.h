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
#ifndef INSTALLWIZARD_INSTALLTYPE_H
#define INSTALLWIZARD_INSTALLTYPE_H

#include <QWizardPage>

namespace Ui {
class InstallWizard_Setup;
}

class Settings;

class InstallWizard_Setup : public QWizardPage
{
    Q_OBJECT

public:
    explicit InstallWizard_Setup(QWidget *parent, Settings *settings);
    ~InstallWizard_Setup();
    virtual void initializePage();
    virtual bool validatePage();
    virtual bool isComplete() const;
    virtual int nextId() const;

private slots:
    void on_rbLocate_clicked();

    void on_rbInstall_clicked();

    void on_rbInstallSteam_clicked();

    void on_btnLocateBrowse_clicked();

    void on_txtLocatePath_textChanged(const QString &arg1);

    void on_btnInstallBrowseSource_clicked();

    void on_btnInstallBrowseDest_clicked();

    void on_cbInstallSource_currentTextChanged(const QString &arg1);

    void on_txtInstallDest_textChanged(const QString &arg1);

    void on_btnInstallSteamBrowseSource_clicked();

    void on_btnInstallSteamBrowseDest_clicked();

    void on_txtInstallSteamSource_textChanged(const QString &arg1);

    void on_txtInstallSteamDest_textChanged(const QString &arg1);

private:
    enum
    {
        Page_Locate,
        Page_Install,
        Page_InstallSteam
    };

    Ui::InstallWizard_Setup *ui;

    Settings *settings;
};

#endif // INSTALLWIZARD_INSTALLTYPE_H
