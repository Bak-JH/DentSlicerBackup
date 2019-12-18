#pragma once

#include "SceneEntity.h"
namespace Hix
{
	namespace Render
	{
		class CircleMeshEntity : public SceneEntity
		{
		public:
			CircleMeshEntity(float radius, size_t segCount, const QColor& color, Qt3DCore::QEntity* parent);
			virtual ~CircleMeshEntity();
		private:
			std::vector<QVector3D> _circle;
			Qt3DExtras::QPhongMaterial _material;
			float _radius;
			size_t _segCount;
		};


	}
}
