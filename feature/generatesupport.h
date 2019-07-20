#ifndef GENERATESUPPORT_H
#define GENERATESUPPORT_H
#include "glmodel.h"

class OverhangPoint
{
public:
    OverhangPoint();
    OverhangPoint(QVector3D point);
    OverhangPoint(QVector3D point, bool top, bool supportInt, bool meshInt);
    OverhangPoint(QVector3D point, bool top, bool supportInt, bool meshInt, float radius);

    QVector3D position;
    bool topPoint = false;
    bool supportInterPoint = false;
    bool meshInterPoint = false;
    float radius = scfg->support_radius_min; // support tip radius 역할
};
namespace Hix
{
	namespace Debug
	{
		QDebug operator<< (QDebug d, const OverhangPoint& obj);
	}
}
class GenerateSupport
{
public:
    GenerateSupport();

    double critical_angle = 45;
    double critical_angle_radian = M_PI * (critical_angle / 180.0);
    double critical_angle_mesh = 30; // critical angle for cone&mesh intersection
    double critical_angle_mesh_radian = M_PI * (critical_angle_mesh / 180.0);
    std::vector<OverhangPoint> overhangPoints;
    std::vector<OverhangPoint> supportPoints;
    float supportRadiusMax = scfg->support_radius_max; // support bottom radius 역할
    float supportRadiusMin = scfg->support_radius_min; // support tip radius 역할
    float z_min;
    float z_min_minimal_diff = scfg->layer_height/2;
    float minLength = scfg->support_base_height+1;
    float near_dis = 3;
    float near_z = 1;
    float area_subdiv = 10;
    float tip_len = 3;

	//set minimum position for support so that it overlaps little bit(layer thickness)
	float getSupportZMin(const Mesh* mesh)const;
    Mesh* generateStraightSupport(Mesh* shellmesh);
    Mesh* generateSupport(Mesh* shellMesh);

    void overhangDetect(Mesh* mesh, Mesh* support_mesh);
    void pointOverhangDetect(Mesh* mesh, Mesh* support_mesh);
    void faceOverhangDetect(Mesh* mesh, Mesh* support_mesh);
    void faceOverhangPoint(Mesh *mesh, Mesh* support_mesh, QVector3D v0, QVector3D v1, QVector3D v2);

    void generateTip(Mesh* mesh, Mesh* support_mesh, QVector3D point);
    void findNearestPoint(size_t index);
    OverhangPoint coneNconeIntersection(OverhangPoint coneApex1, OverhangPoint coneApex2);
    OverhangPoint coneNmeshIntersection(Mesh* mesh, OverhangPoint coneApex);
    float calculateRadius(/*float mesh_height, float bottom_height,*/ float branch_length);
    void generateTopFace(Mesh* mesh, OverhangPoint center);
    void generateBottomFace(Mesh* mesh, OverhangPoint center);
    void generateFaces(Mesh* mesh, OverhangPoint top, OverhangPoint bottom);
    void generateBranch(Mesh* mesh, OverhangPoint leaf1, OverhangPoint leaf2, OverhangPoint* stem);

    void generateStem(Mesh* mesh, OverhangPoint top, OverhangPoint* bottom);
    QVector3D internalDiv(OverhangPoint a, OverhangPoint b, float m, float n);
};

#endif // GENERATESUPPORT_H
