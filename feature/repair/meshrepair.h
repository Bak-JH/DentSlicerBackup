#pragma once
#include <vector>
#include <string>
namespace Hix
{
	namespace Engine3D
	{
		class Mesh;
	}
	namespace Features
	{
		std::vector<Engine3D::Mesh*> importAndRepairMesh(const std::string& filePath);
	}
}