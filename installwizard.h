/*
The MIT License (MIT)

Copyright (c) 2013 The ioquake Group

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
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
    const QList<FileOperation> &getFileCopyOperations() const;

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
    QList<FileOperation> fileCopyOperations;
    bool isQuake3PatchRequired;
    QString quakePath;
};

#endif // INSTALLWIZARD_H
