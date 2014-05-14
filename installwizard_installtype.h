#ifndef INSTALLWIZARD_INSTALLTYPE_H
#define INSTALLWIZARD_INSTALLTYPE_H

#include <QWizardPage>

namespace Ui {
class InstallWizard_InstallType;
}

class InstallWizard_InstallType : public QWizardPage
{
    Q_OBJECT

public:
    explicit InstallWizard_InstallType(QWidget *parent = 0);
    ~InstallWizard_InstallType();
    virtual int nextId() const;

private:
    Ui::InstallWizard_InstallType *ui;
};

#endif // INSTALLWIZARD_INSTALLTYPE_H
