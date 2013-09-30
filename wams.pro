#-------------------------------------------------
#
# Project created by QtCreator 2013-08-23T13:25:24
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = wams
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp

unix|win32: LIBS += -lunwind -lunwind-x86_64 -lunwind-ptrace -ldl -lrt -rdynamic
