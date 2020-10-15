#pragma once
#include <Qt3DCore>
#include <Qt3DExtras>
#include <array>
#include <unordered_set>
#include "../feature/Plane3D.h"
class GLModel;


namespace Hix
{
	namespace Render
	{

		class PlaneMeshEntity : public Qt3DCore::QEntity
		{

			Q_OBJECT
		public:
			PlaneMeshEntity(Qt3DCore::QEntity* owner, bool isDoubleSided = false, float alpha = 0.5f);
			PlaneMeshEntity(Qt3DCore::QEntity* owner, float width, float height, const QColor& color, bool isDoubleSided = false, float alpha = 0.5f);

			virtual ~PlaneMeshEntity();
			Qt3DCore::QTransform& transform();
			void setPointNormal(const Hix::Plane3D::PDPlane& plane);
		protected:
			std::unordered_map<Qt3DCore::QEntity*, Qt3DCore::QTransform*> _meshTransformMap;
			Qt3DCore::QTransform _transform;
			Qt3DCore::QEntity* _owner = nullptr;

		};


	}
	
}
