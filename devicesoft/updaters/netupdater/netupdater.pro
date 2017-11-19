QT       += core gui network

TARGET = netupdater
TEMPLATE = lib

CONFIG  +=plugin

DESTDIR = ../../../../software/bin/device/updaters

LIBS += -L../../../../software/bin -lshared
LIBS += -L../../../../software/bin -lbase

HEADERS += \
    qnetupdaterplugin.h \
    qfindserver.h \
    basedefine.h \
    qnetupdater.h \
    qheartbest.h \
    qdataresever.h

SOURCES += \
    qnetupdaterplugin.cpp \
    qfindserver.cpp \
    qnetupdater.cpp \
    qheartbest.cpp \
    qdataresever.cpp
