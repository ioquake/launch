#ifndef INSTALLWIZARD_H
#define INSTALLWIZARD_H

#include <QWizard>

namespace Ui {
class InstallWizard;
}

class QPushButton;
class Settings;

class InstallWizard : public QWizard
{
    Q_OBJECT

public:
    explicit InstallWizard(QWidget *parent, Settings *settings);
    ~InstallWizard();

    enum
    {
        Page_InstallType,
        Page_Locate,
        Page_Patch,
        Page_Finished
    };

private slots:
    void cancel();
    void on_InstallWizard_finished(int result);

private:
    Ui::InstallWizard *ui;
    QPushButton *cancelButton;
    Settings *settings;
};

#endif // INSTALLWIZARD_H
