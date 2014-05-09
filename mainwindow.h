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


    void on_cbResolution_highlighted(int index);

    void on_cbResolution_currentIndexChanged(int index);

    void on_rbFull_toggled(bool checked);

    void on_rbWin_toggled(bool checked);

    void on_rbDefault_toggled(bool checked);

    void on_sbWidth_valueChanged(int arg1);

    void on_sbHeight_valueChanged(int arg1);

private:
    Ui::ioLaunch *ui;
    QString resOption;
    QString screenOption;
    QMessageBox msg;
    QMessageBox ioq3Failed;
    int ioWidth;
    int ioHeight;
    bool ioWedited;
    bool ioHedited;
    Settings settings;
    QString homePath;
};

#endif // MAINWINDOW_H
