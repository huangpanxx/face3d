#-------------------------------------------------
#
# Project created by QtCreator 2013-08-01T23:40:35
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = test
TEMPLATE = app
CONFIG += c++11



SOURCES += main.cpp \
    fibertest.cpp

HEADERS  += \
    fibertest.h
#opencv
QT_CONFIG -= no-pkg-config
CONFIG += link_pkgconfig
PKGCONFIG += opencv

#algorithm
LIBS    +=  -L$$OUT_PWD/../algorithm -lalgorithm
#cxsparse
LIBS    +=  -L$$OUT_PWD/../cxsparse -lcxsparse
#stasm
LIBS    +=  -L$$OUT_PWD/../stasm -lstasm

INCLUDEPATH += $$PWD/../
DEPENDPATH += $$PWD/../cxsparse
DEPENDPATH += $$PWD/../algorithm
DEPENDPATH += $$PWD/../stasm
