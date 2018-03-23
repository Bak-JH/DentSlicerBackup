#ifndef GLMODEL_H
#define GLMODEL_H

#include <QObject>
#include <Qt3DCore>
#include <Qt3DRender>
#include <Qt3DExtras>
#include <Qt3DInput>
#include <vector>
#include <QBuffer>
#include <QObjectPicker>
#include <QCursor>
#include "mesh.h"
#include "fileloader.h"
#include "feature/modelcut.h"
#include "feature/labellingtextpreview.h"
#include "feature/autoorientation.h"
#include "feature/meshrepair.h"
#include <iostream>

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
    autoorientation* ot;
    modelcut* ct;

signals:
    void loadPopup(QVariant value);
    void setProgress(QVariant value);
public slots:
    void markPopup(bool value);
    void progressChanged(float value);
    void setTypeAndStart(int type);
private:
    void run() Q_DECL_OVERRIDE;
};

class GLModel : public QEntity
{
    Q_OBJECT
public:
    // load teeth model default
    GLModel(QNode* parent=nullptr, Mesh* loadMesh=nullptr, QString fname="", bool isShadow=false); // main constructor for mainmesh and shadowmesh
    ~GLModel();

    GLModel *parentModel;
    GLModel *shadowModel; // GLmodel's sparse mesh that gets picker input

    QThread* ownerThread;

    bool appropriately_rotated=false;
    QPhongMaterial *m_meshMaterial;
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

    //Qt3DCore::QEntity *parentEntity;

    std::vector<QVector3D> cuttingPoints;

    Qt3DExtras::QPlaneMesh* clipPlane[2];
    Qt3DCore::QEntity* planeEntity[2];
    Qt3DCore::QTransform *planeTransform[2];
    Qt3DExtras::QPhongAlphaMaterial *planeMaterial;

    Qt3DExtras::QSphereMesh *sphereMesh[4];
    Qt3DCore::QEntity *sphereEntity[4];
    Qt3DCore::QTransform *sphereTransform[4];
    QPhongMaterial *sphereMaterial[4];

    void removeModel();
    void beforeInitialize();
    void beforeAddVerticies();


    LabellingTextPreview* labellingTextPreview = nullptr;

    // Model move
    void moveModelMesh(QVector3D direction);
    // Model Cut
    void addCuttingPoint(QVector3D v);
    void removeCuttingPoints();
    void drawLine(QVector3D endpoint);
    void bisectModel(Mesh* mesh, Plane plane, Mesh* leftMesh, Mesh* rightMesh);
    bool isLeftToPlane(Plane plane, QVector3D position);


    QString getFileName(const string& s);
    QVector3D spreadPoint(QVector3D endpoint,QVector3D startpoint,int factor);

    Mesh* mesh;
    Mesh* lmesh;
    Mesh* rmesh;

    featureThread* ft;

    int ID; //for use in Part List
    static int globalID;
    static QObject* mainWindow;

private:
    int numPoints;
    bool flatState;
    bool curveState;
    QString filename;
    float x,y,z;
    int v_cnt;
    int f_cnt;
    Mesh* sparseMesh;
    QNode* m_parent;
    QVector3D lastpoint;
    void initialize(const Mesh* mesh);
    void addVertex(QVector3D vertex);
    void addVertices(Mesh* mesh);
    void addVertices(vector<QVector3D> vertices);
    void addNormalVertices(vector<QVector3D> vertices);
    void addColorVertices(vector<QVector3D> vertices);
    void addIndexes(vector<int> vertices);
    void clearVertices();
    void onTimerUpdate();
    Mesh* toSparse(Mesh* mesh);

    bool labelingActive = false;


public slots:
    // object picker parts
    void handlePickerClicked(Qt3DRender::QPickEvent*);
    void mgoo(Qt3DRender::QPickEvent*);
    void pgoo(Qt3DRender::QPickEvent*);
    void rgoo(Qt3DRender::QPickEvent*);
    void engoo();
    void exgoo();

    // Model Cut
    void generatePlane();
    void removePlane();
    void modelCut();
    void lineAccept();
    void pointAccept();
    void getSliderSignal(double value);

    // Labelling
    void getTextChanged(QString text, int contentWidth);
    void openLabelling();
    void closeLabelling();
    void getFontNameChanged(QString fontName);
    void generateText3DMesh();
};



#endif // GLMODEL_H
