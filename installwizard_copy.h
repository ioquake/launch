#ifndef INSTALLWIZARD_COPY_H
#define INSTALLWIZARD_COPY_H

#include <QThread>
#include "installwizard.h"

namespace Ui {
class InstallWizard_Copy;
}

class FileCopyWorker;

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
    FileCopyWorker *copyWorker;
    QThread copyThread;
    bool isCopyFinished;

    // The name of the file currently being copied.
    QString copyFilename;
};

#endif // INSTALLWIZARD_COPY_H
