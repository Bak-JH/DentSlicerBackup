#pragma once
#include "../../cork/src/cork.h"
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
			CorkTriMesh toCorkMesh(const Hix::Engine3D::Mesh& mesh);
			Hix::Engine3D::Mesh* toHixMesh(const CorkTriMesh& mesh);
		}
	}
}
