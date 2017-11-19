QT       += core gui

TARGET = designer
TEMPLATE = lib

CONFIG  +=plugin

DESTDIR = ../../../software/bin/pages


UI_DIR=../../../software/pages/designer/ui
MOC_DIR=../../../software/pages/designer/moc
OBJECTS_DIR=../../../software/pages/designer/obj
RCC_DIR=../../../software/pages/designer/rcc

LIBS += -L../../../software/bin -lshared
LIBS += -L../../../software/bin -lcore

HEADERS += \
    qdesignerwidget.h \
    qdesignerplugin.h \
    widgetbox/widgetboxtreewidget.h \
    widgetbox/qwidgetboxview.h \
    qdesignermimedata.h \
    property/qpropertyview.h \
    form/widgethost.h \
    form/formresizer.h \
    form/sizehandlerect.h \
    form/qformwidgetview.h \
    form/qformlistwidget.h \
    form/qdesignerformhost.h \
    form/qselectwidget.h \
    form/qnewpagedialog.h \
    form/qtemplatetreewidget.h \
    form/qtemplatenamewidget.h \
    objectlist/qobjectlistwidget.h \
    objectlist/qobjectlistview.h \
    objectlist/qlistviewwidget.h \
    form/qnewpagetable.h \
    form/qtempwidget.h \
    form/qpagewidget.h

SOURCES += \
    qdesignerwidget.cpp \
    qdesignerplugin.cpp \
    widgetbox/widgetboxtreewidget.cpp \
    widgetbox/qwidgetboxview.cpp \
    qdesignermimedata.cpp \
    property/qpropertyview.cpp \
    form/widgethost.cpp \
    form/formresizer.cpp \
    form/sizehandlerect.cpp \
    form/qformwidgetview.cpp \
    form/qformlistwidget.cpp \
    form/qdesignerformhost.cpp \
    form/qselectwidget.cpp \
    form/qnewpagedialog.cpp \
    form/qtemplatetreewidget.cpp \
    form/qtemplatenamewidget.cpp \
    objectlist/qobjectlistwidget.cpp \
    objectlist/qobjectlistview.cpp \
    objectlist/qlistviewwidget.cpp \
    form/qnewpagetable.cpp \
    form/qtempwidget.cpp \
    form/qpagewidget.cpp

RESOURCES += \
    designer.qrc

FORMS += \
    form/qnewpagedialog.ui \
    form/qtemplatenamewidget.ui \
    objectlist/qlistviewwidget.ui \
    form/qtempwidget.ui
