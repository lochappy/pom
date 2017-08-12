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
    room.cc

OTHER_FILES += \
    gpl-3.0.txt \
    Makefile \
    pom.pro.user \
    README.md \
    test.pom

HEADERS += \
    array.h \
    global.h \
    integral_array.h \
    integral_proba_view.h \
    misc.h \
    normal_law.h \
    pom_solver.h \
    proba_view.h \
    rectangle.h \
    rgb_image.h \
    room.h \
    vector.h

LIBS += -lm -lpng
