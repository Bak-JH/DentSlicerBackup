#ifndef GENERATESUPPORT_H
#define GENERATESUPPORT_H
#include "glmodel.h"

class OverhangPoint // OverhangPoint 이미 존재
{
public:
    OverhangPoint();
    OverhangPoint(QVector3D point);
    OverhangPoint(QVector3D point, bool top);
    OverhangPoint(QVector3D point, bool meshInt, bool supportInt);
    OverhangPoint(QVector3D point, float radius);

    QVector3D position;
    bool topPoint = false;
    bool meshInterPoint = false;
    bool supportInterPoint = false;
    float radius = 0.1f; // supportRadiusMin 역할
};

class GenerateSupport
{
public:
    GenerateSupport();

    double critical_angle = 45;
    double critical_angle_radian = M_PI * (critical_angle / 180.0);
    vector<OverhangPoint> overhangPoints;
    vector<OverhangPoint> supportPoints;
    float supportRadiusMax = 1;
    float supportTipHeight = 0.1f; //크면 에러
    float minZ = 1;
    float minLength = 3;

    Mesh* generateSupport(Mesh* shellMesh);

    void overhangDetect(Mesh* mesh);
    void pointOverhangDetect(Mesh* mesh);
    void faceOverhangDetect(Mesh* mesh);
    void edgeOverhangDetect(Mesh* mesh);

    void sortOverhangPoints();
    OverhangPoint coneNconeIntersection(Mesh* mesh, OverhangPoint coneApex1, OverhangPoint coneApex2);
    OverhangPoint coneNmeshIntersection(Mesh* mesh, OverhangPoint coneApex);
    float calculateRadius(float mesh_height, float bottom_height, float branch_length);
    // void generateTip(Mesh* mesh, OverhangPoint* position, float height);
    void generateBottomFace(Mesh* mesh, OverhangPoint* center);
    void generateBranch(Mesh* mesh, OverhangPoint* leaf1, OverhangPoint* leaf2, OverhangPoint* stem);
    void generateNShapeBranch(Mesh* mesh, OverhangPoint* pt1, OverhangPoint* pt2, size_t diameter_mm, size_t thickness_mm);
    void generateStem(Mesh* mesh, OverhangPoint* top, OverhangPoint* bottom);
    // void generateStem(Mesh* mesh, OverhangPoint* stem);
    // void mergeSupportMesh(GLModel* glmodel);
    QVector3D internalDiv(OverhangPoint a, OverhangPoint b, float m, float n);
};

#endif // GENERATESUPPORT_H
