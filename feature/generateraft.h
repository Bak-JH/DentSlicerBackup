#ifndef GENERATERAFT_H
#define GENERATERAFT_H
#include "glmodel.h"

class GenerateRaft
{
public:
    GenerateRaft();

    Mesh* generateRaft(Mesh* shellmesh, std::vector<OverhangPoint> overhangPoints);

    void generateFaces(Mesh* mesh, OverhangPoint top, OverhangPoint bottom);
    void generateCap(Mesh* mesh, OverhangPoint top, OverhangPoint bottom);
    void generateCylinder(Mesh* mesh, OverhangPoint op, float height);
    void generateKCylinder(Mesh* mesh, OverhangPoint op, float height);
};

#endif // GENERATERAFT_H
