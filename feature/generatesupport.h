#ifndef GENERATESUPPORT_H
#define GENERATESUPPORT_H
#include "glmodel.h"

class OverhangPoint
{
public:
    OverhangPoint();
    OverhangPoint(QVector3D point);
    OverhangPoint(QVector3D point, bool top);
    OverhangPoint(QVector3D point, bool meshInt, bool supportInt, float radius);
    OverhangPoint(QVector3D point, float radius);

    QVector3D position;
    bool topPoint = false;
    bool meshInterPoint = false;
    bool supportInterPoint = false;
    float radius = 0.1f; // support tip radius 역할
};

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
    float supportRadiusMax = 1;
    float supportTipHeight = 0.1f; //크면 에러
    float minZ = 1;
    float minLength = 3;
    float radiusMin = 0.1f; // support tip radius 역할

    Mesh* generateSupport(Mesh* shellMesh);

    void overhangDetect(Mesh* mesh);
    void pointOverhangDetect(Mesh* mesh);
    void faceOverhangDetect(Mesh* mesh);
    void faceOverhangPoint(Mesh *mesh, QVector3D v0, QVector3D v1, QVector3D v2);
    void edgeOverhangDetect(Mesh* mesh);

    void sortOverhangPoints();
    OverhangPoint coneNconeIntersection(Mesh* mesh, OverhangPoint coneApex1, OverhangPoint coneApex2);
    OverhangPoint coneNmeshIntersection(Mesh* mesh, OverhangPoint coneApex);
    float calculateRadius(float mesh_height, float bottom_height, float branch_length);
    void generateBottomFace(Mesh* mesh, OverhangPoint center);
    void generateFaces(Mesh* mesh, OverhangPoint top, OverhangPoint bottom);
    void generateBranch(Mesh* mesh, OverhangPoint leaf1, OverhangPoint leaf2, OverhangPoint* stem);
    void generateNShapeBranch(Mesh* mesh, OverhangPoint* pt1, OverhangPoint* pt2, size_t diameter_mm, size_t thickness_mm);
    void generateStem(Mesh* mesh, OverhangPoint top, OverhangPoint* bottom);
    QVector3D internalDiv(OverhangPoint a, OverhangPoint b, float m, float n);
};

#endif // GENERATESUPPORT_H
