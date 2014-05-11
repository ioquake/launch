#include <Qt>
#include <QProcess>
#include <QTextStream>

#ifdef Q_OS_WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shlobj.h>
#endif

#include "mainwindow.h"
#include "ui_mainwindow.h"

ioLaunch::ioLaunch(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ioLaunch)
{
    ui->setupUi(this);

    // Calculate ioquake3 home path.
#ifdef Q_OS_WIN32
    wchar_t path[MAX_PATH];

    if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, path)))
    {
        homePath = QString::fromWCharArray(path) + "/Quake3";
    }
#elif defined(Q_OS_MAC) || defined(Q_OS_UNIX)
    const QByteArray homeEnvRaw = qgetenv("HOME");
    const QString homeEnv(homeEnvRaw.constData());

    #if defined Q_OS_MAC
    homePath = homeEnv + "/Library/Application Support/Quake3";
    #elif defined Q_OS_UNIX
    homePath = homeEnv + "/.q3a";
    #endif
#endif

    QDir homeDir(homePath);

    // Try to parse q3config.cfg to get default settings if this is the first time the program has run.
    if (!homePath.isEmpty() && homeDir.exists() && !settings.getHaveRun())
    {
        parseQuake3Config();
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
    // Prompt the user to set the ioq3 path if the settings value either doesn't exist or is invalid.
    bool promptForPath = true;

    if (settings.containsQuakePath())
    {
        const QString path = settings.getQuakePath();
        const QDir dir(path);

        if (!path.isEmpty() && dir.exists())
            promptForPath = false;
    }

    if(promptForPath)
    {
        QMessageBox msg;
        msg.setText("Please select your Quake3 directory");
        msg.exec();

        const QString path = QFileDialog::getExistingDirectory (this, tr("Directory"));

        if (path.isEmpty())
            return;

        settings.setQuakePath(path);
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

// Since q3config.cfg is generated it's nice and clean and shouldn't need a full parser.
static QString ParseToken(const QString &s, int &offset)
{
    // Skip whitespace.
    while (offset < s.length() && s[offset] == ' ')
    {
        offset++;
    }

    if (offset >= s.length())
        return QString();

    // Check for quoted token.
    bool quoted = s[offset] == '\"';

    if (quoted)
        offset++;

    // Parse token.
    int start = offset;

    while (offset < s.length() && ((quoted && s[offset] != '\"') || (!quoted && s[offset] != ' ')))
    {
        offset++;
    }

    // Get token substring.
    int end = offset;

    if (quoted && s[offset] == '\"')
    {
        offset++;
    }

    if (end - start <= 0)
        return QString();

    return s.mid(start, end - start);
}

void ioLaunch::parseQuake3Config()
{
    QFile file(homePath + "/baseq3/q3config.cfg");

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream stream(&file);

    while (!stream.atEnd())
    {
        const QString line(stream.readLine());

        // Skip comments.
        if (line.startsWith("//"))
            continue;

        int offset = 0;

        if (ParseToken(line, offset) == "seta")
        {
            const QString cvar(ParseToken(line, offset));

            if (cvar == "r_mode")
            {
                settings.setResolutionMode(ParseToken(line, offset).toInt());
            }
            else if (cvar == "r_customwidth")
            {
                settings.setResolutionWidth(ParseToken(line, offset).toInt());
            }
            else if (cvar == "r_customheight")
            {
                settings.setResolutionHeight(ParseToken(line, offset).toInt());
            }
            else if (cvar == "r_fullscreen")
            {
                settings.setResolutionFullscreen(ParseToken(line, offset).toInt() != 0);
            }
        }
    }
}
