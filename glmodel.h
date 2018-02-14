#ifndef GLMODEL_H
#define GLMODEL_H

#include <QObject>
#include <Qt3DCore>
#include <Qt3DRender>
#include <Qt3DExtras>
#include <Qt3DInput>
#include <vector>
#include "fileloader.h"
#include "arrange.h"
#include <QBuffer>
#include <QObjectPicker>
#include "qmlmanager.h"
#include <QCursor>
#include "mesh.h"

#define MAX_BUF_LEN 2000000

using namespace Qt3DCore;
using namespace Qt3DRender;
using namespace Qt3DExtras;
using namespace std;

// plane contains at least 3 vertices contained in the plane in clockwise direction
typedef vector<QVector3D> Plane;

class GLModel : public QEntity
{
    Q_OBJECT
public:
    GLModel(QNode* parent=nullptr);
    GLModel(QNode* parent,int p);
    ~GLModel();
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
    Qt3DRender::QMesh *m_mesh;
    Qt3DCore::QTransform *m_transform;

    std::vector<QVector3D> vector_set;

    Qt3DCore::QEntity *motherEntity;
    Qt3DExtras::QPlaneMesh* ClipPlane[2];
    Qt3DCore::QTransform *planeTransform[2];
    Qt3DCore::QEntity* planeEntity[2];
    Qt3DExtras::QPhongMaterial *planeMaterial;
    Qt3DExtras::QSphereMesh *sphereMesh[4];
    Qt3DCore::QTransform *sphereTransform[4];
    QPhongMaterial *sphereMaterial[4];
    Qt3DCore::QEntity *sphereEntity[4];
    void delModel();
    void before_initiate();
    void before_add_verticies();
    void makePlane();
    void pushPoint(QVector3D v);
    void delPoints();
    void drawLine(QVector3D endpoint);
    QVector3D goWider(QVector3D endpoint,QVector3D startpoint,int factor);
private:
    bool flatState;
    bool curveState;
    QString filename;
    float x,y,z;
    int v_cnt;
    int f_cnt;
    Mesh* lmesh;
    Mesh* rmesh;
    Mesh* mesh;
    QNode* m_parent;
    QVector3D lastpoint;
    int Number_of_points;
    void initialize(const Mesh* mesh);
    void addVertex(QVector3D vertex);
    void addVertices(Mesh* mesh);
    void addVertices(vector<QVector3D> vertices);
    void addNormalVertices(vector<QVector3D> vertices);
    void addColorVertices(vector<QVector3D> vertices);
    void addIndexes(vector<int> vertices);
    void clearVertices();
    void onTimerUpdate();
    Mesh* Tolower(Mesh* mesh);


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
    void Lineaccept();
    void Pointaccept();
    void getsignal(double value);
};

Qt3DRender::QAttribute *copyAttribute(Qt3DRender::QAttribute *oldAtt, QMap<Qt3DRender::QBuffer *, Qt3DRender::QBuffer *> &bufferMap);


#endif // GLMODEL_H
