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

    void on_txtInstallSource_textChanged(const QString &arg1);

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
    bool isQuake3PatchRequired;
};

#endif // INSTALLWIZARD_INSTALLTYPE_H
