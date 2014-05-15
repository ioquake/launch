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

    struct CopyFile
    {
        QString source;
        QString dest;
    };

    void addCopyFile(const QString &source, const QString &dest);
    const QList<CopyFile> &getCopyFiles() const;

    bool getIsQuake3PatchRequired() const;
    void setIsQuake3PatchRequired(bool value);

    enum
    {
        Page_Setup,
        Page_Eula,
        Page_Copy,
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
    QList<CopyFile> copyFiles;
    bool isQuake3PatchRequired;
};

#endif // INSTALLWIZARD_H
