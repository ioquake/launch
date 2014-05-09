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
#include <QString>
#include "settings.h"

Settings::Settings() :
    settings("ioquake", "launch")
{
}

bool Settings::getHaveRun() const
{
    return settings.value("haveRun").toBool();
}

void Settings::setHaveRun(bool value)
{
    settings.setValue("haveRun", value);
}

QString Settings::getQuakePath() const
{
    return settings.value("quakePath").toString();
}

bool Settings::containsQuakePath() const
{
    return settings.contains("quakePath");
}

void Settings::setQuakePath(const QString &path)
{
    settings.setValue("quakePath", path);
}

int Settings::getResolutionMode() const
{
    return settings.value("resolution/mode", 3).toInt();
}

void Settings::setResolutionMode(int mode)
{
    settings.setValue("resolution/mode", mode);
}

int Settings::getResolutionWidth() const
{
    return settings.value("resolution/width", 1600).toInt();
}

void Settings::setResolutionWidth(int width)
{
    settings.setValue("resolution/width", width);
}

int Settings::getResolutionHeight() const
{
    return settings.value("resolution/height", 1024).toInt();
}

void Settings::setResolutionHeight(int height)
{
    settings.setValue("resolution/height", height);
}

bool Settings::getResolutionFullscreen() const
{
    return settings.value("resolution/fullscreen", 1).toBool();
}

void Settings::setResolutionFullscreen(bool value)
{
    settings.setValue("resolution/fullscreen", value);
}
