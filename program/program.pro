#-------------------------------------------------
#
# Project created by QtCreator 2013-08-01T23:07:32
#
#-------------------------------------------------



QT       += core gui opengl


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


TARGET = program
TEMPLATE = app

DEFINES += DEBUG

SOURCES += main.cpp \
    align3d.cpp

HEADERS  += \
    align3d.h

CONFIG += c++11

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


