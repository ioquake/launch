#ifndef INSTALLWIZARD_PATCH_H
#define INSTALLWIZARD_PATCH_H

#include <QNetworkAccessManager>
#include <QTemporaryFile>
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
    virtual void initializePage();
    virtual void cleanupPage();
    virtual bool isComplete() const;
    void cancel();

private slots:
    void downloadRead();
    void downloadProgress(qint64 bytesRead, qint64 bytesTotal);
    void downloadFinished();

private:
    Ui::InstallWizard_Patch *ui;
    QTemporaryFile *patchFile, *unzippedPatchFile;
    QNetworkAccessManager nam;
    QNetworkReply *networkReply;
    bool isCancelled;
    bool isDownloadFinished;
    bool isPatchInstalled;
    bool usePatchFileBuffer;
    QByteArray patchFileBuffer;
};

#endif // INSTALLWIZARD_PATCH_H
