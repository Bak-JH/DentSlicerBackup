#pragma once

#include "../DentEngine/src/mesh.h"
#include "RayAccelerator.h"
namespace Hix
{
	namespace Engine3D
	{
		enum class HitType
		{
			FrontSide,
			BackSide,
			Miss
		};

		struct RayHit
		{
			QVector3D intersection;
			HitType type;
			FaceConstItr face;
		};
		typedef std::deque<RayHit> RayHits;
		class RayCaster
		{
		public:
			virtual ~RayCaster();
			void addAccelerator(RayAccelerator* accelerator);
			RayHits rayIntersect(const QVector3D& rayOrigin, const QVector3D& rayEnd);
			RayHits rayIntersectDirection(const QVector3D& rayFrom, const QVector3D& rayDirection);

		protected:
			virtual RayHit rayIntersectTri(const QVector3D& rayOrigin, const QVector3D& rayDirection, const FaceConstItr& tri) = 0;
			std::unique_ptr<RayAccelerator> _accelerator;
		};


	}
}