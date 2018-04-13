#ifndef EXTENSION_H
#define EXTENSION_H
#include "DentEngine/src/mesh.h"
#include "autoarrange.h"
#include "meshrepair.h"

void extendMesh(Mesh* mesh, MeshFace* mf, float distance);
void detectExtensionFaces(Mesh* mesh, QVector3D normal, MeshFace* mf, vector<MeshFace*>* result);
Path3D detectExtensionOutline(Mesh* mesh, vector<MeshFace*> meshfaces);
void extendAlongOutline(Mesh* mesh, QVector3D normal, Path3D selectedPath, float distance);

#endif // EXTENSION_H
