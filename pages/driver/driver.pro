QT       += core gui

TARGET = driver
TEMPLATE = lib

CONFIG  +=plugin

DESTDIR = ../../../software/bin/pages

UI_DIR=../../../software/pages/driver/ui
MOC_DIR=../../../software/pages/driver/moc
OBJECTS_DIR=../../../software/pages/driver/obj
RCC_DIR=../../../software/pages/driver/rcc

LIBS += -L../../../software/bin -lshared
LIBS += -L../../../software/bin -lcore

HEADERS += \
    qdriverplugin.h \
    qdriverwidget.h \
    qdrivertabview.h \
    qdriverview.h \
    qdriverdatalistview.h \
    qdatachossedialog.h \
    qdriverdataadddialog.h \
    qdriverremovedialog.h \
    qadddriverdialog.h

SOURCES += \
    qdriverplugin.cpp \
    qdriverwidget.cpp \
    qdrivertabview.cpp \
    qdriverview.cpp \
    qdriverdatalistview.cpp \
    qdatachossedialog.cpp \
    qdriverdataadddialog.cpp \
    qdriverremovedialog.cpp \
    qadddriverdialog.cpp

RESOURCES += \
    driver.qrc

FORMS += \
    qdatachossedialog.ui \
    qdriverdataadddialog.ui \
    qdriverremovedialog.ui \
    qadddriverdialog.ui
