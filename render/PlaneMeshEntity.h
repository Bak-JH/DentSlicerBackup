#pragma once
#include <Qt3DCore>
#include <Qt3DExtras>
#include <array>
#include <unordered_set>
class GLModel;


namespace Hix
{
	namespace Render
	{
		struct PDPlane
		{
			QVector3D point;
			QVector3D normal;
		};
		class PlaneMeshEntity : public Qt3DCore::QEntity
		{

			Q_OBJECT
		public:
			PlaneMeshEntity(Qt3DCore::QEntity* owner);
			virtual ~PlaneMeshEntity();
			Qt3DCore::QTransform& transform();
			void setPointNormal(const PDPlane& plane);
		protected:
			std::unordered_map<Qt3DCore::QEntity*, Qt3DCore::QTransform*> _meshTransformMap;
			std::array<Qt3DExtras::QPlaneMesh, 2> _planeMeshes;
			Qt3DCore::QTransform _transform;
			Qt3DCore::QEntity* _owner = nullptr;

		};


	}
	
}
