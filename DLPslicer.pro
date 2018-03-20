.TEMPLATE = app

QT += qml quick opengl 3dcore 3drender 3dinput 3dextras

CONFIG += c++11 resources_big

TARGET = DentStudio

SOURCES += main.cpp \
    quaternionhelper.cpp \
    slicingengine.cpp \
    configuration.cpp \
    glmodel.cpp \
    qmlmanager.cpp \
    fileloader.cpp \
    mesh.cpp \
    lights.cpp \
    polyclipping/clipper.cpp \
    feature/autoorientation.cpp \
    feature/modelcut.cpp \
    feature/meshrepair.cpp \
    feature/arrange.cpp \
    feature/convex_hull.cpp \
    feature/text3dgeometrygenerator.cpp \
    feature/labellingtextpreview.cpp \
    utils/mathutils.cpp \
    utils/qtriangulator.cpp

RESOURCES += qml.qrc \
    resources.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# for extra security on Windows: enable ASLR and DEP via GCC linker flags
win32:QMAKE_LFLAGS *= -Wl,--dynamicbase -Wl,--nxcompat
# enable GCC large addresss aware linker flag
win32:QMAKE_LFLAGS *= -Wl,--large-address-aware

HEADERS += \
    quaternionhelper.h \
    slicingengine.h \
    configuration.h \
    glmodel.h \
    qmlmanager.h \
    fileloader.h \
    mesh.h \
    lights.h \
    polyclipping/clipper.hpp \
    feature/autoorientation.h \
    feature/modelcut.h \
    feature/meshrepair.h \
    feature/arrange.h \
    feature/convex_hull.h \
    feature/labellingtextpreview.h \
    feature/text3dgeometrygenerator.h \
    utils/mathutils.h \
    utils/qbezier.h \
    utils/qdatabuffer_p.h \
    utils/qrbtree.h \
    utils/qtriangulator_p.h \
    utils/qvectorpath_p.h

#LIBS += -lOpengl32
