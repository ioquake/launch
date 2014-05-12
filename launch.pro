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
    installwizard_locate.cpp \
    installwizard_installtype.cpp \
    installwizard_finished.cpp \
    installwizard_patch.cpp

HEADERS  += mainwindow.h \
    settings.h \
    installwizard.h \
    installwizard_locate.h \
    installwizard_installtype.h \
    installwizard_finished.h \
    installwizard_patch.h

FORMS    += mainwindow.ui \
    installwizard.ui \
    installwizard_locate.ui \
    installwizard_installtype.ui \
    installwizard_finished.ui \
    installwizard_patch.ui

OTHER_FILES += \
    README.md \
    iol.png \
    iolICO.png

RESOURCES += \
    imgs.qrc

RC_FILE = launch.rc
