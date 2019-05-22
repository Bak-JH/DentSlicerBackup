#ifndef GLMODEL_H
#define GLMODEL_H

#include <QObject>
#include <QApplication>
#include <Qt3DCore>
#include <Qt3DRender>
#include <Qt3DExtras>
#include <Qt3DInput>
#include <vector>
#include <QBuffer>
#include <QObjectPicker>
#include <QCursor>
#include <iostream>
#include "DentEngine/src/mesh.h"
#include "fileloader.h"
#include "slicingengine.h"
#include "feature/modelcut.h"
#include "feature/labellingtextpreview.h"
#include "feature/autoorientation.h"
#include "feature/meshrepair.h"
#include "feature/autoarrange.h"
#include "feature/extension.h"
#include "feature/hollowshell.h"


#define MAX_BUF_LEN 2000000

using namespace Qt3DCore;
using namespace Qt3DRender;
using namespace Qt3DExtras;
using namespace std;

/* feature thread */
#define ftrOpen 1
#define ftrSave 2
#define ftrExport 3
#define ftrMove 4
#define ftrRotate 5
#define ftrLayFlat 6
#define ftrArrange 7
#define ftrOrient 8
#define ftrScale 9
#define ftrRepair 10
#define ftrCut 11
#define ftrShellOffset 12
#define ftrExtend 13
#define ftrManualSupport 14
#define ftrLabel 15
#define ftrSupportViewMode 16
#define ftrLayerViewMode 17
#define ftrDelete 18
#define ftrTempExport 19

class GLModel;
class OverhangPoint;

class featureThread: public QThread
{
    Q_OBJECT
public:
    featureThread(GLModel* glmodel, int type);
    GLModel* m_glmodel;
    int progress;
    int optype; // defines typeofoperation
    QVariant data;
    double arg1;
    double arg2;
    double arg3;
    autoorientation* ot;
    modelcut* ct;
    autoarrange* ar;
    STLexporter* ste;
    SlicingEngine* se;

signals:
    void loadPopup(QVariant value);

public slots:
    void setTypeAndRun(int type);
    void setTypeAndRun(int type, double arg1, double arg2, double arg3);
    void setTypeAndRun(int type, QVariant data);
    void setTypeAndStart(int type);
private:
    void run() Q_DECL_OVERRIDE;
};

class arrangeSignalSender: public QObject
{
    Q_OBJECT
public:
    arrangeSignalSender();
signals:
    void runArrange();
};

class GLModel : public QEntity
{
    Q_OBJECT
public:
    // load teeth model default
    GLModel(QObject* mainWindow=nullptr, QNode* parent=nullptr, Mesh* loadMesh=nullptr, QString fname="", bool isShadow=false); // main constructor for mainmesh and shadowmesh
    ~GLModel();

    GLModel *parentModel = NULL;
    GLModel *shadowModel = NULL; // GLmodel's sparse mesh that gets picker input
    GLModel *leftModel = NULL;
    GLModel *rightModel = NULL;
    GLModel *twinModel = NULL; // saves cut right for left, left for right models

    // Core mesh structures
    Mesh* mesh;
    Mesh* lmesh;
    Mesh* rmesh;
    QSphereMesh* dragMesh;
    Mesh* supportMesh = nullptr;
    Mesh* raftMesh = nullptr;

    // layer view
    Mesh* layerMesh;
    Mesh* layerSupportMesh;
    Mesh* layerRaftMesh;
    Mesh* layerInfillMesh;

    MeshFace *targetMeshFace = NULL; // used for object selection (specific area, like extension or labelling)

    bool appropriately_rotated=false;
    QPhongMaterial *m_meshMaterial;
    //QMaterial *m_meshMaterial;
    QPhongAlphaMaterial *m_meshAlphaMaterial;
    QPerVertexColorMaterial *m_meshVertexMaterial;
    QMaterial *m_layerMaterial;
    QParameter *m_layerMaterialHeight;
    QParameter *m_layerMaterialRaftHeight;

    //QPhongMaterial *m_meshMaterial;
    Qt3DRender::QBuffer *vertexBuffer;
    Qt3DRender::QBuffer *vertexNormalBuffer;
    Qt3DRender::QBuffer *vertexColorBuffer;
    Qt3DRender::QBuffer *indexBuffer;
    QAttribute *positionAttribute;
    QAttribute *normalAttribute;
    QAttribute *colorAttribute;
    QAttribute *indexAttribute;
    QGeometry* m_geometry;
    QGeometryRenderer* m_geometryRenderer;
    Qt3DRender::QObjectPicker *m_objectPicker;
    Qt3DCore::QTransform *m_transform;
    //QVector3D m_translation;

