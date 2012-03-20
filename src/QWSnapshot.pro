#-------------------------------------------------
#
# Project created by QtCreator 2012-03-17T23:41:55
#
#-------------------------------------------------

QT       += core gui webkit

TARGET = qwsnapshot
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

target.path = /usr/local/bin/
launcher.path = /usr/share/applications
launcher.files = ../data/QWSnapshot.desktop
icon.path = /usr/share/pixmaps
icon.files = ../data/QWSnapshot.png
INSTALLS += target launcher icon

RESOURCES += \
    res.qrc

windows:RC_FILE = qwsnapshot.rc
