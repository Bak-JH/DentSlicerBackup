#pragma once

#include "../Mesh/mesh.h"
#include "RayAccelerator.h"
namespace Hix
{
	namespace Engine3D
	{
		enum class HitType
		{
			FrontSide,
			BackSide,
			Degenerate,
			Miss
		};

		struct RayHit
		{
			QVector3D intersection;
			HitType type;
			FaceConstItr face;
			float distance;
		};
		typedef std::deque<RayHit> RayHits;
		class RayCaster
		{
		public:
			virtual ~RayCaster();
			void addAccelerator(RayAccelerator* accelerator);
			RayHits rayIntersect(const QVector3D& rayOrigin, const QVector3D& rayEnd);
			RayHits rayIntersectWithoutDegen(const QVector3D& rayOrigin, const QVector3D& rayEnd);
			RayHits rayIntersectDirection(const QVector3D& rayFrom, const QVector3D& rayDirection);
			std::optional<RayHit> getFirstFront(RayHits& hits);
			/// <summary>
			/// volumetric ray cast test with early exit.
			/// </summary>
			bool checkCSecInteresect(const QVector3D& origin, const QVector3D& end, const std::vector<QVector3D>& crossSec);

		protected:
			virtual RayHit rayIntersectTri(const QVector3D& rayOrigin, const QVector3D& rayDirection, const FaceConstItr& tri) = 0;
			virtual RayHit rayIntersectTriWIthoutDegen(const QVector3D& rayOrigin, const QVector3D& rayDirection, const FaceConstItr& tri) = 0;
			RayAccelerator* _accelerator;
		};


	}
}
