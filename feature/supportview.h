#ifndef SUPPORTVIEW_H
#define SUPPORTVIEW_H

#include "DentEngine/src/mesh.h"
#include "DentEngine/src/support.h"

void generateSupporter(Mesh* mesh, OverhangPoint *point, OverhangPoint *parent = nullptr, vector<OverhangPoint *> *points = nullptr, float support_z_min=0.0f);
void generateFace(Mesh* mesh, Path path, float z);
void generateRaft(Mesh* mesh, Slices* slices);
void generateInfill(Mesh* mesh, Slice* slice);

#endif // SUPPORTVIEW_H