    // feature hollowshell
    float hollowShellRadius = 0;

    // feature extension
    vector<int> extendFaces;

    // feature offset
    double shellOffsetFactor;

    std::vector<QVector3D> cuttingPoints;
    vector<QEntity*> cuttingContourCylinders;
    Plane cuttingPlane;

    // used for layer view
    Qt3DExtras:: QPlaneMesh* layerViewPlane[1];
    Qt3DCore::QEntity* layerViewPlaneEntity[1];
    Qt3DCore::QTransform *layerViewPlaneTransform[1];
    Qt3DRender::QTextureLoader* layerViewPlaneTextureLoader;
    Qt3DExtras::QTextureMaterial* layerViewPlaneMaterial;
    //Qt3DExtras::QPhongAlphaMaterial *layerViewPlaneMaterial = nullptr;
    //QObjectPicker* planeObjectPicker[2];


    Qt3DExtras::QPlaneMesh* clipPlane[2];
    Qt3DCore::QEntity* planeEntity[2];
    Qt3DCore::QTransform *planeTransform[2];
    Qt3DExtras::QPhongAlphaMaterial *planeMaterial = nullptr;
    QObjectPicker* planeObjectPicker[2];

    vector<Qt3DExtras::QSphereMesh*> sphereMesh;
    vector<Qt3DCore::QEntity*> sphereEntity;
    vector<Qt3DCore::QTransform*> sphereTransform;
    vector<Qt3DRender::QObjectPicker*> sphereObjectPicker;
    vector<QPhongMaterial*> sphereMaterial;

    // Dragbox
    Qt3DExtras::QSphereMesh* dragSphere;
    //Qt3DExtras::QText2DEntity* dragSphere;
    //Qt3DExtras::QExtrudedTextMesh* dragSphere;
    //Qt3DExtras::QPlaneMesh* dragSphere;
    Qt3DCore::QEntity* dragEntity;
    Qt3DCore::QTransform* dragTransform;
    Qt3DExtras::QPhongAlphaMaterial *dragMaterial = nullptr;
    QObjectPicker *dragObjectPicker;

    void removeModel();
    void removeModelPartList();

    LabellingTextPreview* labellingTextPreview = nullptr;

    void copyModelAttributeFrom(GLModel* from);

    void addMouseHandlers();
    void removeMouseHandlers();

    // changeColor
    void changecolor(int mode); //0 default, 1 selected, 2 outofarea


    // Model Mesh move
    void moveModelMesh(QVector3D direction);
    // Model Mesh rotate
    void rotateModelMesh(int Axis, float Angle);
    void rotateModelMesh(QMatrix3x3 matrix);
    void rotateModelMesh(QMatrix4x4 matrix);
    // Model Mesh scale
    void scaleModelMesh(float scaleX, float scaleY, float scaleZ);
    // Model Cut
    void addCuttingPoint(QVector3D v);
    void removeCuttingPoint(int idx);
    void removeCuttingPoints();
    void drawLine(QVector3D endpoint);
    //void bisectModel(Mesh* mesh, Plane plane, Mesh* leftMesh, Mesh* rightMesh);
    //void bisectModel_internal(Mesh* mesh, Plane plane, Mesh* leftMesh, Mesh* rightMesh);
    void bisectModel(Plane plane);
    void bisectModel_internal(Plane plane);
    void checkPrintingArea();
    bool EndsWith(const string& a, const string& b);
    bool modelSelectChangable();
    QVector2D world2Screen(QVector3D target);
    QString getFileName(const string& s);
    QVector3D spreadPoint(QVector3D endpoint,QVector3D startpoint,int factor);
    void changeViewMode(int viewMode);
    void inactivateFeatures();

    // support
    Slicer* slicer;

    featureThread* ft;
    //arrangeSignalSender* arsignal; //unused, signal from qml goes right into QmlManager.runArrange
    QFutureWatcher<Slicer*> futureWatcher; // future watcher for feature thread results returned

