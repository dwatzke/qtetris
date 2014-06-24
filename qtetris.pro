#-------------------------------------------------
#
# Project created by QtCreator 2014-06-14T19:46:50
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qtetris
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    board.cpp \
    square.cpp \
    brick.cpp \
    previewwidget.cpp

HEADERS  += mainwindow.h \
    board.h \
    square.h \
    brick.h \
    previewwidget.h
