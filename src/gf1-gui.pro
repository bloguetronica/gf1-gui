#-------------------------------------------------
#
# Project created by QtCreator 2017-06-04T18:44:44
#
#-------------------------------------------------

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = gf1-gui
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    about.cpp \
    serial.cpp

HEADERS  += mainwindow.h \
    about.h \
    serial.h

FORMS    += mainwindow.ui \
    about.ui \
    serial.ui

RESOURCES += \
    resources.qrc
