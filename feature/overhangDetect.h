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
	}
	namespace OverhangDetect
	{
		struct Overhangs
		{
			//point overhangs and faces that depnds on them
			std::unordered_map<VertexConstItr, std::unordered_set<FaceConstItr>> pointOverhangs;
			std::unordered_multimap<FaceConstItr, QVector3D> faceOverhangs;
		};
		Overhangs detectOverhangs(const Slicer::Planes* planes, const Mesh* shellMesh);
	}

}