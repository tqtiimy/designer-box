#-------------------------------------------------
#
# Project created by QtCreator 2014-03-28T14:55:00
#
#-------------------------------------------------

TARGET = gradientEditor
TEMPLATE = lib
DESTDIR = ../../../software/bin

MOC_DIR=../../../software/libs/gradientEditor/moc
OBJECTS_DIR=../../../software/libs/gradientEditor/obj
RCC_DIR=../../../software/libs/gradientEditor/rcc

DEFINES += GRADIENTEDITOR_LIBRARY

SOURCES += \
    qtgradienteditor.cpp \
    qtgradientstopscontroller.cpp \
    qtgradientstopsmodel.cpp \
    qtcolorline.cpp \
    qtcolorbutton.cpp \
    qtgradientstopswidget.cpp \
    qtgradientwidget.cpp \
    qtgradientdialog.cpp

HEADERS +=\
        gradientEditor_global.h \
    qtgradienteditor.h \
    qtgradientstopscontroller.h \
    qtgradientstopsmodel.h \
    qtcolorline.h \
    qtcolorbutton.h \
    qtgradientstopswidget.h \
    qtgradientwidget.h \
    qtgradientdialog.h

FORMS += \
    qtgradienteditor.ui \
    qtgradientdialog.ui

RESOURCES += \
    gradienteditor.qrc
