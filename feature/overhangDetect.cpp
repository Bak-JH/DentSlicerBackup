#include "overhangDetect.h"
#include "DentEngine/src/polyclipping/polyclipping.h"
#include "DentEngine/src/slicer.h"
using namespace Hix;
using namespace Hix::Engine3D;
using namespace Hix::Slicer;
using namespace Hix::OverhangDetect;


namespace OverhangDetectPrivate
{
	std::unordered_set<VertexConstItr> localMinFacingDown(const Mesh* mesh);
	//removes multiple min vertices that makes contact to same filled segments.
	void removeOverlappingMins(std::unordered_set<VertexConstItr>& mins, const Slicer::Planes* planes, const Slicer::Slices* slices);
}
using namespace OverhangDetectPrivate;
std::unordered_set<VertexConstItr> OverhangDetectPrivate::localMinFacingDown(const Mesh* mesh){

	std::unordered_set<VertexConstItr> minis;
	const auto& vertices = mesh->getVertices();
	for (auto vtx = vertices.cbegin(); vtx != vertices.cend(); ++vtx)
	{
		if (vtx->vn.z() < 0.0f)
		{
			bool localMin = true;
			auto connected = vtx->connectedVertices();
			for (auto& each : connected)
			{
				if (each->position.z() < vtx->position.z())
				{
					localMin = false;
					break;
				}
			}
			if (localMin)
				minis.insert(vtx);
		}
	}
	return minis;
}

void OverhangDetectPrivate::removeOverlappingMins(std::unordered_set<VertexConstItr>& mins, 
	const Slicer::Planes* planes, const Slicer::Slices* slices)
{
	//map of filled contour and local minimas in that contour
	std::unordered_map<ClipperLib::PolyNode*, VertexConstItr> contourMiniPairs;
	for (auto& each : mins)
	{
		bool isHit = false;
		ClipperLib::PolyNode* owningNode = nullptr;
		for (auto & slice: *slices)
		{
			isHit = ClipperLib::ptInPolytree(QVector2D(each->position.x(), each->position.y()), slice.polytree, owningNode);
			if (isHit)
			{
				contourMiniPairs[owningNode] = each;
				break;
			}
		}
	}
	mins.clear();
	mins.reserve(contourMiniPairs.size());
	for (auto& each : contourMiniPairs)
	{
		mins.insert(each.second);
	}
}

std::unordered_set<VertexConstItr> Hix::OverhangDetect::detectOverhangsPostSlice(const Slicer::Planes* planes, const Mesh* shellMesh, const Slicer::Slices* slices)
{
	Overhangs overhangs;
	//get all local minimas
	auto pointOverhangs = localMinFacingDown(shellMesh);
	//ray cast in perpendicular z-axis direction, get the lowest filled contour hit, make sure only one support for each overhang region
	removeOverlappingMins(pointOverhangs, planes, slices);

	return pointOverhangs;

}
