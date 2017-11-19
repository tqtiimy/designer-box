QT       += core gui xml
QT += script

TARGET = running
TEMPLATE = lib

DESTDIR = ../../../software/bin
DEFINES += RUNNING_LIBRARY

LIBS += -L../../../software/bin -lshared

UI_DIR=../../../software/libs/runing/ui
MOC_DIR=../../../software/libs/runing/moc
OBJECTS_DIR=../../../software/runing/core/obj
RCC_DIR=../../../software/libs/runing/rcc

HEADERS += \
    runninglibglobal.h \
    qrunningmanager.h \
    qbasedialogwidget.h \
    qbasedialog.h

SOURCES += \
    qrunningmanager.cpp \
    qbasedialogwidget.cpp \
    qbasedialog.cpp
