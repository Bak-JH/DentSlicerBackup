#ifndef HOLLOWSHELL_H
#define HOLLOWSHELL_H
#include "DentEngine/src/mesh.h"

class HollowShell
{
public:
	static void hollowShell(Mesh* mesh, MeshFace* mf, QVector3D center, float radius);
	static void detectHollowShellFaces(Mesh* mesh, QVector3D normal, MeshFace* original_mf, MeshFace* mf, std::vector<MeshFace*>* result);
	static void offsetHollowShellFaces(Mesh* mesh, std::vector<MeshFace*>* hollow_shell_faces, QVector3D center, float radius);

};

#endif // HOLLOWSHELL_H
