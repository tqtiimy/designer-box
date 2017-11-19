QT       += core gui

TARGET = base
TEMPLATE = lib

DEFINES += BASE_LIBRARY


DESTDIR = ../../../../software/bin

LIBS += -L../../../../software/bin -lshared
LIBS += -L../../../../software/bin -lrunning

HEADERS += \
    baselibglobal.h \
    qupdateplugin.h \
    qabstractupdater.h \
    qupdatemanager.h

SOURCES += \
    qabstractupdater.cpp \
    qupdatemanager.cpp
