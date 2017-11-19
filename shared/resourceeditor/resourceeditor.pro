QT       +=core gui network

TARGET = resourceeditor
TEMPLATE = lib

CONFIG  +=plugin

DESTDIR = ../../../software/bin/shared

LIBS += -L../../../software/bin -lshared
LIBS += -L../../../software/bin -lcore


UI_DIR=../../../software/shared/resourceeditor/ui
MOC_DIR=../../../software/shared/resourceeditor/moc
OBJECTS_DIR=../../../software/shared/resourceeditor/obj
RCC_DIR=../../../software/shared/resourceeditor/rcc

HEADERS += \
    qbaesfileeditorplugin.h \
    texteditor/qplaintexteditor.h \
    imageeditor/qimageeditor.h \
    imageeditor/qimageview.h \
    texteditor/qtexteditview.h

SOURCES += \
    qbaesfileeditorplugin.cpp \
    texteditor/qplaintexteditor.cpp \
    imageeditor/qimageeditor.cpp \
    imageeditor/qimageview.cpp \
    texteditor/qtexteditview.cpp
