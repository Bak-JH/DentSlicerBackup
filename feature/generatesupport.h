#ifndef GENERATESUPPORT_H
#define GENERATESUPPORT_H
#include "glmodel.h"

class OverhangPoint_ // OverhangPoint 이미 존재
{
public:
    OverhangPoint_();
    OverhangPoint_(QVector3D point);
    OverhangPoint_(QVector3D point, bool top);
    OverhangPoint_(QVector3D point, bool meshInt, bool supportInt);
    OverhangPoint_(QVector3D point, float radius);

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
    vector<OverhangPoint_> overhangPoints;
    vector<OverhangPoint_> supportPoints;
    float supportRadiusMax = 1;
    float supportTipHeight = 0.1f; //크면 에러
    float minZ = 1;
    float minLength = 3;

    void generateSupport(GLModel* glmodel);

    void overhangDetect(Mesh* mesh);
    void pointOverhangDetect(Mesh* mesh);
    void faceOverhangDetect(Mesh* mesh);
    void edgeOverhangDetect(Mesh* mesh);

    void sortOverhangPoints();
    OverhangPoint_ coneNconeIntersection(Mesh* mesh, OverhangPoint_ coneApex1, OverhangPoint_ coneApex2);
    OverhangPoint_ coneNmeshIntersection(Mesh* mesh, OverhangPoint_ coneApex);
    float calculateRadius(float mesh_height, float bottom_height, float branch_length);
    // void generateTip(Mesh* mesh, OverhangPoint_* position, float height);
    void generateBottomFace(Mesh* mesh, OverhangPoint_* center);
    void generateBranch(Mesh* mesh, OverhangPoint_* leaf1, OverhangPoint_* leaf2, OverhangPoint_* stem);
    void generateNShapeBranch(Mesh* mesh, OverhangPoint_* pt1, OverhangPoint_* pt2, size_t diameter_mm, size_t thickness_mm);
    void generateStem(Mesh* mesh, OverhangPoint_* top, OverhangPoint_* bottom);
    // void generateStem(Mesh* mesh, OverhangPoint_* stem);
    // void mergeSupportMesh(GLModel* glmodel);
    QVector3D internalDiv(OverhangPoint_ a, OverhangPoint_ b, float m, float n);
};

#endif // GENERATESUPPORT_H
