#include <QProcess>
#include "mainwindow.h"
#include "ui_mainwindow.h"

ioLaunch::ioLaunch(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ioLaunch),
    ioWidth(0), ioHeight(0), ioWedited(false), ioHedited(false)
{
    ui->setupUi(this);
    resOption = "";
    screenOption = "";

}

ioLaunch::~ioLaunch()
{
    delete ui;
}

void ioLaunch::on_btnLaunch_clicked()
{
#ifdef Q_OS_WIN32
    if(ioq3 == NULL)
    {
        msg.setText("Please select your Quake3 directory");
        msg.exec();
        QString path = QFileDialog::getExistingDirectory (this, tr("Directory"), directory.path());
        ioq3 = QString("\"") + path + QDir::separator() + "ioquake3.x86.exe\" +set r_mode -1";
    }
#elif defined Q_OS_MAC
    ioq3 = "open -a ioquake3 --args +set r_mode -1";
#elif defined Q_OS_UNIX
    ioq3 = "ioquake3 +set r_mode -1";
#else
    #error "Unsupported platform"
#endif


    if(ioWedited == true && ioHedited == true)
    {

        resOption = " +set r_customwidth " + QString::number(ioWidth) + " +set r_customheight " + QString::number(ioHeight);

    }
    if(resOption == NULL)
    {
        resOption = "";
    }
    if(screenOption == NULL)
    {
        screenOption = "";
    }

    if(!QProcess::startDetached(ioq3+resOption+screenOption))
    {
        ioq3Failed.setText("ioquake3 failed to start!\nIs it installed?\n");
        ioq3Failed.exec();
    }
}

void ioLaunch::on_cbResolution_highlighted(int /*index*/)
{
    ioWedited = false;
    ioHedited = false;
}

void ioLaunch::on_cbResolution_currentIndexChanged(int index)
{
    ioWedited = false;
    ioHedited = false;
    switch(index)
    {
        case 0:
            {
                resOption = "";
                break;
            }
        case 1:
            {
                ioWidth = QApplication::desktop()->screenGeometry().width();
                ioHeight = QApplication::desktop()->screenGeometry().height();

                resOption = " +set r_customwidth " + QString::number(ioWidth) + " +set r_customheight " + QString::number(ioHeight);
                break;
            }
        case 2:
            {
                resOption = " +set r_customwidth 320 +set r_customheight 240";
                break;
            }
        case 3:
            {
                resOption = " +set r_customwidth 400 +set r_customheight 300";
                break;
            }
        case 4:
            {
                resOption = " +set r_customwidth 512 +set r_customheight 384";
                break;
            }
        case 5:
            {
                resOption = " +set r_customwidth 640 +set r_customheight 480";
                break;
            }
        case 6:
            {
                resOption = " +set r_customwidth 800 +set r_customheight 600";
                break;
            }
        case 7:
            {
                resOption = " +set r_customwidth 960 +set r_customheight 720";
                break;
            }
        case 8:
            {
                resOption = " +set r_customwidth 1024 +set r_customheight 768";
                break;
            }
        case 9:
            {
                resOption = " +set r_customwidth 1152 +set r_customheight 864";
                break;
            }
        case 10:
            {
                resOption = " +set r_customwidth 1280 +set r_customheight 1024";
                break;
            }
        case 11:
            {
                resOption = " +set r_customwidth 1600 +set r_customheight 1200";
                break;
            }
        case 12:
            {
                resOption = " +set r_customwidth 2048 +set r_customheight 1536";
                break;
            }
        case 13:
            {
                resOption = " +set r_customwidth 856 +set r_customheight 480";
                break;
            }
        case 14:
            {
                resOption = " +set r_customwidth 1280 +set r_customheight 720";
                break;
            }
        case 15:
            {
                resOption = " +set r_customwidth 1920 +set r_customheight 1080";
                break;
            }
        case 16:
            {
                resOption = " +set r_customwidth 1280 +set r_customheight 800";
                break;
            }
        default:
            {
                resOption = "";
                break;
            }
    }
}

void ioLaunch::on_rbFull_toggled(bool checked)
{
    if(checked)
    {
        screenOption = " +set r_fullscreen 1";
    }
}

void ioLaunch::on_rbWin_toggled(bool checked)
{
    if(checked)
    {
        screenOption = " +set r_fullscreen 0";
    }
}

void ioLaunch::on_rbDefault_toggled(bool checked)
{
    if(checked)
    {
        screenOption = "";
    }
}


void ioLaunch::on_sbWidth_valueChanged(int arg1)
{
    ioWidth = 0;
    if(arg1 >= 0)
    {
        ioWidth = arg1;
        ioWedited = true;
    }
    else{
        ioWedited = false;
    }
}

void ioLaunch::on_sbHeight_valueChanged(int arg1)
{
    ioHeight = 0;
    if(arg1 >= 0)
    {
        ioHeight = arg1;
        ioHedited = true;
    }
    else{
        ioHedited = false;
    }
}
