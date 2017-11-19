QT       += core gui

TARGET = project
TEMPLATE = lib

CONFIG  +=plugin

DESTDIR = ../../../software/bin/pages

UI_DIR=../../../software/pages/project/ui
MOC_DIR=../../../software/pages/project/moc
OBJECTS_DIR=../../../software/pages/project/obj
RCC_DIR=../../../software/pages/project/rcc

LIBS += -L../../../software/bin -lshared
LIBS += -L../../../software/bin -lcore

RESOURCES += \
    project.qrc \

HEADERS += \
    qprojectplugin.h \
    qprojectwidget.h \
    qprojectpropertyview.h \
    qusermanagerview.h \
    qlanguagemanagerview.h \
    qadduserdialog.h \
    qaddlanguagedialog.h \
    quserandlanguagetreewidget.h

SOURCES += \
    qprojectplugin.cpp \
    qprojectwidget.cpp \
    qprojectpropertyview.cpp \
    qusermanagerview.cpp \
    qlanguagemanagerview.cpp \
    qadduserdialog.cpp \
    qaddlanguagedialog.cpp \
    quserandlanguagetreewidget.cpp

FORMS += \
    qadduserdialog.ui \
    qaddlanguagedialog.ui
