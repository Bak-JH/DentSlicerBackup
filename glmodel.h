#ifndef GLMODEL_H
#define GLMODEL_H

#include <QObject>
#include <Qt3DCore>
#include <Qt3DRender>
#include <Qt3DExtras>
#include <Qt3DInput>
#include <vector>
#include "fileloader.h"
#include <QBuffer>
#include <QObjectPicker>

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
    GLModel(QNode* parent = nullptr);

    QPhongAlphaMaterial *m_planeMaterial;
    Qt3DRender::QBuffer *vertexBuffer;
    Qt3DRender::QBuffer *vertexNormalBuffer;
    QAttribute *positionAttribute;
    QAttribute * normalAttribute;
    QGeometry* m_geometry;
    QGeometryRenderer* m_geometryRenderer;


    Qt3DRender::QObjectPicker *m_objectPicker;
    Qt3DRender::QMesh *m_mesh;
    Qt3DCore::QTransform *m_transform;

private:


    QString filename;
    float x,y,z;
    int v_cnt;
    int f_cnt;

    Mesh* mesh;
    QNode* m_parent;


    void initialize();
    void addVertex(QVector3D vertex);
    void addVertices(Mesh* mesh);
    void addVertices(vector<QVector3D> vertices);
    void addNormalVertices(vector<QVector3D> vertices);
    void clearVertices();
    void onTimerUpdate();

    // bisects mesh into leftMesh, rightMesh divided by plane
    void bisectModel(Mesh* mesh, Plane plane, Mesh* leftMesh, Mesh* rightMesh);
    bool isLeftToPlane(Plane plane, QVector3D position);

public slots:
    void handlePickerClicked(Qt3DRender::QPickEvent*);
};

Qt3DRender::QAttribute *copyAttribute(Qt3DRender::QAttribute *oldAtt, QMap<Qt3DRender::QBuffer *, Qt3DRender::QBuffer *> &bufferMap);


#endif // GLMODEL_H
