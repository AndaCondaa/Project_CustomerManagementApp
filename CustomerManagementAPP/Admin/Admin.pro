QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    chatmanager.cpp \
    customerinput.cpp \
    customermanager.cpp \
    delegate.cpp \
    home.cpp \
    logsavethread.cpp \
    main.cpp \
    mainwindow.cpp \
    orderinput.cpp \
    ordermanager.cpp \
    productinput.cpp \
    productmanager.cpp

HEADERS += \
    chatmanager.h \
    customerinput.h \
    customermanager.h \
    delegate.h \
    home.h \
    logsavethread.h \
    mainwindow.h \
    orderinput.h \
    ordermanager.h \
    productinput.h \
    productmanager.h

FORMS += \
    chatmanager.ui \
    customermanager.ui \
    home.ui \
    mainwindow.ui \
    ordermanager.ui \
    productmanager.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource.qrc

DISTFILES +=


