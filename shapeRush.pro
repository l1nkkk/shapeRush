QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    drawview.cpp \
    gof/adaptershape.cpp \
    gof/command.cpp \
    gof/composite.cpp \
    gof/decorator.cpp \
    gof/factory.cpp \
    gof/iterator.cpp \
    gof/observer.cpp \
    gof/prototype.cpp \
    gof/proxy.cpp \
    gof/singletom.cpp \
    gof/strategy.cpp \
    gof/unclone.cpp \
    main.cpp \
    mainwindow.cpp \
    mainwindowStable.cpp \
    shape/elliitem.cpp \
    shape/lineitem.cpp \
    shape/rectitem.cpp \
    shape/shape.cpp \
    storage/serialization.cpp

HEADERS += \
    drawview.h \
    gof/adaptershape.h \
    gof/command.h \
    gof/composite.h \
    gof/decorator.h \
    gof/factory.h \
    gof/iterator.h \
    gof/observer.h \
    gof/prototype.h \
    gof/proxy.h \
    gof/singletom.h \
    gof/strategy.h \
    gof/unclone.h \
    mainwindow.h \
    shape/elliitem.h \
    shape/lineitem.h \
    shape/rectitem.h \
    shape/shape.h \
    shape/shapeImport.h \
    shape/shapeItem.h \
    storage/serialization.h

FORMS += \
    mainwindow.ui


LIBS += /usr/local/lib/libyaml-cpp.a

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    README.MD
