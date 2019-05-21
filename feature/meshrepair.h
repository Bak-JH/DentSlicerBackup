#ifndef MESHREPAIR_H
#define MESHREPAIR_H
#include "DentEngine/src/mesh.h"
#include <QDebug>
#include <QCoreApplication>
using namespace Hix::Engine3D;

namespace MeshRepair{
    void repairMesh(Mesh* mesh);

    // removes unconnected vertices and faces

    // if 2 edges are unconnected or 1 edge is unconnected, goto fillhole
    void removeUnconnected(Mesh* mesh);

    // removes zero area triangles
    void removeDegenerate(Mesh* mesh);

    // find hole edges(edges along the holes) and make contour from it
    Paths3D identifyHoles(const Mesh* mesh);


    // detects hole and remove them
    void fillHoles(Mesh* mesh, const Paths3D& holes);

    // fill hole path with faces
    std::vector<std::array<QVector3D, 3>> fillPath(Path3D path);

    // detects orientation defects, which induces normal std::vector errors and render errors
    void fixNormalOrientations(Mesh* mesh);

    // future works
    void removeGaps(Mesh* mesh);

    /*---------------- helper functions -----------------*/

    // divide angle per 60 deg
    int suggestDivisionCnt(Path3D e1, Path3D e2);

};



#endif // MESHREPAIR_H
