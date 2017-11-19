QT       += core gui

TARGET = data
TEMPLATE = lib

CONFIG  +=plugin

DESTDIR = ../../../software/bin/pages

UI_DIR=../../../software/pages/data/ui
MOC_DIR=../../../software/pages/data/moc
OBJECTS_DIR=../../../software/pages/data/obj
RCC_DIR=../../../software/pages/data/rcc

LIBS += -L../../../software/bin -lshared
LIBS += -L../../../software/bin -lcore

HEADERS += \
    qdataplugin.h \
    qdataviewtabwidget.h \
    qdatawidget.h \
    qdatalistwidget.h \
    qadddatadialog.h \
    qadddatagroupdialog.h \
    qdatagroupremovedialog.h

SOURCES += \
    qdataplugin.cpp \
    qdataviewtabwidget.cpp \
    qdatawidget.cpp \
    qdatalistwidget.cpp \
    qadddatadialog.cpp \
    qadddatagroupdialog.cpp \
    qdatagroupremovedialog.cpp

RESOURCES += \
    data.qrc

FORMS += \
    qadddatadialog.ui \
    qadddatagroupdialog.ui \
    qdatagroupremovedialog.ui
