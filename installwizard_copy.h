#ifndef INSTALLWIZARD_COPY_H
#define INSTALLWIZARD_COPY_H

#include <QMutex>
#include <QThread>
#include <QWizardPage>
#include "installwizard.h"

namespace Ui {
class InstallWizard_Copy;
}

class CopyWorker : public QObject
{
    Q_OBJECT

public:
    CopyWorker(const QList<InstallWizard::CopyFile> &copyFiles);
    void cancel();

public slots:
    void copy();

signals:
    void fileChanged(const QString &filename);
    void progressChanged(qint64 bytesWritten, qint64 bytesTotal);
    void errorMessage(const QString &message);
    void copyFinished();

private:
    static const int bufferSize = 32 * 1024;
    const QList<InstallWizard::CopyFile> copyFiles;
    char buffer[bufferSize];
    bool isCancelled;
    QMutex cancelMutex;
};

class InstallWizard_Copy : public QWizardPage
{
    Q_OBJECT

public:
    explicit InstallWizard_Copy(QWidget *parent = 0);
    ~InstallWizard_Copy();
    virtual void initializePage();
    virtual bool isComplete() const;
    virtual int nextId() const;
    void cancel();

private slots:
    void setCopyFilename(const QString &filename);
    void setCopyProgress(qint64 bytesWritten, qint64 bytesTotal);
    void setCopyErrorMessage(const QString &message);
    void finishCopy();

signals:
    void copy();

private:
    Ui::InstallWizard_Copy *ui;
    CopyWorker *copyWorker;
    QThread copyThread;
    bool isCopyFinished;

    // The name of the file currently being copied.
    QString copyFilename;
};

#endif // INSTALLWIZARD_COPY_H
