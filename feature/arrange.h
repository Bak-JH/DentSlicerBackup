#ifndef ARRANGE_H
#define ARRANGE_H
#include <vector>
#include <QVector3D>
#include <QHash>
#include "./configuration.h"
#include "./mesh.h"
#include "./polyclipping/clipper.hpp"

using namespace std;
using namespace ClipperLib;

class Arrange
{
public:
    Arrange();
};

Paths spreadingCheck(Mesh* mesh, bool* check, int chking_start, bool is_chking_pos);
int getPathHead(MeshFace* mf, int side, bool is_chking_pos);
Path buildOutline(Mesh* mesh, bool* check, int chking, int path_head, bool is_chking_pos);
bool isEdgeBound(MeshFace* mf, int side, bool is_chking_pos);
bool isNbrOrientSame(MeshFace* mf, int side);
int searchVtxInFace(MeshFace* mf, int vertexIdx);
vector<int> arrToVect(int arr[]);
int getNbrVtx(MeshFace* mf, int base, int xth);
Path idxsToPath(Mesh* mesh, vector<int> path_by_idx);
Paths project(Mesh* mesh);
Paths clipOutlines(vector<Paths> outline_sets);
bool checkFNZ(MeshFace* face, bool is_chking_pos);
void debugPath(Paths paths);
void debugFaces(Mesh* mesh, vector<int> face_list);
void debugFace(Mesh* mesh, int face_idx);
int findVertexWithIntpoint(IntPoint p, Mesh* mesh);
int findVertexWithIntXY(int x, int y, Mesh* mesh);
#endif // ARRANGE_H
