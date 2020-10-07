#pragma once

#include "SceneEntityWithMaterial.h"
namespace Hix
{
	namespace Render
	{
		class CircleMeshEntity : public SceneEntityWithMaterial
		{
		public:
			CircleMeshEntity(float radius, size_t segCount, const QColor& color, Qt3DCore::QEntity* parent);
			virtual ~CircleMeshEntity();
		protected:
			QVector4D getPrimitiveColorCode(const Hix::Engine3D::Mesh* mesh, Hix::Engine3D::FaceConstItr faceItr)override;
		};


	}
}