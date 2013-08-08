#-------------------------------------------------
#
# Project created by QtCreator 2013-08-01T23:40:35
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = test
TEMPLATE = app



macx {
# mac only
LIBS    += -l cxsparse \
           -I/usr/local/include/opencv -I/usr/local/include /usr/local/lib/libopencv_calib3d.dylib /usr/local/lib/libopencv_contrib.dylib /usr/local/lib/libopencv_core.dylib /usr/local/lib/libopencv_features2d.dylib /usr/local/lib/libopencv_flann.dylib /usr/local/lib/libopencv_gpu.dylib /usr/local/lib/libopencv_highgui.dylib /usr/local/lib/libopencv_imgproc.dylib /usr/local/lib/libopencv_legacy.dylib /usr/local/lib/libopencv_ml.dylib /usr/local/lib/libopencv_nonfree.dylib /usr/local/lib/libopencv_objdetect.dylib /usr/local/lib/libopencv_ocl.dylib /usr/local/lib/libopencv_photo.dylib /usr/local/lib/libopencv_stitching.dylib /usr/local/lib/libopencv_superres.dylib /usr/local/lib/libopencv_ts.dylib /usr/local/lib/libopencv_video.dylib /usr/local/lib/libopencv_videostab.dylib
DEFINES += MACX
}
unix:!macx{
# linux only
LIBS    +=  -L/usr/lib/
LIBS    +=  -lcxsparse

CONFIG += link_pkgconfig
PKGCONFIG += opencv
DEFINES += LINUX
}
win32 {
# windows only
DEFINES += WIN32
}

SOURCES += main.cpp \
    fibertest.cpp

HEADERS  += \
    fibertest.h




win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../algorithm/release/ -lalgorithm
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../algorithm/debug/ -lalgorithm
else:unix: LIBS += -L$$OUT_PWD/../algorithm/ -lalgorithm

INCLUDEPATH += $$PWD/../algorithm
DEPENDPATH += $$PWD/../algorithm

win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../algorithm/release/algorithm.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../algorithm/debug/algorithm.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../algorithm/libalgorithm.a

FORMS +=
