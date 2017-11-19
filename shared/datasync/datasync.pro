QT       +=core gui network

TARGET = datasync
TEMPLATE = lib

CONFIG  +=plugin

DESTDIR = ../../../software/bin/shared

LIBS += -L../../../software/bin -lshared
LIBS += -L../../../software/bin -lcore


UI_DIR=../../../software/shared/datasync/ui
MOC_DIR=../../../software/shared/datasync/moc
OBJECTS_DIR=../../../software/shared/datasync/obj
RCC_DIR=../../../software/shared/datasync/rcc

HEADERS += \
    qdatasyncplugin.h

SOURCES += \
    qdatasyncplugin.cpp
