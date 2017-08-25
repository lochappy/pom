#-------------------------------------------------
#
# Project created by QtCreator 2017-08-12T11:36:22
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = pom
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app
DESTDIR = ../


SOURCES += \
    global.cc \
    misc.cc \
    normal_law.cc \
    pom.cc \
    pom_solver.cc \
    proba_view.cc \
    rectangle.cc \
    rgb_image.cc \
    room.cc \
    IntegralImage.cc

OTHER_FILES += \
    gpl-3.0.txt \
    Makefile \
    pom.pro.user \
    README.md \
    test.pom

HEADERS += \
    array.h \
    global.h \
    misc.h \
    normal_law.h \
    pom_solver.h \
    proba_view.h \
    rectangle.h \
    rgb_image.h \
    room.h \
    vector.h \
    IntegralImage.h

LIBS += -lm -lpng
LIBS += -lopencv_calib3d \
                                                -lopencv_contrib \
                                                -lopencv_core \
                                                -lopencv_features2d \
                                                -lopencv_flann \
                                                -lopencv_gpu \
                                                -lopencv_highgui \
                                                -lopencv_imgproc \
                                                -lopencv_legacy \
                                                -lopencv_ml \
                                                -lopencv_nonfree \
                                                -lopencv_objdetect \
                                                -lopencv_photo \
                                                -lopencv_stitching \
                                                -lopencv_superres \
                                                -lopencv_ts \
                                                -lopencv_video \
                                                -lopencv_videostab \
                                                -lopencv_viz
