#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QString>
#include <QtGlobal>
#include <QFileDialog>
#include <QDir>
#include "settings.h"

namespace Ui {
class ioLaunch;
}

class ioLaunch : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit ioLaunch(QWidget *parent = 0);
    ~ioLaunch();
    
private slots:
    void on_btnLaunch_clicked();

    void on_cbResolution_currentIndexChanged(int index);

    void on_rbFull_toggled(bool checked);

    void on_rbWin_toggled(bool checked);

    void on_sbWidth_valueChanged(int arg1);

    void on_sbHeight_valueChanged(int arg1);

    void on_btnRunInstallWizard_clicked();

private:
#ifdef Q_OS_WIN32
    // Returns false if the settings ioq3 path either doesn't exist or is invalid.
    bool isQuake3PathValid() const;
#endif

    void parseQuake3Config();

    Ui::ioLaunch *ui;
    Settings settings;
    QString homePath;
};

#endif // MAINWINDOW_H
