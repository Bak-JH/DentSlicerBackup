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
    GLModel* glmodel;

private:

    QString filename;
    float x,y,z;
    int v_cnt;
    int f_cnt;
    Mesh* lmesh;
    Mesh* rmesh;
    Mesh* mesh;
    QNode* m_parent;


    void initialize(Mesh* mesh);
    void addVertex(QVector3D vertex);
    void addVertices(Mesh* mesh);
    void addVertices(vector<QVector3D> vertices);
    void addNormalVertices(vector<QVector3D> vertices);
    void addColorVertices(vector<QVector3D> vertices);
    void addIndexes(vector<int> vertices);
    void clearVertices();
    void onTimerUpdate();
    void makePlane();
    void delModel();
    void before_initiate();
//    void makePlane(QVector3D& v0, QVector3D& v1, QVector3D& v2);

    // bisects mesh into leftMesh, rightMesh divided by plane
    void bisectModel(Mesh* mesh, Plane plane, Mesh* leftMesh, Mesh* rightMesh);
    bool isLeftToPlane(Plane plane, QVector3D position);

public slots:
    void handlePickerClicked(Qt3DRender::QPickEvent*);
    void modelcut();



};

Qt3DRender::QAttribute *copyAttribute(Qt3DRender::QAttribute *oldAtt, QMap<Qt3DRender::QBuffer *, Qt3DRender::QBuffer *> &bufferMap);


#endif // GLMODEL_H
