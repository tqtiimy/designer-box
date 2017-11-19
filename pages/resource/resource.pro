QT       += core gui

TARGET = resource
TEMPLATE = lib

CONFIG  +=plugin

DESTDIR = ../../../software/bin/pages


UI_DIR=../../../software/pages/resource/ui
MOC_DIR=../../../software/pages/resource/moc
OBJECTS_DIR=../../../software/pages/resource/obj
RCC_DIR=../../../software/pages/resource/rcc

LIBS += -L../../../software/bin -lcore
LIBS += -L../../../software/bin -lshared

HEADERS += \
    qresourceplugin.h \
    qresourcewidget.h \
    resourcelist/qresourcefilelist.h \
    resourcelist/qaddfilegroupdialog.h \
    resourcelist/qaddfiledialog.h \
    qeditorwidget.h \
    qstyledbarwidget.h
SOURCES += \
    qresourceplugin.cpp \
    qresourcewidget.cpp \
    resourcelist/qresourcefilelist.cpp \
    resourcelist/qaddfilegroupdialog.cpp \
    resourcelist/qaddfiledialog.cpp \
    qeditorwidget.cpp \
    qstyledbarwidget.cpp

RESOURCES += \
    resource.qrc

FORMS += \
    resourcelist/qaddfilegroupdialog.ui \
    resourcelist/qaddfiledialog.ui \
    qstyledbarwidget.ui
