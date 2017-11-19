QT       +=core gui network

TARGET = update
TEMPLATE = lib

CONFIG  +=plugin

DESTDIR = ../../../software/bin/shared

LIBS += -L../../../software/bin -lshared
LIBS += -L../../../software/bin -lcore


UI_DIR=../../../software/shared/update/ui
MOC_DIR=../../../software/shared/update/moc
OBJECTS_DIR=../../../software/shared/update/obj
RCC_DIR=../../../software/shared/update/rcc

HEADERS += \
    qupdateplugin.h \
    net/qnetupdate.h \
    net/basedefine.h \
    net/qlistendevice.h \
    net/qheartbest.h \
    net/qsendfile.h

SOURCES += \
    qupdateplugin.cpp \
    net/qnetupdate.cpp \
    net/qlistendevice.cpp \
    net/qheartbest.cpp \
    net/qsendfile.cpp
