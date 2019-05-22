#ifndef EXTENSION_H
#define EXTENSION_H
#include "DentEngine/src/mesh.h"
#include "autoarrange.h"
#include "meshrepair.h"
#include <Qt3DRender>
void extendMesh(Mesh* mesh, FaceConstItr mf, double distance);
void detectExtensionFaces(const Mesh* mesh, QVector3D normal, FaceConstItr original_mf, FaceConstItr mf, std::vector<FaceConstItr>& result, std::vector<int>* result_idx);
Paths3D detectExtensionOutline(Mesh* mesh, const std::vector<FaceConstItr>& meshfaces);
void extendAlongOutline(Mesh* mesh, QVector3D normal, Paths3D selectedPath, double distance);
void coverCap(Mesh* mesh, QVector3D normal,const std::vector<FaceConstItr>& extension_faces, double distance);
void extendColorMesh(Mesh* mesh, FaceConstItr mf, Qt3DRender::QBuffer * colorbutter,std::vector<int>* extendFaces);
void resetColorMesh(Mesh* mesh, Qt3DRender::QBuffer * colorbuffer, std::vector<int> extendFaces);
#endif // EXTENSION_H
