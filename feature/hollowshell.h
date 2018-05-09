#ifndef HOLLOWSHELL_H
#define HOLLOWSHELL_H
#include "DentEngine/src/mesh.h"

void hollowShell(Mesh* mesh, MeshFace* mf, QVector3D center, float radius);
void detectHollowShellFaces(Mesh* mesh, QVector3D normal, MeshFace* original_mf, MeshFace* mf, vector<MeshFace*>* result);
void offsetHollowShellFaces(Mesh* mesh, vector<MeshFace*>* hollow_shell_faces, QVector3D center, float radius);

#endif // HOLLOWSHELL_H
