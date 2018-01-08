#ifndef ARRANGE_H
#define ARRANGE_H
#include <vector>
#include <QVector3D>
#include <QHash>
#include "configuration.h"
#include "mesh.h"
#include "polyclipping/clipper.hpp"

using namespace std;
using namespace ClipperLib;

class Arrange
{
public:
    Arrange();
};

Paths SpreadingCheck(Mesh* mesh, bool* check, bool* outer_check, int chking);
Path GetOutline(Mesh* mesh, bool* check, bool* outer_check, int chking, int path_head);
bool IsEdgeBound(Mesh* mesh, MeshFace mf, int neighbor_idx);
Paths Projection(Mesh* mesh);
Mesh PathsToMesh(Paths paths);
Paths Resize(Paths paths);
void DebugPath(Paths paths);
#endif // ARRANGE_H
