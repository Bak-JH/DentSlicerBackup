#pragma once


#include "SceneEntity.h"

namespace Hix
{
	namespace Render
	{
		using namespace Qt3DCore;
		using namespace Qt3DRender;
		using namespace Qt3DExtras;
		using namespace Hix::Engine3D;
		class SceneEntityWithMaterial : public SceneEntity
		{
			Q_OBJECT
		public:


			SceneEntityWithMaterial(QEntity* parent = nullptr); // main constructor for mainmesh and shadowmesh
			virtual ~SceneEntityWithMaterial();
			Hix::Render::ModelMaterial& meshMaterial();


		protected:


			//unsigned int getPrimitiveColorCode(const Hix::Engine3D::Mesh* mesh, size_t faceIdx);
			virtual QVector4D getPrimitiveColorCode(const Hix::Engine3D::Mesh* mesh, FaceConstItr faceItr) = 0;
			void appendMeshVertex(const Hix::Engine3D::Mesh* mesh,
				Hix::Engine3D::FaceConstItr begin, Hix::Engine3D::FaceConstItr end)override;


			void appendMeshVertexPerPrimitive(const Hix::Engine3D::Mesh* mesh,
				Hix::Engine3D::FaceConstItr begin, Hix::Engine3D::FaceConstItr end);



			Hix::Render::ModelMaterial _meshMaterial;

		};
	}
}
