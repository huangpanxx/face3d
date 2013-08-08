#-------------------------------------------------
#
# Project created by QtCreator 2013-08-01T23:05:19
#
#-------------------------------------------------

QT       += opengl
QT       -= gui

CONFIG += staticlib

TARGET = algorithm

TEMPLATE = lib

DEFINES += ALGORITHM_LIBRARY

SOURCES += \
    fiber.cpp \
    common.cpp \
    matting.cpp \
    ui/scribblewidget.cpp \
    ui/functionviewer.cpp \
    ui/clickablelabel.cpp \
    ui/canvaswidget.cpp \
    share.cpp \
    gl/usfhead.cpp \
    gl/mathead.cpp \
    gl/glviewer.cpp \
    gl/globject.cpp \
    misc/triangle.cpp \
    misc/matrix.cpp \
    misc/geometry.cpp \
    misc/number.cpp \
    misc/cvextend.cpp \
    misc/interpolate.cpp \
    align/similaritytrans.cpp \
    align/shapevec.cpp \
    align/shapemodel.cpp \
    align/shapeinfo.cpp \
    align/modelimage.cpp \
    align/modelfile.cpp \
    align/asmmodel.cpp \
    align/asmdemo.cpp \
    align/asm.cpp \
    align/afreader.cpp \
    misc/qtextend.cpp \
    ui/facelocatewidget.cpp \
    face/templateface.cpp \
    gl/dotobject.cpp \
    misc/3dhelper.cpp

HEADERS += \
    fiber.h \
    common.h \
    matting.h \
    ui/scribblewidget.h \
    ui/functionviewer.h \
    ui/clickablelabel.h \
    ui/canvaswidget.h \
    share.h \
    gl/usfhead.h \
    gl/mathead.h \
    gl/glviewer.h \
    gl/globject.h \
    misc/triangle.h \
    misc/matrix.h \
    misc/geometry.h \
    misc/number.h \
    misc/cvextend.h \
    misc/interpolate.h \
    align/similaritytrans.h \
    align/shapevec.h \
    align/shapemodel.h \
    align/shapeinfo.h \
    align/modelimage.h \
    align/modelfile.h \
    align/asmmodel.h \
    align/asmdemo.h \
    align/asm.h \
    align/afreader.h \
    misc/qtextend.h \
    ui/facelocatewidget.h \
    face/templateface.h \
    gl/dotobject.h \
    misc/3dhelper.h

FORMS += \
    ui/scribblewidget.ui \
    ui/facelocatewidget.ui

OTHER_FILES +=

RESOURCES += \
    resource.qrc



macx {
# mac only
DEFINES += MACX
}
unix:!macx{
# linux only
DEFINES += LINUX
}
win32 {
# windows only
DEFINES += WIN32
}
