#-------------------------------------------------
#
# Project created by QtCreator 2020-03-30T20:54:51
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = OrcaLauncher
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    infodialog.cpp

HEADERS  += mainwindow.h \
    infodialog.h

FORMS    += mainwindow.ui \
    infodialog.ui

RESOURCES += \
    icons.qrc
