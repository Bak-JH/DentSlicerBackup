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
#include "feature/meshrepair.h"
#include "qmlmanager.h"
#include "mesh.h"
#include "fileloader.h"
#include "arrange.h"
#include "feature/labellingtextpreview.h"

#define MAX_BUF_LEN 2000000

using namespace Qt3DCore;
using namespace Qt3DRender;
using namespace Qt3DExtras;
using namespace std;



class GLModel : public QEntity
{
    Q_OBJECT
public:
    // load teeth model default
    GLModel(QNode* parent=nullptr, QString fname="", bool isShadow=false); // main constructor for mainmesh and shadowmesh
    ~GLModel();

    GLModel *shadowModel; // GLmodel's sparse mesh that gets picker input
    QPhongAlphaMaterial *m_planeMaterial;
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

    Qt3DCore::QEntity *parentEntity;

    std::vector<QVector3D> cuttingPoints;

    Qt3DExtras::QPlaneMesh* clipPlane[2];
    Qt3DCore::QEntity* planeEntity[2];
    Qt3DCore::QTransform *planeTransform[2];
    Qt3DExtras::QPhongMaterial *planeMaterial;

    Qt3DExtras::QSphereMesh *sphereMesh[4];
    Qt3DCore::QEntity *sphereEntity[4];
    Qt3DCore::QTransform *sphereTransform[4];
    QPhongMaterial *sphereMaterial[4];

    LabellingTextPreview* labellingTextPreview = nullptr;

    void removeModel();
    void beforeInitialize();
    void beforeAddVerticies();
    void generatePlane();
    void addCuttingPoint(QVector3D v);
    void removeCuttingPoints();
    void drawLine(QVector3D endpoint);

    QVector3D spreadPoint(QVector3D endpoint,QVector3D startpoint,int factor);
private:
    int numPoints;
    bool flatState;
    bool curveState;
    QString filename;
    float x,y,z;
    int v_cnt;
    int f_cnt;
    Mesh* lmesh;
    Mesh* rmesh;
    Mesh* mesh;
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


    // bisects mesh into leftMesh, rightMesh divided by plane
    void bisectModel(Mesh* mesh, Plane plane, Mesh* leftMesh, Mesh* rightMesh);
    bool isLeftToPlane(Plane plane, QVector3D position);

public slots:
    void handlePickerClicked(Qt3DRender::QPickEvent*);
    void mgoo(Qt3DRender::QPickEvent*);
    void pgoo(Qt3DRender::QPickEvent*);
    void rgoo(Qt3DRender::QPickEvent*);
    void engoo();
    void exgoo();
    void modelcut();
    void lineAccept();
    void pointAccept();
    void getSignal(double value);
    void getTextChanged(QString text);
    void openLabelling();
    void closeLabelling();
    void getFontNameChanged(QString fontName);
    void generateText3DMesh();
};

Qt3DRender::QAttribute *copyAttribute(Qt3DRender::QAttribute *oldAtt, QMap<Qt3DRender::QBuffer *, Qt3DRender::QBuffer *> &bufferMap);


#endif // GLMODEL_H
