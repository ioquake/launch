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
    settings.cpp \
    installwizard.cpp \
    installwizard_locatepage.cpp

HEADERS  += mainwindow.h \
    settings.h \
    installwizard.h \
    installwizard_locatepage.h

FORMS    += mainwindow.ui \
    installwizard.ui \
    installwizard_locatepage.ui

OTHER_FILES += \
    README.md \
    iol.png \
    iolICO.png

RESOURCES += \
    imgs.qrc

RC_FILE = launch.rc
