#-------------------------------------------------
#
# Project created by QtCreator 2013-08-01T23:40:35
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = test
TEMPLATE = app




SOURCES += main.cpp \
    fibertest.cpp

HEADERS  += \
    fibertest.h

CONFIG += link_pkgconfig
PKGCONFIG += opencv
LIBS += -L$$OUT_PWD/../
LIBS += -lcxsparse/cxsparse
LIBS += -lalgorithm/algorithm

INCLUDEPATH += $$PWD/../

#DEPENDPATH += $$PWD/../algorithm \
#              $$PWD/../cxsparse


FORMS +=
