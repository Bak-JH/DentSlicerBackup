#pragma once

#include <Qt3DCore>
#include <Qt3DExtras>
#include <array>
#include <unordered_set>
#include "../feature/Plane3D.h"

namespace Hix
{
	namespace Render
	{
		class CircleMeshEntity : public Qt3DCore::QEntity
		{
			Q_OBJECT
		public:
			CircleMeshEntity(Qt3DCore::QEntity* parent, bool isDoubleSided = false);
			CircleMeshEntity(Qt3DCore::QEntity* parent, float radius, const QColor& color, bool isDoubleSided = false);
			
			virtual ~CircleMeshEntity();
			Qt3DCore::QTransform& transform();
			void setPointNormal(const Hix::Plane3D::PDPlane& plane);
		private:
			std::unordered_map<Qt3DCore::QEntity*, Qt3DCore::QTransform*> _meshTransformMap;
			Qt3DCore::QTransform _transform;
			Qt3DCore::QEntity* _owner = nullptr;
		};


	}
}
