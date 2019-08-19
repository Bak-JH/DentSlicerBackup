
#include "../catch.hpp"
#include <string>
#include "../../DentEngine/src/mesh.h"
#include "../../fileloader.h"

//#include "../../DentEngine/src/slicer.h"

//acces unaccessible cpp only functions
#include "../../DentEngine/src/slicer.h"
#include "../RandomGen.h"
namespace SlicerTest
{
	using namespace Hix;
	using namespace Hix::Engine3D;
	using namespace Slicer;
	RandomGen rand;
	bool addTrigBetweenZ(Mesh* mesh, float min_z, float max_z)
	{
		bool result = false;
		float z0 = rand.getFloat(min_z, max_z);
		float z1 = rand.getFloat(min_z, max_z);
		float z2 = rand.getFloat(min_z, max_z);
		result = mesh->addFace(
			QVector3D(rand.getFloat(), rand.getFloat(), z0),
			QVector3D(rand.getFloat(), rand.getFloat(), z1),
			QVector3D(rand.getFloat(), rand.getFloat(), z2));
		return result;
	}

	TEST_CASE("PlaneGeneration", "[Slicer]") {
		SECTION("Plane")
		{
			{
				auto planes = buildUniformPlanes(0.0f, 20.0f, 1.0f);
				REQUIRE(planes.size() == 20);
			}
			{
				auto planes = buildUniformPlanes(0.1f, 20.1f, 1.0f);
				REQUIRE(planes.size() == 20);
			}
			{
				auto planes = buildUniformPlanes(0.00000f, 20000.0f, 1000.0f);
				REQUIRE(planes.size() == 20);
			}
			{
				auto planes = buildUniformPlanes(0.0f, 0.20f, 0.01f);
				REQUIRE(planes.size() == 20);
			}
		}
	}

	TEST_CASE("buildTriangleLists", "[Slicer]") {
		float delta = 0.01f;
		float meshMin = 0.0f;
		float meshMax = 0.20f;
		auto planes = buildUniformPlanes(meshMin, meshMax, 0.01f);
		std::vector<size_t> planeFaceCnt(planes.size(), 0);
		const size_t MAX_TRIG_CNT = 500000;
		SECTION("buildTriangleLists random cases") {
			for (size_t r = 0; r < 100; ++r)
			{
				//add trigs and build model answer
				Mesh* testMesh = new Mesh();
				for (size_t i = 0; i < MAX_TRIG_CNT; ++i)
				{
					bool result = addTrigBetweenZ(testMesh, meshMin, meshMax);
				}
				const auto& faces = testMesh->getFaces();
				std::vector<std::vector<NodeState>> model;
				for (int l_idx = 0; l_idx < planes.size(); l_idx++) {
					model.push_back(std::vector<Slicer::NodeState>(faces.size(), Slicer::NodeState::NotUsed));
				}
				for (auto faceItr = faces.cbegin(); faceItr != faces.cend(); ++faceItr)
				{
					auto faceIdx = testMesh->indexOf(faceItr);
					auto zOrder = faceItr->sortZ();
					auto faceZMin = zOrder[0];
					auto faceZMed = zOrder[1];
					auto faceZMax = zOrder[2];
					if (faceZMin == faceZMax)
						continue;
					for (size_t i = 0; i < planes.size(); ++i)
					{
						auto currPlane = planes[i];
						if ((faceZMin < currPlane && faceZMax > currPlane)
							|| (faceZMin == currPlane && faceZMed == currPlane)
							|| (faceZMax == currPlane && faceZMed == currPlane)
							)
						{
							model[i][faceIdx] = NodeState::Unexplored;
						}
					}

				}
				std::vector<std::vector<NodeState>> planeFaces = buildTriangleLists(testMesh, planes, delta, planeFaceCnt);
				qDebug() << "plane cnt[5]" << planeFaceCnt[5];
				for (size_t i = 0; i < model.size(); ++i)
				{
					for (size_t j = 0; j < model[0].size(); ++j)
					{
						auto currentPlane = planes[i];
						auto faceItr = faces.cbegin() + j;
						auto zOrder = faceItr->sortZ();
						auto faceZMin = zOrder[0];
						auto faceZMed = zOrder[1];
						auto faceZMax = zOrder[2];
						REQUIRE(model[i][j] == planeFaces[i][j]);
					}
				}
				delete testMesh;
			}

		}

		SECTION("against a real model")
		{


			//Mesh* testMesh = nullptr;
			//std::string largeTeeth("33-43.stl");
			//FileLoader::loadMeshSTL(testMesh, largeTeeth.c_str());
			//
			//
			SECTION("against a real model")
			{
			}

			//float delta = scfg->layer_height;
			//std::vector<std::vector<Contour>> contoursPerPlane;
			//std::vector<float> planes;

			//if (scfg->slicing_mode == SlicingConfiguration::SlicingMode::Uniform) {
			//	planes = buildUniformPlanes(mesh->z_min(), mesh->z_max(), delta);
			//}
			////else if (scfg->slicing_mode == "adaptive") {
			// //       // adaptive slice
			// //       planes = buildAdaptivePlanes(mesh->z_min(), mesh->z_max());
			// //   }

			//// build triangle list per layer height
			//std::vector<size_t> planeFaceCnt(planes.size(), 0);
			//std::vector<std::vector<NodeState>> planeFaces = buildTriangleLists(mesh, planes, delta, planeFaceCnt);

			//for (int i = 0; i < planes.size(); i++) {
			//	ContourBuilder contourBuilder(mesh, planeFaces[i], planes[i], planeFaceCnt[i]);
			//	std::vector<Contour> contours = contourBuilder.buildContours();
			//	contoursPerPlane.push_back(contours);
			//}
			//
			//auto planes = Slicer::Private::buildUniformPlanes();

		}

	}
}