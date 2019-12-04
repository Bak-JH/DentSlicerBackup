#pragma once

namespace Hix
{
	namespace Engine3D
	{
		class Mesh;
	}
	namespace Features
	{
		class PoissonRemesh
		{
		public:
			Engine3D::Mesh* remesh(const Engine3D::Mesh& mesh);
		};

	}
}

