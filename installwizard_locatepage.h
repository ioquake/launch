#ifndef INSTALLWIZARD_LOCATEPAGE_H
#define INSTALLWIZARD_LOCATEPAGE_H

#include <QWizardPage>

namespace Ui {
class InstallWizard_LocatePage;
}

class Settings;

class InstallWizard_LocatePage : public QWizardPage
{
    Q_OBJECT

public:
    explicit InstallWizard_LocatePage(QWidget *parent, Settings *settings);
    ~InstallWizard_LocatePage();
    virtual void initializePage();

private slots:
    void on_btnBrowse_clicked();

protected:
    virtual bool validatePage();

private:
    Ui::InstallWizard_LocatePage *ui;
    Settings *settings;
};

#endif // INSTALLWIZARD_LOCATEPAGE_H
