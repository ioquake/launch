#-------------------------------------------------
#
# Project created by QtCreator 2012-08-06T14:25:21
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = launch
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    settings.cpp

HEADERS  += mainwindow.h \
    settings.h

FORMS    += mainwindow.ui

OTHER_FILES += \
    README.md \
    iol.png \
    iolICO.png

RESOURCES += \
    imgs.qrc

RC_FILE = launch.rc
