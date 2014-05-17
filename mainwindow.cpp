#include <QDir>
#include <QMessageBox>
#include <QProcess>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "installwizard.h"
#include "quakeutils.h"

ioLaunch::ioLaunch(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ioLaunch)
{
    ui->setupUi(this);

    // Calculate the ioquake3 home path.
    const QString homePath = QuakeUtils::calculateHomePath();
    const QDir homeDir(homePath);

    // Try to parse q3config.cfg to get default settings if this is the first time the program has run.
    if (!homePath.isEmpty() && homeDir.exists() && !settings.getHaveRun())
    {
        QuakeUtils::parseQuake3Config(&settings, homePath);
    }

    // On first run, try to get the Q3A path.
    if (!settings.getHaveRun())
    {
        settings.setQuakePath(QuakeUtils::calculateQuake3Path());
    }

    settings.setHaveRun(true);

    // Populate the GUI with values read from settings.
    if (settings.getResolutionMode() >= 0)
    {
        // Predefined.
        ui->cbResolution->setCurrentIndex(2 + settings.getResolutionMode());
    }
    else if (settings.getResolutionMode() == -1)
    {
        // Custom.
        ui->cbResolution->setCurrentIndex(1);
    }
    else if (settings.getResolutionMode() == -2)
    {
        // Desktop.
        ui->cbResolution->setCurrentIndex(0);
    }

    ui->sbWidth->setValue(settings.getResolutionWidth());
    ui->sbHeight->setValue(settings.getResolutionHeight());

    if (settings.getResolutionFullscreen())
    {
        ui->rbFull->setChecked(true);
    }
    else
    {
        ui->rbWin->setChecked(true);
    }
}

ioLaunch::~ioLaunch()
{
    delete ui;
}

void ioLaunch::on_btnLaunch_clicked()
{
    QString ioq3;

#ifdef Q_OS_WIN32
    if(!isQuake3PathValid())
    {
        InstallWizard wizard(this, &settings);
        wizard.exec();

        if(!isQuake3PathValid())
            return;
    }

    ioq3 = QString("\"") + settings.getQuakePath() + "/ioquake3.x86.exe\"";
#elif defined Q_OS_MAC
    ioq3 = "open -a ioquake3 --args";
#elif defined Q_OS_UNIX
    ioq3 = "ioquake3";
#else
    #error "Unsupported platform"
#endif

    int r_mode = settings.getResolutionMode();
    int r_width = settings.getResolutionWidth();
    int r_height = settings.getResolutionHeight();

    // Handle modes outside what ioquake3 recognize.
    if (r_mode == 12)
    {
        r_mode = -1;
        r_width = 1280;
        r_height = 720;
    }
    else if (r_mode == 13)
    {
        r_mode = -1;
        r_width = 1920;
        r_height = 1080;
    }
    else if (r_mode == 14)
    {
        r_mode = -1;
        r_width = 1280;
        r_height = 800;
    }

    ioq3 += QString(" +set r_mode \"%1\"").arg(r_mode);

    if (r_mode == -1)
    {
        ioq3 += QString(" +set r_customwidth %1").arg(r_width) + QString(" +set r_customheight %1").arg(r_height);
    }

    ioq3 += QString(" +set r_fullscreen %1").arg(settings.getResolutionFullscreen() ? "1" : "0");

    if(!QProcess::startDetached(ioq3))
    {
        QMessageBox ioq3Failed;
        ioq3Failed.setText("ioquake3 failed to start!\nIs it installed?\n");
        ioq3Failed.exec();
    }
}

void ioLaunch::on_cbResolution_currentIndexChanged(int index)
{
    if (index == 0)
    {
        // Desktop.
        settings.setResolutionMode(-2);
    }
    else if (index == 1)
    {
        // Custom.
        settings.setResolutionMode(-1);
    }
    else if (index >= 2)
    {
        // Predefined.
        settings.setResolutionMode(index - 2);
    }

    if (index == 1)
    {
        ui->sbWidth->setEnabled(true);
        ui->sbHeight->setEnabled(true);
    }
    else
    {
        ui->sbWidth->setEnabled(false);
        ui->sbHeight->setEnabled(false);
    }
}

void ioLaunch::on_rbFull_toggled(bool checked)
{
    if(checked)
    {
        settings.setResolutionFullscreen(true);
    }
}

void ioLaunch::on_rbWin_toggled(bool checked)
{
    if(checked)
    {
        settings.setResolutionFullscreen(false);
    }
}

void ioLaunch::on_sbWidth_valueChanged(int arg1)
{
    settings.setResolutionWidth(arg1);
}

void ioLaunch::on_sbHeight_valueChanged(int arg1)
{
    settings.setResolutionHeight(arg1);
}

void ioLaunch::on_btnRunInstallWizard_clicked()
{
    InstallWizard wizard(this, &settings);
    wizard.exec();
}

#ifdef Q_OS_WIN32
bool ioLaunch::isQuake3PathValid() const
{
    if (!settings.containsQuakePath())
        return false;

    return !settings.getQuakePath().isEmpty() && QDir(settings.getQuakePath()).exists();
}
#endif
