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


		protected:

			//update faces given indicies, if index >= indexUppderLimit, it's ignored
			void updateFaces(const std::unordered_set<size_t>& faceIndicies, const Hix::Engine3D::Mesh& mesh);
			void updateVertices(const std::unordered_set<size_t>& vtxIndicies, const Hix::Engine3D::Mesh& mesh);

			//unsigned int getPrimitiveColorCode(const Hix::Engine3D::Mesh* mesh, size_t faceIdx);
			virtual QVector3D getPrimitiveColorCode(const Hix::Engine3D::Mesh* mesh, FaceConstItr faceItr) = 0;

			void setMesh(Hix::Engine3D::Mesh* mesh);
			void updateMesh(Hix::Engine3D::Mesh* mesh);
			void appendMeshVertex(const Hix::Engine3D::Mesh* mesh,
				Hix::Engine3D::FaceConstItr begin, Hix::Engine3D::FaceConstItr end);

			void appendMeshVertexSingleColor(const Hix::Engine3D::Mesh* mesh,
				Hix::Engine3D::FaceConstItr begin, Hix::Engine3D::FaceConstItr end);
			void appendMeshVertexPerPrimitive(const Hix::Engine3D::Mesh* mesh,
				Hix::Engine3D::FaceConstItr begin, Hix::Engine3D::FaceConstItr end);

			void appendIndexArray(const Hix::Engine3D::Mesh* mesh,
				Hix::Engine3D::FaceConstItr begin, Hix::Engine3D::FaceConstItr end);


			Hix::Render::ModelMaterial _meshMaterial;

		};
	}
}
