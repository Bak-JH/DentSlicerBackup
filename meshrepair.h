#ifndef MESHREPAIR_H
#define MESHREPAIR_H
#include "mesh.h"

// removes unused vertices
void removeUnconnectedVertices(Mesh& mesh);

// removes totally unconnected faces
// if 2 edges are unconnected or 1 edge is unconnected, goto fillhole
void removeUnconnectedFaces(Mesh& mesh);

// removes zero area triangles
void removeDegeneracy(Mesh& mesh);

// detects hole and remove them
void fillHoles(Mesh& mesh);

// detects orientation defects, which induces normal vector errors and render errors
void fixNormalOrientations(Mesh& mesh);

// future works
void removeGaps(Mesh& mesh);

#endif // MESHREPAIR_H
