#ifndef EXTENSION_H
#define EXTENSION_H
#include "DentEngine/src/mesh.h"
#include "autoarrange.h"
#include "meshrepair.h"
#include <Qt3DRender>
void extendMesh(Mesh* mesh, MeshFace* mf, double distance);
void detectExtensionFaces(Mesh* mesh, QVector3D normal, MeshFace* original_mf, MeshFace* mf, std::vector<MeshFace*>* result, std::vector<int>* result_idx);
Paths3D detectExtensionOutline(Mesh* mesh, std::vector<MeshFace*> meshfaces);
void extendAlongOutline(Mesh* mesh, QVector3D normal, Paths3D selectedPath, double distance);
void coverCap(Mesh* mesh, QVector3D normal, std::vector<MeshFace*> extension_faces, double distance);
void extendColorMesh(Mesh* mesh, MeshFace* mf, Qt3DRender::QBuffer * colorbutter,std::vector<int>* extendFaces);
void resetColorMesh(Mesh* mesh, Qt3DRender::QBuffer * colorbuffer, std::vector<int> extendFaces);
#endif // EXTENSION_H
