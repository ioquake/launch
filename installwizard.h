#ifndef INSTALLWIZARD_H
#define INSTALLWIZARD_H

#include <QWizard>
#include "filecopy.h"

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

    void clearFileCopyOperations();
    void addFileCopyOperation(const QString &source, const QString &dest);
    const QList<FileCopyOperation> &getFileCopyOperations() const;

    bool getIsQuake3PatchRequired() const;
    void setIsQuake3PatchRequired(bool value);

    QString getQuakePath() const;
    void setQuakePath(const QString &path);

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
    QList<FileCopyOperation> fileCopyOperations;
    bool isQuake3PatchRequired;
    QString quakePath;
};

#endif // INSTALLWIZARD_H
