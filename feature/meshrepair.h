#ifndef MESHREPAIR_H
#define MESHREPAIR_H
#include "DentEngine/src/mesh.h"
#include <QDebug>
#include <QCoreApplication>

class MeshRepair
{
public:
    static void repairMesh(Mesh* mesh);

    // removes unconnected vertices and faces

    // if 2 edges are unconnected or 1 edge is unconnected, goto fillhole
    static void removeUnconnected(Mesh* mesh);

    // removes zero area triangles
    static void removeDegenerate(Mesh* mesh);

    // find hole edges(edges along the holes) and make contour from it
    static Paths3D identifyHoles(const Mesh* mesh);


    // detects hole and remove them
    static void fillHoles(Mesh* mesh, const Paths3D& holes);

    // fill hole path with faces
    static std::vector<std::array<QVector3D, 3>> fillPath(Path3D path);

    // detects orientation defects, which induces normal std::vector errors and render errors
    static void fixNormalOrientations(Mesh* mesh);

    // future works
    static void removeGaps(Mesh* mesh);

    /*---------------- helper functions -----------------*/

    // divide angle per 60 deg
    static int suggestDivisionCnt(Path3D e1, Path3D e2);

};



#endif // MESHREPAIR_H
