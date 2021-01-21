#pragma once
#include "RayCaster.h"


namespace Hix
{
	namespace Engine3D
	{
		class MTRayCaster: public RayCaster
		{
		public: 
			MTRayCaster();
			virtual ~MTRayCaster();
		protected:
			RayHit rayIntersectTri(const QVector3D& rayOrigin, const QVector3D& rayDirection, const FaceConstItr& tri)override;
		};


	}
}


