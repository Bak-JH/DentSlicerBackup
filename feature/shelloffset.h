#ifndef SHELLOFFSET_H
#define SHELLOFFSET_H

#include "DentEngine/src/mesh.h"
#include "glmodel.h"
#include "meshrepair.h"

void shellOffset(GLModel* glmodel, float factor);
void connectHoles(Mesh* target_mesh, Paths3D from_holes, Paths3D to_holes);

#endif // SHELLOFFSET_H
