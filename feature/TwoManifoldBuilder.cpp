#include "TwoManifoldBuilder.h"
#include "Shapes2D.h"
//Few assumptions
//1. Assume filling plane is XY plane.
//2. Assume Model have a boundary edges that is roughly parallel to this plane
using namespace Hix::Engine3D;


std::deque<HalfEdgeConstItr>  getLongestBoundary(const Hix::Engine3D::Mesh& mesh)
{
	std::deque<HalfEdgeConstItr> longestBoundary;
	std::unordered_set<HalfEdgeConstItr> boundaryEdges;
	auto heEnd = mesh.getHalfEdges().cend();
	for (auto heItr = mesh.getHalfEdges().cbegin(); heItr != heEnd; ++heItr)
	{
		if (heItr.twins().size() != 1)
		{
			boundaryEdges.insert(heItr);
		}
	}
	std::deque<std::deque<HalfEdgeConstItr>> boundaries;
	while (!boundaryEdges.empty())
	{
		std::deque<HalfEdgeConstItr> currBound;
		currBound.emplace_back(*boundaryEdges.begin());
		boundaryEdges.erase(boundaryEdges.begin());
		bool isDeadend;
		auto to = currBound.begin()->to();
		while (currBound.front().from() != currBound.back().to())
		{
			isDeadend = true;
			auto leavingEdges = to.leavingEdges();
			for (auto& each : leavingEdges)
			{
				auto foundBoundary = boundaryEdges.find(each);
				if (foundBoundary != boundaryEdges.end())
				{
					currBound.emplace_back(*foundBoundary);
					to = foundBoundary->to();
					boundaryEdges.erase(foundBoundary);
					isDeadend = false;
					break;
				}
			}
			if (isDeadend)
				break;
		}
		if(!isDeadend)
			boundaries.emplace_back(std::move(currBound));
	}
	std::deque<HalfEdgeConstItr>* longest = nullptr;
	size_t maxLength = 0;
	for (auto& each : boundaries)
	{
		if (each.size() > maxLength)
		{
			maxLength = each.size();
			longest = &each;
		}
	}
	if (longest)
	{
		longestBoundary = std::move(*longest);
	}
	return longestBoundary;
	
}
bool isClockwise(const std::deque<HalfEdgeConstItr>& boundary)
{
	std::vector<QVector3D> path;
	path.reserve(boundary.size());
	for (auto& each : boundary)
	{
		path.emplace_back(each.from().localPosition());
	}
	return Hix::Shapes2D::isClockwise(path);
}

void earClipEdgeSmoothing(Hix::Engine3D::Mesh& mesh)
{

}



Hix::Features::TwoManifoldBuilder::TwoManifoldBuilder(Hix::Engine3D::Mesh& model):_model(model)
{
	//do everything in local coordinates since its transform is null

	//determine the orientation of XY plane, upward or downward

	//auto faceEnd = model.getFaces().cend();
	//for (auto itr = model.getFaces().cbegin(); itr != faceEnd; ++itr)
	//{
	//	_fnCache.insert(std::make_pair(itr, itr.localFn()));
	//}
	auto vtxEnd = model.getVertices().cend();
	float zMin = std::numeric_limits<float>::max();
	float zMax = std::numeric_limits<float>::lowest();
	for (auto itr = model.getVertices().cbegin(); itr != vtxEnd; ++itr)
	{
		auto posZ = itr.localPosition().z();
		if (posZ < zMin)
			zMin = posZ;
		if (posZ > zMax)
			zMax = posZ;
	}

	auto boundary = getLongestBoundary(_model);
	auto isBottEmpty = !isClockwise(boundary);
	//for(auto& )


}
