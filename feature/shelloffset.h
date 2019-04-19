#ifndef SHELLOFFSET_H
#define SHELLOFFSET_H

#include "DentEngine/src/mesh.h"
#include "glmodel.h"
#include "meshrepair.h"
class ShellOffset
{
public:
    static Mesh* shellOffset(Mesh* glmodel, float factor);
    static void connectHoles(Mesh* target_mesh, Paths3D from_holes, Paths3D to_holes);

};

#endif // SHELLOFFSET_H
