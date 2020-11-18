#pragma once


#include "SceneEntity.h"

namespace Hix
{
	namespace Render
	{
		using namespace Qt3DRender;
		using namespace Qt3DExtras;
		using namespace Hix::Engine3D;
		class SceneEntityWithMaterial : public SceneEntity
		{
			Q_OBJECT
		public:
			// face selection enabled
			std::unordered_set<FaceConstItr> selectedFaces;

			SceneEntityWithMaterial(Qt3DCore::QEntity* parent = nullptr); // main constructor for mainmesh and shadowmesh
			virtual ~SceneEntityWithMaterial();
			void setMaterialMode(const Hix::Render::ShaderMode mode);
			void setMaterialColor(const QVector4D color);
			void setMaterialParamter(const std::string& key, const QVariant& value);
		protected:

			//unsigned int getPrimitiveColorCode(const Hix::Engine3D::Mesh* mesh, size_t faceIdx);
			virtual QVector4D getPrimitiveColorCode(const Hix::Engine3D::Mesh* mesh, FaceConstItr faceItr) = 0;
			void appendMeshVertex(const Hix::Engine3D::Mesh* mesh,
				Hix::Engine3D::FaceConstItr begin, Hix::Engine3D::FaceConstItr end)override;


			void appendMeshVertexPerPrimitive(const Hix::Engine3D::Mesh* mesh,
				Hix::Engine3D::FaceConstItr begin, Hix::Engine3D::FaceConstItr end);

		private:
			Hix::Render::ModelMaterial _meshMaterial;

		};
	}
}
