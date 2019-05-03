#ifndef HOLLOWSHELL_H
#define HOLLOWSHELL_H
#include "DentEngine/src/mesh.h"

namespace HollowShell
{
	void hollowShell(Mesh* mesh,const MeshFace* mf, QVector3D center, float radius);
	void detectHollowShellFaces(Mesh* mesh, QVector3D normal, const MeshFace* original_mf,const MeshFace* mf, std::vector<const MeshFace*>* result);
	void offsetHollowShellFaces(Mesh* mesh, std::vector<const MeshFace*>* hollow_shell_faces, QVector3D center, float radius);

};

#endif // HOLLOWSHELL_H
