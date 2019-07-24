#pragma once

#include <qvector3d.h>
#include <vector>

namespace tf
{
	class Subflow;
}



namespace Hix
{
	namespace Engine3D
	{
		class Mesh;
	}
	namespace SupportGeneration
	{
		namespace PostSlice
		{

			std::vector<QVector3D> OverhangDetectPostSlice(const Engine3D::Mesh& shellMesh);
		}

	}
}

