QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    chatmanager.cpp \
    customeredit.cpp \
    customerinput.cpp \
    customermanager.cpp \
    customersearch.cpp \
    home.cpp \
    logsavethread.cpp \
    main.cpp \
    mainwindow.cpp \
    orderedit.cpp \
    orderinput.cpp \
    ordermanager.cpp \
    ordersearch.cpp \
    productedit.cpp \
    productinput.cpp \
    productmanager.cpp \
    productsearch.cpp

HEADERS += \
    chatmanager.h \
    customeredit.h \
    customerinput.h \
    customermanager.h \
    customersearch.h \
    home.h \
    logsavethread.h \
    mainwindow.h \
    orderedit.h \
    orderinput.h \
    ordermanager.h \
    ordersearch.h \
    productedit.h \
    productinput.h \
    productmanager.h \
    productsearch.h

FORMS += \
    chatmanager.ui \
    customermanager.ui \
    customersearch.ui \
    home.ui \
    mainwindow.ui \
    ordermanager.ui \
    ordersearch.ui \
    productmanager.ui \
    productsearch.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

TRANSLATIONS += admintr_ko.ts
RESOURCES += \
    resource.qrc

DISTFILES += \
    admintr_ko.ts


