#ifndef INSTALLWIZARD_H
#define INSTALLWIZARD_H

#include <QWizard>

namespace Ui {
class InstallWizard;
}

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
        Page_Locate
    };

private slots:
    void on_InstallWizard_customButtonClicked(int which);

private:
    Ui::InstallWizard *ui;
    Settings *settings;
};

#endif // INSTALLWIZARD_H
