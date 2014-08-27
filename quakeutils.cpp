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
#include <Qt>

#ifdef Q_OS_WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shlobj.h>
#endif

#include <QFile>
#include <QTextStream>
#include "quakeutils.h"
#include "settings.h"

QString QuakeUtils::calculateQuake3Path()
{
#ifdef Q_OS_WIN32
    // Try to get the Q3A path on Windows by reading it from the registry.
    QSettings registry("HKEY_LOCAL_MACHINE\\SOFTWARE\\Id\\Quake III Arena", QSettings::NativeFormat);

    if (registry.contains("INSTALLPATH"))
    {
        return registry.value("INSTALLPATH").toString();
    }
#endif

    return QString();
}

QString QuakeUtils::calculateHomePath()
{
#ifdef Q_OS_WIN32
    wchar_t path[MAX_PATH];

    if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, path)))
    {
        return QString::fromWCharArray(path) + "/Quake3";
    }
#elif defined(Q_OS_MAC) || defined(Q_OS_UNIX)
    const QByteArray homeEnvRaw = qgetenv("HOME");
    const QString homeEnv(homeEnvRaw.constData());

    #if defined Q_OS_MAC
    return homeEnv + "/Library/Application Support/Quake3";
    #elif defined Q_OS_UNIX
    return homeEnv + "/.q3a";
    #endif
#endif

    return QString();
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

void QuakeUtils::parseQuake3Config(Settings *settings, const QString &homePath)
{
    Q_ASSERT(settings);

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
                settings->setResolutionMode(ParseToken(line, offset).toInt());
            }
            else if (cvar == "r_customwidth")
            {
                settings->setResolutionWidth(ParseToken(line, offset).toInt());
            }
            else if (cvar == "r_customheight")
            {
                settings->setResolutionHeight(ParseToken(line, offset).toInt());
            }
            else if (cvar == "r_fullscreen")
            {
                settings->setResolutionFullscreen(ParseToken(line, offset).toInt() != 0);
            }
        }
    }
}
