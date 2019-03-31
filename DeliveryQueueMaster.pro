#-------------------------------------------------
#
# Project created by QtCreator 2018-10-31T21:59:31
#
#-------------------------------------------------

# QTXlsx is an external library that allows developers to read from and write to Excel spreadsheets.
# This library is licensed under the GNU Free Documentation License version 1.3, published by the Free Software Foundation.
# This library is used for this project under the impression that this project will not be used for commercial profit.
include(qtxlsx.pri)

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DeliveryQueueMaster
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        deliveryqueue.cpp \
    adddelivery.cpp \
    delivery.cpp \
    algorithm.cpp \
    editdelivery.cpp \
    schedule.cpp \
    change.cpp

HEADERS += \
        deliveryqueue.h \
    adddelivery.h \
    delivery.h \
    algorithm.h \
    editdelivery.h \
    schedule.h \
    change.h

FORMS += \
    frmDeliveryQueue.ui \
    frmAddDelivery.ui \
    frmEditDelivery.ui \
    ChangeSchedule.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
