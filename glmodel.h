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
#include "feature/stlexporter.h"
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
#define ftrSupport 14
#define ftrLabel 15

class GLModel;

class featureThread: public QThread
{
    Q_OBJECT
public:
    featureThread(GLModel* glmodel, int type);
    GLModel* m_glmodel;
    int progress;
    int optype; // defines typeofoperation
    QVariant data;
    autoorientation* ot;
    modelcut* ct;
    autoarrange* ar;
    STLexporter* ste;
    SlicingEngine* se;

signals:
    void loadPopup(QVariant value);

public slots:
    void setTypeAndRun(int type);
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

    // Core mesh structures
    Mesh* mesh;
    Mesh* lmesh;
    Mesh* rmesh;

    MeshFace *targetMeshFace = NULL; // used for object selection (specific area, like extension or labelling)

    bool appropriately_rotated=false;
    QPhongMaterial *m_meshMaterial;
    QPerVertexColorMaterial *m_meshVertexMaterial;

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
    QVector3D m_translation;

    // feature hollowshell
    float hollowShellRadius = 0;

    // feature extension
    vector<int> extendFaces;

    // feature offset
    double shellOffsetFactor;

    std::vector<QVector3D> cuttingPoints;
    Plane cuttingPlane;

    Qt3DExtras::QPlaneMesh* clipPlane[2];
    Qt3DCore::QEntity* planeEntity[2];
    Qt3DCore::QTransform *planeTransform[2];
    Qt3DExtras::QPhongAlphaMaterial *planeMaterial = nullptr;

    vector<Qt3DExtras::QSphereMesh*> sphereMesh;
    vector<Qt3DCore::QEntity*> sphereEntity;
    vector<Qt3DCore::QTransform*> sphereTransform;
    vector<QPhongMaterial*> sphereMaterial;

    void removeModel();
    void removeModelPartList();

    LabellingTextPreview* labellingTextPreview = nullptr;
    // changeColor
    void changecolor(int mode); //0 default, 1 selected, 2 outofarea


    // Model Mesh move
    void moveModelMesh(QVector3D direction);
    // Model Mesh rotate
    void rotateModelMesh(int Axis, float Angle);
    void rotateModelMesh(QMatrix4x4 matrix);
    // Model Mesh scale
    void scaleModelMesh(float scale);
    // Model Cut
    void addCuttingPoint(QVector3D v);
    void removeCuttingPoints();
    void drawLine(QVector3D endpoint);
    //void bisectModel(Mesh* mesh, Plane plane, Mesh* leftMesh, Mesh* rightMesh);
    //void bisectModel_internal(Mesh* mesh, Plane plane, Mesh* leftMesh, Mesh* rightMesh);
    void bisectModel(Plane plane);
    void bisectModel_internal(Plane plane);
    void checkPrintingArea();
    bool EndsWith(const string& a, const string& b);
    QString getFileName(const string& s);
    QVector3D spreadPoint(QVector3D endpoint,QVector3D startpoint,int factor);


    featureThread* ft;
    //arrangeSignalSender* arsignal; //unused, signal from qml goes right into QmlManager.runArrange

    int ID; //for use in Part List
    static int globalID;
    QString filename;
    QObject* mainWindow;
    void addVertices(Mesh* mesh, bool CW);

private:
    int colorMode;
    float x,y,z;
    int v_cnt;
    int f_cnt;
    QNode* m_parent;
    QVector3D lastpoint;
    void initialize(const Mesh* mesh);
    void applyGeometry();
    void addVertex(QVector3D vertex);
    void addVertices(vector<QVector3D> vertices);
    void addNormalVertices(vector<QVector3D> vertices);
    void addColorVertices(vector<QVector3D> vertices);
    void addIndexes(vector<int> vertices);
    void clearVertices();
    void onTimerUpdate();
    Mesh* toSparse(Mesh* mesh);

    int cutMode = 1;
    int cutFillMode = 1;
    bool labellingActive = false;
    bool extensionActive = false;
    bool cutActive = false;
    bool hollowShellActive = false;
    bool shellOffsetActive = false;
    bool layflatActive = false;

signals:

    void modelSelected(int);
    void resetLayflat();
    void bisectDone();
    void _updateModelMesh();
    void layFlatSelect();
    void layFlatUnSelect();
    void extensionSelect();
    void extensionUnSelect();

public slots:
    // Model Undo & Redo
    void savePrevState();
    void loadPrevState();
    void loadNextState();

    // object picker parts
    void handlePickerClicked(Qt3DRender::QPickEvent*);
    void handlePickerClickedLayflat(MeshFace shadow_meshface);
    void mgoo(Qt3DRender::QPickEvent*);
    void pgoo(Qt3DRender::QPickEvent*);
    void rgoo(Qt3DRender::QPickEvent*);
    void engoo();
    void exgoo();


    // Lay Flat
    void openLayflat();
    void closeLayflat();
    void generateLayFlat();

    // Model Cut
    void generatePlane();
    void removePlane();
    void modelCut();
    void cutModeSelected(int type);
    void cutFillModeSelected(int type);
    void getSliderSignal(double value);
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

    // Model Mesh info update
    void updateModelMesh();
};


#endif // GLMODEL_H
