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
		struct Overhangs
		{
			//point overhangs and faces that depnds on them
			std::unordered_set<VertexConstItr> pointOverhangs;
			std::unordered_multimap<FaceConstItr, QVector3D> faceOverhangs;
		};
		std::unordered_set<VertexConstItr> detectOverhangsPostSlice(const Slicer::Planes* planes, const Mesh* shellMesh, const Slicer::Slices* slices);
	}

}