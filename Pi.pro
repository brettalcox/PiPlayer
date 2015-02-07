#-------------------------------------------------
#
# Project created by QtCreator 2015-01-21T13:50:24
#
#-------------------------------------------------

QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Pi
TEMPLATE = app
CONFIG += c++11


SOURCES += main.cpp\
        player.cpp \
    playlist.cpp

HEADERS  += player.h \
    playlist.h

FORMS    += player.ui
