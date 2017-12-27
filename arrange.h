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

void SpreadingCheck(Mesh* mesh, bool* check, int chking, std::vector<std::pair<int, int>>* edge_set);
void GetOutline (Mesh* mesh, std::vector<std::pair<int, int>> edge_set, Paths* outline_set);
Paths SpreadingCheck1(Mesh* mesh, bool* check, bool* outer_check, int chking);
Path GetOutline1(Mesh* mesh, bool* check, bool* outer_check, int chking, int path_head);
bool IsSideBound(Mesh* mesh, MeshFace mf, int neighbor_idx);
Paths Projection(Mesh* mesh);
Mesh PathsToMesh(Paths paths);
#endif // ARRANGE_H
