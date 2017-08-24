QT += core
QT += gui

CONFIG += c++11

QT +=  widgets
CONFIG += qt

TARGET = DLPengine
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    src/cmdlineparser.cpp \
    src/slicer.cpp \
    src/mesh.cpp \
    src/fileloader.cpp \
    src/svgexporter.cpp \
    src/support.cpp \
    src/support/kbranch.cpp \
    src/infill.cpp \
    src/raft.cpp \
    src/configuration.cpp \
    src/support/generalbranch.cpp \
    src/raft/generalraft.cpp \
    src/raft/kraft.cpp \
    src/util/convexhull.cpp

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += "debug_mode=1"\
            "debug_layer=110"

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


# ClipperLib
SOURCES += src/polyclipping/clipper.cpp
HEADERS += src/polyclipping/clipper.hpp \
    src/cmdlineparser.h \
    src/slicer.h \
    src/mesh.h \
    src/configuration.h \
    src/fileloader.h \
    src/svgexporter.h \
    src/support.h \
    src/support/kbranch.h \
    src/infill.h \
    src/raft.h \
    src/support/generalbranch.h \
    src/raft/generalraft.h \
    src/raft/kraft.h \
    src/util/convexhull.h
