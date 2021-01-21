#ifndef HOLLOWSHELL_H
#define HOLLOWSHELL_H
#include "Mesh/mesh.h"
#include <unordered_set>
namespace HollowShell
{
	void hollowShell(Hix::Engine3D::Mesh* mesh, const Hix::Engine3D::FaceConstItr& mf, QVector3D center, float radius);
	void detectHollowShellFaces(Hix::Engine3D::Mesh* mesh, QVector3D normal, const Hix::Engine3D::FaceConstItr& original_mf, const Hix::Engine3D::FaceConstItr& mf, std::unordered_set<Hix::Engine3D::FaceConstItr>& result);
	void offsetHollowShellFaces(Hix::Engine3D::Mesh * mesh, std::unordered_set<Hix::Engine3D::FaceConstItr>& hollow_shell_faces, QVector3D center, float radius);
};

#endif // HOLLOWSHELL_H


