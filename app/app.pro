QT       += core gui

TARGET = Software_Box
TEMPLATE = app

DESTDIR = ../../software/bin

LIBS += -L../../software/bin -lshared
LIBS += -L../../software/bin -lcore
LIBS += -L../../software/bin -lrunning

UI_DIR=../../software/app/ui
MOC_DIR=../../software/app/moc
OBJECTS_DIR=../../software/app/obj
RCC_DIR=../../software/app/rcc

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    fancyactionbar.cpp \
    fancytabwidget.cpp \
    qstatuswidget.cpp \
    qdebugwidget.cpp \
    qnewprojectdialog.cpp \
    qdevicetreewidget.cpp \
    qupdatechoosedialog.cpp \
    qupdatedialog.cpp \
    device_data/qdevicedatawindow.cpp \
    device_data/qprojectinformationtreewidget.cpp

HEADERS += \
    mainwindow.h \
    fancyactionbar.h \
    fancytabwidget.h \
    qstatuswidget.h \
    qdebugwidget.h \
    qnewprojectdialog.h \
    qdevicetreewidget.h \
    qupdatechoosedialog.h \
    qupdatedialog.h \
    device_data/qdevicedatawindow.h \
    device_data/qprojectinformationtreewidget.h

RESOURCES += \
    app.qrc

FORMS += \
    qnewprojectdialog.ui \
    qupdatechoosedialog.ui \
    qupdatedialog.ui \
    device_data/qdevicedatawindow.ui
