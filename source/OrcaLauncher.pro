#-------------------------------------------------
#
# Project created by QtCreator 2020-03-30T20:54:51
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

VERSION = 1.4.0
QMAKE_TARGET_COMPANY = "ORG335a"
QMAKE_TARGET_PRODUCT = OrcaLauncher

TARGET = OrcaLauncher
TEMPLATE = app
RC_ICONS = programIcon.ico

SOURCES += main.cpp\
    aboutdialog.cpp \
        mainwindow.cpp \
    infodialog.cpp \
    queue.cpp \
    templatesmanager.cpp

HEADERS  += mainwindow.h \
    aboutdialog.h \
    infodialog.h \
    queue.h \
    templatesmanager.h

FORMS    += mainwindow.ui \
    aboutdialog.ui \
    infodialog.ui \
    templatesmanager.ui

RESOURCES += \
    icons.qrc

CONFIG += rtti

