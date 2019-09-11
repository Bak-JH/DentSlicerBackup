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



    static void interpolate(Hix::Engine3D::Mesh* mesh, Hix::Engine3D::Path3D contour1, Hix::Engine3D::Path3D contour2);
    static void cutAway(Hix::Engine3D::Mesh* leftMesh, Hix::Engine3D::Mesh* rightMesh, Hix::Engine3D::Mesh* mesh, std::vector<QVector3D> cuttingPoints, int cutFillMode);

	Path _convexHull;
    int numPoints;

};


void interpolate(Hix::Engine3D::Mesh* mesh, Hix::Engine3D::Path3D contour1, Hix::Engine3D::Path3D contour2);
void bisectModelByPlane(Hix::Engine3D::Mesh* leftMesh, Hix::Engine3D::Mesh* rightMesh, Hix::Engine3D::Mesh* mesh, float plane, int cutFillMode);
void cutAway(Hix::Engine3D::Mesh* leftMesh, Hix::Engine3D::Mesh* rightMesh, Hix::Engine3D::Mesh* mesh, std::vector<QVector3D> cuttingPoints, int cutFillMode);


#endif // MODELCUT_H
