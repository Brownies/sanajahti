#-------------------------------------------------
#
# Project created by QtCreator 2015-11-10T14:28:22
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = OCRTest
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

LIBS += -llept -ltesseract

QMAKE_CXXFLAGS += -Wall -Wextra -pedantic -std=c++0x
