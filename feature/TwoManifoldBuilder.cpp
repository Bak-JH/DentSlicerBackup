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

/* area:  determines area of triangle formed by three points
 */
double area(const QVector2D& v0, const QVector2D& v1, const QVector2D& v2)
{
	double areaSum = 0;

	areaSum += v0.x() * (v2.y() - v1.y());
	areaSum += v1.x() * (v0.y() - v2.y());
	areaSum += v2.x() * (v1.y() - v0.y());
	return areaSum;
}


bool isConcave(const QVector2D& v0, const QVector2D& v1, const QVector2D& v2)
{
	if (area(v0, v1, v2) > 0)
		return true;
	else
		return false;
}



void edgeSmoothing(Hix::Engine3D::Mesh& mesh, std::deque<HalfEdgeConstItr>& boundary, bool isBottEmpty)
{
	if (boundary.size() < 2)
		return;
	//if bott is empty, CCW,  CW otherwise
	size_t totalFilledCnt = 0;
	size_t filledCnt;
	if (isBottEmpty)
	{
		do
		{
			filledCnt = 0;
			auto e0 = boundary.begin();
			auto e1 = e0 + 1;
			//cause we need to compare 3 at a time
			while (e1 != boundary.end())
			{
				if (e0->to() != e1->from())
				{
					qDebug() << "shit";
				}
				auto vtx0(e0->from().localPosition());
				auto vtx1(e0->to().localPosition());
				auto vtx2(e1->to().localPosition());
				//Compare in XZ, YZ planes, skip XY plane check
				//XZ
				bool concave = false;
				//QVector2D xz0(vtx0.x(), vtx0.z());
				//QVector2D xz1(vtx1.x(), vtx1.z());
				//QVector2D xz2(vtx2.x(), vtx2.z());
				//if (isConcave(xz0, xz1, xz2))
				//{
				//	concave = true;
				//}
				//else
				//{
				//	QVector2D yz0(vtx0.y(), vtx0.z());
				//	QVector2D yz1(vtx1.y(), vtx1.z());
				//	QVector2D yz2(vtx2.y(), vtx2.z());
				//	if (isConcave(yz0, yz1, yz2))
				//	{
				//		concave = true;
				//	}
				//}
				if (concave)
				{
					//ear clip
					mesh.addFace(vtx2, vtx1, vtx0);
					auto latestFace = mesh.getFaces().cend() - 1;
					if (latestFace.isWrongOrientation())
					{
						qDebug() << "shit3";
					}
					HalfEdgeConstItr itr;
					bool isworking = latestFace.getEdgeWithVertices(itr, e0->from(), e1->to());
					if (!isworking)
					{
						qDebug() << "shit2";
					}
					//update boundary
					e0 = boundary.erase(e0, e1 + 1);
					e0 = boundary.insert(e0, itr);
					++e0;
					if (e0 != boundary.end())
						e1 = e0 + 1;
					else
						e1 = boundary.end();
					++filledCnt;

				}
				else
				{
					++e0;
					++e1;
				}
			}
			totalFilledCnt += filledCnt;
			break;
		} while (filledCnt != 0);
		
	}
	std::unordered_set<HalfEdgeConstItr> wrongSideE;
	std::unordered_set<FaceConstItr> wrongSideF;

	auto eEnd = mesh.getHalfEdges().cend();
	for (auto itr = mesh.getHalfEdges().cbegin(); itr != eEnd; ++itr)
	{
		if (!itr.nonTwins().empty())
		{
			wrongSideE.insert(itr);
			wrongSideF.insert(itr.owningFace());
		}
	}



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

	auto vtxEnd = _model.getVertices().cend();
	float zMin = std::numeric_limits<float>::max();
	float zMax = std::numeric_limits<float>::lowest();
	for (auto itr = _model.getVertices().cbegin(); itr != vtxEnd; ++itr)
	{
		auto posZ = itr.localPosition().z();
		if (posZ < zMin)
			zMin = posZ;
		if (posZ > zMax)
			zMax = posZ;
	}

	auto boundary = getLongestBoundary(_model);
	auto isBottEmpty = !isClockwise(boundary);
	edgeSmoothing(_model, boundary, isBottEmpty);
	//for(auto& )


}
