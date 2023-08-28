#pragma once
#include <Qt3DCore>
#include <Qt3DExtras>
#include <array>
#include <unordered_set>
#include "../../render/SceneEntityWithMaterial.h"
class GLModel;
namespace Hix
{
	namespace Features
	{
		class CrossSectionPlane : public Hix::Render::SceneEntityWithMaterial
		{
		public:
			CrossSectionPlane(Qt3DCore::QEntity* owner);
			void init(const std::unordered_map<Hix::Render::SceneEntityWithMaterial*, QVector4D>& modelColorMap);
			virtual ~CrossSectionPlane();
			void showLayer(double zHeight, bool isTop);
		protected:
			QVector4D getPrimitiveColorCode(const Hix::Engine3D::Mesh* mesh, Hix::Engine3D::FaceConstItr faceItr)override;
		private:
			std::vector<Hix::Engine3D::Mesh> _layerMeshes;
		};


	}
}
