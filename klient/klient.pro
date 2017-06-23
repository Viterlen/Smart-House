#-------------------------------------------------
#
# Project created by QtCreator 2017-06-12T16:26:30
#
#-------------------------------------------------

QT       += core gui
CONFIG   += c++14
QT += serialport
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = klient
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui
