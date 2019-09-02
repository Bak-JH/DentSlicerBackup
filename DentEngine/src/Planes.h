#pragma once
#include <vector>
#include <unordered_set>
#include "mesh.h"
//namespace Hix
//{
//	namespace Engine3D
//	{
//		class Mesh;
//	}
//}

namespace Hix
{
	namespace Slicer
	{
		class Planes
		{
		public:
			virtual size_t minIntersectIdx(float z)const = 0;
			virtual size_t maxIntersectIdx(float z)const = 0;
			virtual const std::vector<float>& getPlanesVector()const = 0;
			virtual std::vector<std::unordered_set<Hix::Engine3D::FaceConstItr>> buildTriangleLists(const Hix::Engine3D::Mesh* mesh)const = 0;

		};

		class UniformPlanes : public Planes
		{
		public:
			UniformPlanes(float zMin, float zMax, float delta);
			size_t minIntersectIdx(float z)const override;
			size_t maxIntersectIdx(float z)const  override;
			const std::vector<float>& getPlanesVector()const override;
			std::vector<std::unordered_set<Hix::Engine3D::FaceConstItr>> buildTriangleLists(const Hix::Engine3D::Mesh* mesh)const override;

		private:
			std::vector<float> _planes;
			float _delta;
		};

		//class AdaptivePlanes : public Planes

	}
}