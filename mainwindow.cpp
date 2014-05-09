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
    ui(new Ui::ioLaunch),
    ioWidth(0), ioHeight(0), ioWedited(false), ioHedited(false)
{
    ui->setupUi(this);
    resOption = "";
    screenOption = "";

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
        ui->cbResolution->setCurrentIndex(0);
    }
    else if (settings.getResolutionMode() == -2)
    {
        // Desktop.
        ui->cbResolution->setCurrentIndex(1);
    }

    ui->sbWidth->setValue(settings.getResolutionWidth());
    ui->sbHeight->setValue(settings.getResolutionHeight());
    ioWedited = ioHedited = true;

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
        msg.setText("Please select your Quake3 directory");
        msg.exec();

        const QString path = QFileDialog::getExistingDirectory (this, tr("Directory"));

        if (path.isEmpty())
            return;

        settings.setQuakePath(path);
    }

    ioq3 = QString("\"") + settings.getQuakePath() + "/ioquake3.x86.exe\" +set r_mode -1";
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

    if (index == 0)
    {
        // Custom.
        settings.setResolutionMode(-1);
    }
    else if (index == 1)
    {
        // Desktop.
        settings.setResolutionMode(-2);
    }
    else
    {
        // Predefined.
        settings.setResolutionMode(index - 2);
    }
}

void ioLaunch::on_rbFull_toggled(bool checked)
{
    if(checked)
    {
        screenOption = " +set r_fullscreen 1";
        settings.setResolutionFullscreen(true);
    }
}

void ioLaunch::on_rbWin_toggled(bool checked)
{
    if(checked)
    {
        screenOption = " +set r_fullscreen 0";
        settings.setResolutionFullscreen(false);
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

    settings.setResolutionWidth(arg1);
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
