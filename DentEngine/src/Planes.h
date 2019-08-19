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
			virtual void buildTriangleLists() = 0;
			virtual const std::vector<std::unordered_set<Hix::Engine3D::FaceConstItr>>&  getTrigList()const = 0;

		};

		class UniformPlanes : public Planes
		{
		public:
			UniformPlanes(const Hix::Engine3D::Mesh* mesh, float delta);
			size_t minIntersectIdx(float z)const override;
			size_t maxIntersectIdx(float z)const  override;
			const std::vector<float>& getPlanesVector()const override;
			void buildTriangleLists()override;
			const std::vector<std::unordered_set<Hix::Engine3D::FaceConstItr>>& getTrigList()const override;

		private:
			std::vector<float> _planes;
			std::vector<std::unordered_set<Hix::Engine3D::FaceConstItr>> _triangleList;
			const Hix::Engine3D::Mesh* _mesh;
			float _delta;
		};

		//class AdaptivePlanes : public Planes

	}
}