    int ID; //for use in Part List
    static int globalID;
    QString filename;
    QObject* mainWindow;
    QString slicingInfo;

    // implement lock as bool variable
    bool updateLock;

    void addVertices(Mesh* mesh, bool CW, QVector3D color=QVector3D(0.278f, 0.670f, 0.706f));


private:
    int colorMode;
    float x,y,z;
    int v_cnt;
    int f_cnt;
    QNode* m_parent;
    QVector3D lastpoint;
    QVector2D prevPoint;
    void initialize(const int& faces_cnt);
    void applyGeometry();
    void addVertex(QVector3D vertex);
    void addVertices(vector<QVector3D> vertices);
    void addNormalVertices(vector<QVector3D> vertices);
    void addColorVertices(vector<QVector3D> vertices);
    void addIndexes(vector<int> vertices);
    void clearVertices();
    void onTimerUpdate();
    Mesh* toSparse(Mesh* mesh);
    void removeLayerViewComponents();
    void generateLayerViewMaterial();

    int cutMode = 1;
    int cutFillMode = 1;
    bool labellingActive = false;
    bool extensionActive = false;
    bool cutActive = false;
    bool hollowShellActive = false;
    bool shellOffsetActive = false;
    bool layflatActive = false;
    bool manualSupportActive = false;
    bool layerViewActive = false;
    bool supportViewActive = false;
    bool scaleActive = false;

    bool isMoved = false;
    bool isReleased = true;

    bool isFlatcutEdge = false;

    int viewMode = -1;

signals:

    void modelSelected(int);
    void resetLayflat();
    void bisectDone();
    void _generateSupport();
    void _updateModelMesh(bool);
    void layFlatSelect();
    void layFlatUnSelect();
    void extensionSelect();
    void extensionUnSelect();

public slots:
    // Model Undo & Redo
    void saveUndoState();
    void saveUndoState_internal();
    void loadUndoState();
    void loadRedoState();

    // object picker parts
    void handlePickerEnteredFreeCutSphere();
    void handlePickerExitedFreeCutSphere();
    void handlePickerClickedFreeCutSphere(Qt3DRender::QPickEvent*);
    void handlePickerClickedFreeCut(Qt3DRender::QPickEvent*);
    void handlePickerClicked(Qt3DRender::QPickEvent*);
    void handlePickerClickedLayflat(MeshFace shadow_meshface);
    void mgoo(Qt3DRender::QPickEvent*);
    void pgoo(Qt3DRender::QPickEvent*);
    void engoo();
    void exgoo();

    // Scale
    void openScale();
    void closeScale();

    // Lay Flat
    void openLayflat();
    void closeLayflat();
    void generateLayFlat();

    // Model Cut
    void removeCuttingContour();
    void generateCuttingContour(vector<QVector3D> cuttingContour);
    void regenerateCuttingPoint(vector<QVector3D> cuttingContour);
    void generateClickablePlane();
    void generatePlane();
    void removePlane();
    void modelCut();
    void cutModeSelected(int type);
    void cutFillModeSelected(int type);
    void getSliderSignal(double value);
    void getLayerViewSliderSignal(double value);
    void generateRLModel();
    void openCut();
    void closeCut();

    // Hollow Shell
    void indentHollowShell(double radius);
    void openHollowShell();
    void closeHollowShell();

    // Labelling
    void getTextChanged(QString text, int contentWidth);
    void openLabelling();
    void closeLabelling();
    void stateChangeLabelling();
    void getFontNameChanged(QString fontName);
    void getFontBoldChanged(bool isBold);
    void getFontSizeChanged(int fontSize);
    void applyLabelInfo(QString text, int contentWidth, QString fontName, bool isBold, int fontSize);
    void generateText3DMesh();

    // Extension
    void openExtension();
    void closeExtension();
    void colorExtensionFaces();
    void uncolorExtensionFaces();
    void generateColorAttributes();
    void generateExtensionFaces(double distance);

    // ShellOffset
    void openShellOffset();
    void closeShellOffset();
    void generateShellOffset(double factor);

    // Manual Support
    void openManualSupport();
    void closeManualSupport();
    void generateManualSupport();

    // Model Mesh info update
    void updateModelMesh(bool);

    // Generate support mesh
    void generateSupport();
    void slicingDone();

    // Generate Dragbox mesh
    void generateDragbox();
};


#endif // GLMODEL_H
