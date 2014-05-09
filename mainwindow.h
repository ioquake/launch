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

private:
    void parseQuake3Config();

    Ui::ioLaunch *ui;
    Settings settings;
    QString homePath;
};

#endif // MAINWINDOW_H
