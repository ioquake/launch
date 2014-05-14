#ifndef INSTALLWIZARD_EULA_H
#define INSTALLWIZARD_EULA_H

#include <QWizardPage>

namespace Ui {
class InstallWizard_Eula;
}

class InstallWizard_Eula : public QWizardPage
{
    Q_OBJECT

public:
    explicit InstallWizard_Eula(QWidget *parent = 0);
    ~InstallWizard_Eula();
    virtual int nextId() const;

private:
    Ui::InstallWizard_Eula *ui;
};

#endif // INSTALLWIZARD_EULA_H
