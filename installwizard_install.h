#ifndef INSTALLWIZARD_INSTALL_H
#define INSTALLWIZARD_INSTALL_H

#include <QWizardPage>

namespace Ui {
class InstallWizard_Install;
}

class InstallWizard_Install : public QWizardPage
{
    Q_OBJECT

public:
    explicit InstallWizard_Install(QWidget *parent = 0);
    ~InstallWizard_Install();
    virtual void initializePage();
    virtual bool validatePage();
    virtual bool isComplete() const;
    virtual int nextId() const;

private slots:
    void on_btnBrowseSource_clicked();

    void on_btnBrowseDest_clicked();

private:
    Ui::InstallWizard_Install *ui;
};

#endif // INSTALLWIZARD_INSTALL_H
