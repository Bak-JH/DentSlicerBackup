#pragma once
#include <unordered_set>
#include <unordered_map>

#include <vector>
#include "DentEngine/src/mesh.h"
namespace Hix
{
	using namespace Engine3D;
	namespace Slicer
	{
		class Planes;
		class Slices;
	}
	namespace OverhangDetect
	{
		typedef std::pair<QVector3D, FaceConstItr> FaceOverhang;
		typedef std::vector<std::variant<VertexConstItr, FaceOverhang>>  Overhangs;
		Overhangs detectOverhang(const Mesh* shellMesh);
	}

}