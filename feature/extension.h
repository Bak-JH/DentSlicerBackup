#ifndef EXTENSION_H
#define EXTENSION_H
#include "DentEngine/src/mesh.h"
#include "autoarrange.h"
#include "meshrepair.h"

void extendMesh(Mesh* mesh, MeshFace* mf, double distance);
void detectExtensionFaces(Mesh* mesh, QVector3D normal, MeshFace* original_mf, MeshFace* mf, vector<MeshFace*>* result);
Paths3D detectExtensionOutline(Mesh* mesh, vector<MeshFace*> meshfaces);
void extendAlongOutline(Mesh* mesh, QVector3D normal, Paths3D selectedPath, double distance);
void coverCap(Mesh* mesh, QVector3D normal, vector<MeshFace*> extension_faces, double distance);

#endif // EXTENSION_H
