#-------------------------------------------------
#
# Project created by QtCreator 2020-01-11T16:56:48
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PicData
TEMPLATE = app

win32: RC_ICONS = $$PWD/bin/images/vertical.ico


SOURCES += main.cpp\
        mainwindow.cpp \
        card.cpp \
        counter.cpp \
    model.cpp \
    application.cpp \
    worker.cpp \
    imgloader.cpp \
    imgwidget.cpp \
    addfolder.cpp \
    data_pic.cpp

HEADERS  += mainwindow.h \
    card.h \
    counter.h \
    model.h \
    application.h \
    worker.h \
    imgloader.h \
    imgwidget.h \
    addfolder.h \
    data_pic.h

FORMS    += mainwindow.ui \
    card.ui \
    imgwidget.ui \
    addfolder.ui

RESOURCES += \
    bin/img.qrc
