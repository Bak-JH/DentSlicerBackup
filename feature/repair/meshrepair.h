#pragma once
#include <vector>
#include <string>
#include "../../DentEngine/src/Mesh.h"
#include "../interfaces/Feature.h"
class GLModel;
namespace Hix
{
	namespace Render
	{
		class SceneEntity;
	}
	namespace Features
	{
		class MeshRepair: public Hix::Features::FeatureContainer
		{
		public:
			MeshRepair(GLModel* selectedModel);
			virtual ~MeshRepair();
			void runImpl()override;
			GLModel* get();
		private:
			GLModel* _model;
			void repairImpl(GLModel* subject, const QString& modelName);

		};
		std::vector<Hix::Engine3D::Mesh*> importAndRepairMesh(const std::string& filePath);
		void repair(Hix::Engine3D::Mesh& mesh);

		// find hole edges(edges along the holes) and make contour from it
		//Hix::Engine3D::Paths3D identifyHoles(const  Hix::Engine3D::Mesh* mesh);
		bool isRepairNeeded(const  Hix::Engine3D::Mesh* mesh);
		//get boundaries ie) half edges on the edge/hole of the mesh
		std::vector<Hix::Engine3D::HalfEdgeConstItr> getBoundaryEdges(const Hix::Engine3D::Mesh* mesh);
		std::vector<Engine3D::Mesh*> seperateDisconnectedMeshes(Hix::Engine3D::Mesh* mesh);
		std::deque<std::unordered_set<Hix::Engine3D::FaceConstItr>>  seperateConnectedFaceSet(const Hix::Engine3D::Mesh& mesh);

	}
}