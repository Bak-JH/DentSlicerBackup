.TEMPLATE = app

QT += qml quick opengl 3dcore 3drender 3dinput 3dextras concurrent widgets core 3dquick 3dlogic 3dquickextras

CONFIG += c++17 resources_big

TARGET = DentStudio


RC_ICONS = icon-32.ico
RC_FILE = DLPslicer.rc

SOURCES += main.cpp \
    meshdatacontainer.cpp \
    raycastcontroller.cpp \
    slicingengine.cpp \
    glmodel.cpp \
    qmlmanager.cpp \
    fileloader.cpp \
    lights.cpp \
    feature/autoorientation.cpp \
    feature/modelcut.cpp \
    feature/meshrepair.cpp \
    feature/autoarrange.cpp \
    feature/convex_hull.cpp \
    feature/text3dgeometrygenerator.cpp \
    feature/labellingtextpreview.cpp \
    utils/quaternionhelper.cpp \
    utils/mathutils.cpp \
    utils/qtriangulator.cpp \
    feature/stlexporter.cpp \
    feature/extension.cpp \
    feature/shelloffset.cpp \
    DentEngine/src/cmdlineparser.cpp \
    DentEngine/src/configuration.cpp \
    DentEngine/src/mesh.cpp \
    DentEngine/src/slicer.cpp \
    DentEngine/src/svgexporter.cpp \
    DentEngine/src/polyclipping/clipper/clipper.cpp \
    DentEngine/src/polyclipping/clip2tri/clip2tri.cpp \
    DentEngine/src/polyclipping/poly2tri/common/shapes.cpp \
    DentEngine/src/polyclipping/poly2tri/sweep/advancing_front.cpp \
    DentEngine/src/polyclipping/poly2tri/sweep/cdt.cpp \
    DentEngine/src/polyclipping/poly2tri/sweep/sweep.cpp \
    DentEngine/src/polyclipping/poly2tri/sweep/sweep_context.cpp \
    utils/httpreq.cpp \
    feature/hollowshell.cpp \
    utils/updatechecker.cpp \
    utils/gridmesh.cpp \
    utils/linemeshgeometry.cpp \
    feature/generateraft.cpp \
    feature/generatesupport.cpp \
    DentEngine/src/utils/metric.cpp \
    feature/manualsupport.cpp


RESOURCES += qml.qrc \
    resources.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
#QML_IMPORT_PATH =
LIB_DIR=utils/winsparkle

INCLUDEPATH += $$_PRO_FILE_PWD_/$$LIB_DIR
LIBS += -L$$_PRO_FILE_PWD_/$$LIB_DIR -lWinSparkle
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


#TRANSLATIONS += lang_ko.ts

HEADERS += \
    boundingbox.h \
    common/Indexedlist.h \
    common/Singleton.h \
    common/TrackedIndexedList.h \
    common/indexedlist.h \
    raycastcontroller.h \
    slicingengine.h \
    glmodel.h \
    qmlmanager.h \
    fileloader.h \
    lights.h \
    feature/autoorientation.h \
    feature/modelcut.h \
    feature/meshrepair.h \
    feature/autoarrange.h \
    feature/convex_hull.h \
    feature/labellingtextpreview.h \
    feature/text3dgeometrygenerator.h \
    unitTest/catch.hpp \
    unitTest/tests/IndexedListTest.h \
    unitTest/tests/TrackedIndexedListTest.h \
    utils/quaternionhelper.h \
    utils/mathutils.h \
    utils/qbezier.h \
    utils/qdatabuffer_p.h \
    utils/qrbtree.h \
    utils/qtriangulator_p.h \
    utils/qvectorpath_p.h \
    utils/utils.h \
    utils/winsparkle/winsparkle.h \
    feature/stlexporter.h \
    feature/extension.h \
    feature/shelloffset.h \
    DentEngine/src/cmdlineparser.h \
    DentEngine/src/configuration.h \
    DentEngine/src/mesh.h \
    DentEngine/src/slicer.h \
    DentEngine/src/svgexporter.h \
    DentEngine/src/polyclipping/clipper/clipper.hpp \
    DentEngine/src/polyclipping/clip2tri/clip2tri.h \
    DentEngine/src/polyclipping/poly2tri/common/shapes.h \
    DentEngine/src/polyclipping/poly2tri/common/utils.h \
    DentEngine/src/polyclipping/poly2tri/sweep/advancing_front.h \
    DentEngine/src/polyclipping/poly2tri/sweep/cdt.h \
    DentEngine/src/polyclipping/poly2tri/sweep/sweep.h \
    DentEngine/src/polyclipping/poly2tri/sweep/sweep_context.h \
    DentEngine/src/polyclipping/poly2tri/poly2tri.h \
    utils/httpreq.h \
    feature/hollowshell.h \
    utils/updatechecker.h \
    feature/earcut.hpp \
    utils/gridmesh.h \
    utils/linemeshgeometry.h \
    feature/generateraft.h \
    feature/generatesupport.h \
    DentEngine/src/utils/metric.h \
    feature/manualsupport.h

#LIBS += -lOpengl32

DISTFILES += \
    icon-32.ico \
    DLPslicer.rc \
    icon.ico \
