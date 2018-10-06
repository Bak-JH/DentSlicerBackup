#ifndef SUPPORTVIEW_H
#define SUPPORTVIEW_H

#include "DentEngine/src/mesh.h"
#include "DentEngine/src/support.h"

void generateSupporter(Mesh* mesh, OverhangPoint *point, OverhangPoint *parent = nullptr, vector<OverhangPoint *> *points = nullptr);
void generateFace(Mesh* mesh, Path path, float z);
void generateRaft(Mesh* mesh, OverhangPoint *point);
void generateInfill(Mesh* mesh, Slice* slice);

#endif // SUPPORTVIEW_H
