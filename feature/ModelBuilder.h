#pragma once
#include <string>
#include <filesystem>
namespace Hix
{
	namespace Engine3D
	{
		class Mesh;
	}
	class ModelBuilder
	{
	public:
		void buildModel(const std::string& filePath, Engine3D::Mesh& dest);
		void buildModel(const std::filesystem::path& filePath, Engine3D::Mesh& dest);

	};

}

