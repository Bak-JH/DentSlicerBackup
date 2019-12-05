#pragma once
#include "../DentEngine/src/mesh.h"
#include <vector>
namespace Hix
{
	namespace Engine3D
	{
		class RayAccelerator
		{
		public:
			virtual ~RayAccelerator();
			virtual std::deque<FaceConstItr> getRayCandidates(const QVector3D& rayFrom, const QVector3D& rayTo) = 0;
			virtual std::deque<FaceConstItr> getRayCandidatesDirection(const QVector3D& rayFrom, const QVector3D& rayDirection) = 0;

			virtual QVector3D getWorldPos(const VertexConstItr& vtx)const = 0;
		};


	}
}