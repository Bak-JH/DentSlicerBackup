#pragma once
#include <vector>
#include <string>
#include "../../DentEngine/src/Mesh.h"
#include "../Feature.h"
class GLModel;
namespace Hix
{
	namespace Render
	{
		class SceneEntity;
	}
	namespace Features
	{
		class MeshRepair : public Hix::Features::Feature
		{
		public:
			MeshRepair(const std::unordered_set<GLModel*>& selectedModels);
			virtual ~MeshRepair();
		private:
			void repairImpl(Hix::Render::SceneEntity* subject, const QString& modelName);

		};
		std::vector<Hix::Engine3D::Mesh*> importAndRepairMesh(const std::string& filePath);
		std::vector<Hix::Engine3D::Mesh*> repair(const Hix::Engine3D::Mesh& mesh);

		// find hole edges(edges along the holes) and make contour from it
		Hix::Engine3D::Paths3D identifyHoles(const  Hix::Engine3D::Mesh* mesh);
		//get boundaries ie) half edges on the edge/hole of the mesh
		std::vector<Hix::Engine3D::HalfEdgeConstItr> identifyBoundary(const Hix::Engine3D::Mesh* mesh);
		std::vector<Engine3D::Mesh*> seperateDisconnectedMeshes(Engine3D::Mesh* mesh);
	}
}