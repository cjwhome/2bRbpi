#-------------------------------------------------
#
# Project created by QtCreator 2017-04-30T20:49:55
#
#-------------------------------------------------

QT       += gui core serialport network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = 2bRbpi
    target.files = 2bRbpi
    target.path = /home/armadillo
TEMPLATE = app


SOURCES += main.cpp \
    deviceprofile.cpp \
    filewriter.cpp \
    serialhandler.cpp \
    xmldevicereader.cpp \
    twobtechdevice.cpp \
    serialdataitem.cpp \
    parseddata.cpp \
    mainwindow.cpp

HEADERS += \
    deviceprofile.h \
    xmldevicereader.h \
    twobtechdevice.h \
    serialhandler.h \
    filewriter.h \
    serialdataitem.h \
    parseddata.h \
    mainwindow.h \
    defines.h

FORMS += \
    mainwindow.ui

RESOURCES += \
    res.qrc
