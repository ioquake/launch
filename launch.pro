#-------------------------------------------------
#
# Project created by QtCreator 2012-08-06T14:25:21
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = launch
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    settings.cpp \
    installwizard.cpp \
    installwizard_setup.cpp \
    installwizard_finished.cpp \
    installwizard_patch.cpp \
    installwizard_eula.cpp \
    installwizard_copy.cpp \
    filecopy.cpp \
    quakeutils.cpp

HEADERS  += mainwindow.h \
    settings.h \
    installwizard.h \
    installwizard_setup.h \
    installwizard_finished.h \
    installwizard_patch.h \
    installwizard_eula.h \
    installwizard_copy.h \
    filecopy.h \
    quakeutils.h

FORMS    += mainwindow.ui \
    installwizard.ui \
    installwizard_setup.ui \
    installwizard_finished.ui \
    installwizard_patch.ui \
    installwizard_eula.ui \
    installwizard_copy.ui

OTHER_FILES += \
    README.md \
    iol.png \
    iolICO.png

RESOURCES += \
    imgs.qrc

RC_FILE = launch.rc
