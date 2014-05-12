#ifndef INSTALLWIZARD_FINISHED_H
#define INSTALLWIZARD_FINISHED_H

#include <QWizardPage>

namespace Ui {
class InstallWizard_Finished;
}

class InstallWizard_Finished : public QWizardPage
{
    Q_OBJECT

public:
    explicit InstallWizard_Finished(QWidget *parent = 0);
    ~InstallWizard_Finished();

private:
    Ui::InstallWizard_Finished *ui;
};

#endif // INSTALLWIZARD_FINISHED_H
