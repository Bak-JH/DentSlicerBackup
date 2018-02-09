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

Paths spreadingCheck(Mesh* mesh, bool* check, int chking_start);
int getPathHead(MeshFace* mf, int side);
Path buildOutline(Mesh* mesh, bool* check, bool* outer_check, int chking, int path_head);
bool isEdgeBound(MeshFace* mf, int side);
Path idxsToPath(Mesh* mesh, vector<int> path_by_idx);
Paths project(Mesh* mesh);
Paths clipOutlines(vector<Paths> outline_sets);
Mesh pathsToMesh(Paths paths);
Paths resize(Paths paths);
void debugPath(Paths paths);
#endif // ARRANGE_H
