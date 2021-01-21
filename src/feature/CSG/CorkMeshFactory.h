#pragma once
#include "../../cork/src/cork.h"
class QMatrix4x4;
namespace Hix
{
	namespace Engine3D
	{
		class Mesh;
	}
	namespace Features
	{
		namespace CSG
		{
			//just use ToWorld trans matrix
			CorkTriMesh toCorkMesh(const Hix::Engine3D::Mesh& mesh);
			CorkTriMesh toCorkMesh(const Hix::Engine3D::Mesh& mesh, const QMatrix4x4& tranMatrix);
			Hix::Engine3D::Mesh* toHixMesh(const CorkTriMesh& mesh);
		}
	}
}
