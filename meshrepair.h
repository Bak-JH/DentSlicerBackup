#ifndef MESHREPAIR_H
#define MESHREPAIR_H
#include "mesh.h"
#include <QDebug>

void repairMesh(Mesh* mesh);

// removes unconnected vertices and faces

// if 2 edges are unconnected or 1 edge is unconnected, goto fillhole
void removeUnconnected(Mesh* mesh);

// removes zero area triangles
void removeDegenerate(Mesh* mesh);

// find hole edges(edges along the holes) and make contour from it
void identifyHoles(Mesh* mesh);


// detects hole and remove them
void fillHoles(Mesh* mesh);

// detects orientation defects, which induces normal vector errors and render errors
void fixNormalOrientations(Mesh* mesh);

// future works
void removeGaps(Mesh* mesh);

/*---------------- helper functions -----------------*/

// divide angle per 60 deg
int suggestDivisionCnt(Path3D e1, Path3D e2);

// convert mesh vertex to hash value
uint32_t meshVertex2Hash(MeshVertex u);

// construct closed contour using segments created from identify step
Paths3D contourConstruct(Paths3D hole_edges);

#endif // MESHREPAIR_H
