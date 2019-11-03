#pragma once
#include <vector>
#include <string>
#include "../../DentEngine/src/Mesh.h"

namespace Hix
{

	namespace Features
	{
		std::vector<Engine3D::Mesh*> importAndRepairMesh(const std::string& filePath);

		// find hole edges(edges along the holes) and make contour from it
		Hix::Engine3D::Paths3D identifyHoles(const  Hix::Engine3D::Mesh* mesh);
		//get boundaries ie) half edges on the edge/hole of the mesh
		std::vector<Hix::Engine3D::HalfEdgeConstItr> identifyBoundary(const Hix::Engine3D::Mesh* mesh);
		std::vector<Engine3D::Mesh*> seperateDisconnectedMeshes(Engine3D::Mesh* mesh);
	}
}