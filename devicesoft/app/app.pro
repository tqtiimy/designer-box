#-------------------------------------------------
#
# Project created by QtCreator 2014-03-21T16:33:00
#
#-------------------------------------------------

QT       += core gui


TARGET = QDeviceSoft
TEMPLATE = app

DESTDIR = ../../../software/bin

LIBS += -L../../../software/bin -lshared
LIBS += -L../../../software/bin -lrunning
LIBS += -L../../../software/bin -lbase


SOURCES += main.cpp \
    qprojectloader.cpp \
    qupdatewidget.cpp \
    qdeviceapplication.cpp

HEADERS += \
    qprojectloader.h \
    qupdatewidget.h \
    qdeviceapplication.h

FORMS += \
    qupdatewidget.ui
