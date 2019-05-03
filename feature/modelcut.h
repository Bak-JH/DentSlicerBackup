#ifndef MODELCUT_H
#define MODELCUT_H
#include "DentEngine/src/mesh.h"
#include "feature/convex_hull.h"
#include "DentEngine/src/utils/metric.h"
#include <QObject>
#include <Qt3DCore>
#include <Qt3DRender>
#include <Qt3DExtras>
#include <Qt3DInput>
#include <QEntity>

class modelcut : public QObject {
    Q_OBJECT
public:
    modelcut();
    void bisectModel(Mesh* mesh, Plane plane, Mesh* leftMesh, Mesh* rightMesh);
    void generatePlane(Qt3DCore::QEntity* targetEntity);
    void addCuttingPoint(Qt3DCore::QEntity* targetEntity, QVector3D v);
    void removeCuttingPoints();
    //rotateResult* Tweak(Mesh* mesh, bool bi_algorithmic,int CA,bool *appropriately_rotated);
    //float approachvertex(Mesh* mesh,float n[]);
    Qt3DExtras::QSphereMesh *sphereMesh[4];
    Qt3DCore::QEntity *sphereEntity[4];
    Qt3DCore::QTransform *sphereTransform[4];
    Qt3DExtras::QPhongMaterial *sphereMaterial[4];
    std::vector<QVector3D> cuttingPoints;

    Qt3DExtras::QPlaneMesh* clipPlane[2];
    Qt3DCore::QEntity* planeEntity[2];
    Qt3DCore::QTransform *planeTransform[2];
    Qt3DExtras::QPhongAlphaMaterial *planeMaterial;

    static bool isLeftToPlane(Plane plane, QVector3D position);

    static void interpolate(Mesh* mesh, Path3D contour1, Path3D contour2);
    static void cutAway(Mesh* leftMesh, Mesh* rightMesh, Mesh* mesh, std::vector<QVector3D> cuttingPoints, int cutFillMode);

	Path _convexHull;
    int numPoints;
//private:
    //float target_function(float touching,float overhang,float line);
    //float* lithograph(Mesh* mesh, float n[], float amin, int CA);
    //float get_touching_line(Mesh* mesh,float a[],int i,float touching_height);
    //Orient* area_cumulation(Mesh* mesh,float n[],bool bi_algorithmic);
    //Orient* egde_plus_vertex(Mesh* mesh, int bsvest_n);
    //float* calc_random_normal(Mesh* mesh,int i);
    //Orient* remove_duplicates(Orient* o,int *orientCnt);
    //rotateResult* euler(Liste bestside);
signals:
    void progressChanged(float);

public slots:
    void getSliderSignal(double value);
};


#endif // MODELCUT_H
