#ifndef HOLLOWSHELL_H
#define HOLLOWSHELL_H
#include "DentEngine/src/mesh.h"
#include <unordered_set>

using namespace Hix::Engine3D;
namespace HollowShell
{
	void hollowShell(Mesh* mesh, const FaceConstItr& mf, QVector3D center, float radius);
	void detectHollowShellFaces(Mesh* mesh, QVector3D normal, const FaceConstItr& original_mf, const FaceConstItr& mf, std::unordered_set<FaceConstItr>& result);
	void offsetHollowShellFaces(Mesh * mesh, std::unordered_set<FaceConstItr>& hollow_shell_faces, QVector3D center, float radius);
};

#endif // HOLLOWSHELL_H


