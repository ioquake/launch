#ifndef INSTALLWIZARD_PATCH_H
#define INSTALLWIZARD_PATCH_H

#include <QWizardPage>

namespace Ui {
class InstallWizard_Patch;
}

class InstallWizard_Patch : public QWizardPage
{
    Q_OBJECT

public:
    explicit InstallWizard_Patch(QWidget *parent = 0);
    ~InstallWizard_Patch();

private:
    Ui::InstallWizard_Patch *ui;
};

#endif // INSTALLWIZARD_PATCH_H
