#include "TwoManifoldBuilder.h"
#include "Shapes2D.h"
#include "PoissonRemesh.h"
#include "../common/Debug.h"
#include "../qmlmanager.h"
//Few assumptions
//1. Assume filling plane is XY plane.
//2. Assume Model have a boundary edges that is roughly parallel to this plane
using namespace Hix::Engine3D;




std::unordered_set<HalfEdgeConstItr> getBoundaryEdges(const Mesh& mesh)
{

	std::unordered_set<HalfEdgeConstItr> allEdges;
	std::unordered_set<HalfEdgeConstItr> boundaryEdges;

	auto heEnd = mesh.getHalfEdges().cend();
	for (auto heItr = mesh.getHalfEdges().cbegin(); heItr != heEnd; ++heItr)
	{
		allEdges.emplace(heItr);

	}
	while (!allEdges.empty())
	{
		auto curr = *allEdges.begin();
		auto twins = curr.twins();
		if (twins.empty())
		{
			boundaryEdges.insert(curr);
		}
		else
		{
			for (auto& each : twins)
			{
				allEdges.erase(each);
			}
		}
		allEdges.erase(curr);

	}
	return boundaryEdges;
	
	//auto heEnd = mesh.getHalfEdges().cend();

	//for (auto heItr = mesh.getHalfEdges().cbegin(); heItr != heEnd; ++heItr)
	//{

	//}

}

std::deque<std::deque<HalfEdgeConstItr>> formBoundaryPaths(std::unordered_set<HalfEdgeConstItr>& boundaryEdges)
{
	std::deque<std::deque<HalfEdgeConstItr>> boundaries;
	while (!boundaryEdges.empty())
	{
		auto start = *boundaryEdges.begin();
		boundaryEdges.erase(start);
		std::deque<HalfEdgeConstItr> boundary;
		boundary.emplace_back(start);
		auto curr = start;
		bool success = true;
		while (boundary.front().from() != boundary.back().to())
		{
			auto to = curr.to();
			auto nexts = to.leavingBoundary();
			if (nexts.size() == 0)
			{
				qDebug("shit, deadend");
				success = false;
				break;
			}
			else if (nexts.size() > 1)
			{
				//suspect butterfly degeneracy
				auto currentFace = curr.owningFace();
				auto nf = currentFace.neighborFaces();
				qDebug() << "currentFace: " << currentFace.index() << "nf size:" << nf.size();

				success = false;

				for (auto& nextEdge : nexts)
				{
					qDebug()<< "doing butterfly on: " << nextEdge.index() << nextEdge.owningFace().index();

					if (nextEdge.owningFace() == currentFace || !currentFace.isButterfly(nextEdge.owningFace(), to))
					{
						qDebug() << "yay butterfly handled is singleTriangle: " << (nextEdge.owningFace() == currentFace);

						success = true;
						curr = nextEdge;
						break;
					}
				}
				//auto owner = curr.owner();
				//std::unordered_set<FaceConstItr> testF;
				//testF.insert(currentFace);
				////for (auto& nextEdge : nexts)
				////{
				////	testF.insert(nextEdge.owningFace());
				////}
				//for (auto& face : testF)
				//{
				//	Hix::Debug::DebugRenderObject::getInstance().outlineFace(face);
				//}

				//auto glmodel = qmlManager->getModelByID(dynamic_cast<const GLModel*>(owner->entity())->ID);
				//Hix::Debug::DebugRenderObject::getInstance().registerDebugColorFaces(glmodel, testF);
				//Hix::Debug::DebugRenderObject::getInstance().colorDebugFaces();

				//return boundaries;
			}
			else
			{
				//default case
				qDebug() << "default";

				curr = *nexts.begin();
			}
			if (!success || boundaryEdges.find(curr) == boundaryEdges.end())
			{
				qDebug("shit, boundary doesn't exist");
				success = false;
				break;
			}
			boundary.emplace_back(curr);
			boundaryEdges.erase(curr);
		}
		if(success)
			boundaries.emplace_back(std::move(boundary));
	}
	return boundaries;
}
std::deque<HalfEdgeConstItr>  getLongestBoundary(const Hix::Engine3D::Mesh& mesh)
{
	std::deque<HalfEdgeConstItr> longestBoundary;
	std::unordered_set<HalfEdgeConstItr> boundaryEdges;
	boundaryEdges = getBoundaryEdges(mesh);
	std::deque<std::deque<HalfEdgeConstItr>> boundaries = formBoundaryPaths(boundaryEdges);
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
std::deque<HalfEdgeConstItr>  getBoundary(const HalfEdgeConstItr& start)
{
	std::deque<HalfEdgeConstItr> boundary;
	boundary.emplace_back(start);
	auto curr = start;
	while (boundary.front().from() != boundary.back().to())
	{
		auto to = curr.to();
		auto nexts = to.leavingBoundary();
		if (nexts.size() != 1)
			qDebug("shit, boundaries multiple");
		curr = *nexts.begin();
		boundary.emplace_back(curr);
	}
	return boundary;
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

bool checkBoundary(std::deque<HalfEdgeConstItr>& boundary)
{
	std::unordered_set<HalfEdgeConstItr> set;
	for (auto& each : boundary)
	{
		set.insert(each);
	}
	if (set.size() != boundary.size())
	{
		qDebug("boundary has duplicate edges");
		return false;
	}
	auto prevTo = boundary.back().to();
	for (auto itr = boundary.begin(); itr != boundary.end(); ++itr)
	{
		if (prevTo != itr->from())
		{
			qDebug() << prevTo.index() << itr->from().index() << prevTo.localPosition() << itr->from().localPosition();
			return false;
		}
		prevTo = itr->to();
	}
	return true;


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
				auto vtx0(e0->from());
				auto vtx1(e0->to());
				auto vtx2(e1->to());
				auto pos0 = vtx0.localPosition();
				auto pos1 = vtx1.localPosition();
				auto pos2 = vtx2.localPosition();

				//Compare in XZ, YZ planes, skip XY plane check
				//XZ
				bool fill = false;

				//if (vtx0.isConnected(vtx2))
				//{
				//	//constexpr float MIN_Z_NORMAL = -0.7f;

				//	////fn
				//	//QVector3D fn = QVector3D::normal(pos2, pos1, pos0);
				//	//if (fn.z() < MIN_Z_NORMAL)
				//	//{
				//	//	//is concave, but we don't care much about concavity in XY-Plane, especially for downfacing faces
				//	//	constexpr float MAX_Z_MULTI = 2;
				//	//	Bounds3D triBound;
				//	//	triBound.update(pos0);
				//	//	triBound.update(pos1);
				//	//	triBound.update(pos2);
				//	//	auto maxXY = std::max(triBound.lengthX(), triBound.lengthY());
				//	//	if (maxXY < triBound.lengthZ())
				//	//	{
				//	//		fill = true;
				//	//	}
				//	//}
				//	//else
				//	//{
				//	//	fill = true;
				//	//}

				//}
				//else
				//{
				//	fill = true;
				//}

				QVector2D xz0(pos0.x(), pos0.z());
				QVector2D xz1(pos1.x(), pos1.z());
				QVector2D xz2(pos2.x(), pos2.z());
				QVector2D yz0(pos0.y(), pos0.z());
				QVector2D yz1(pos1.y(), pos1.z());
				QVector2D yz2(pos2.y(), pos2.z());
				if (isConcave(xz0, xz1, xz2) || isConcave(yz0, yz1, yz2))
				{
					if (!vtx0.isConnected(vtx2))
					{
						constexpr float MIN_Z_NORMAL = -0.7f;
						//fn
						QVector3D fn = QVector3D::normal(pos2, pos1, pos0);
						if (fn.z() < MIN_Z_NORMAL)
						{
							////is concave, but we don't care much about concavity in XY-Plane, especially for downfacing faces
							//constexpr float MAX_Z_MULTI = 2;
							//Bounds3D triBound;
							//triBound.update(pos0);
							//triBound.update(pos1);
							//triBound.update(pos2);
							//auto maxXY = std::max(triBound.lengthX(), triBound.lengthY());
							//if (maxXY < triBound.lengthZ())
							//{
							//	fill = true;
							//}

						}
						else
						{
							fill = true;
						}
					}
				}
				else
				{

				}

				
				if (fill)
				{
					//ear clip
					mesh.addFace(pos2, pos1, pos0);
					auto latestFace = mesh.getFaces().cend() - 1;
					if (latestFace.isWrongOrientation())
					{
						qDebug() << "wrong orientation face added" << area(xz0, xz1, xz2);
					}
					else
					{
						qDebug() << "		working" << area(xz0, xz1, xz2);

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
					if (e0 == boundary.end())
						break;
					else
						e1 = e0 + 1;
					++filledCnt;

				}
				else
				{
					++e0;
					//++e1;
					e1 = e0 + 1;

				}
			}
			totalFilledCnt += filledCnt;
			//break;
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


void edgeRemoveConcavity(Hix::Engine3D::Mesh& mesh, std::deque<HalfEdgeConstItr>& boundary, bool isBottEmpty)
{
	std::unordered_set<FaceConstItr> toBeDeleted;
	if (boundary.size() < 2)
		return;
	//if bott is empty, CCW,  CW otherwise
	size_t totalRemovedCnt = 0;
	size_t removeCnt;
	if (isBottEmpty)
	{
		do
		{
			removeCnt = 0;
			auto e0 = boundary.begin();
			auto e1 = e0 + 1;			
			while (e1 != boundary.end())
			{
				auto face0 = e0->owningFace();
				auto face1 = e1->owningFace();
				if (face0 == face1)
				{
					++removeCnt;
					auto twins = e1->next().twins();
					if (twins.size() > 1)
						qDebug() << "non manifold scan, more than 1 twin";
					auto eNext = *twins.begin();
					toBeDeleted.insert(face0);
					e0 = boundary.erase(e0, e1 + 1);
					e0 = boundary.insert(e0, eNext);
					e1 = e0 + 1;
				}
				else
				{
					++e0;
					++e1;
				}


			}
			auto isBoundaryCorrect = checkBoundary(boundary);
			totalRemovedCnt += removeCnt;
			//break;
		} while (removeCnt != 0);
	}
	mesh.removeFaces(toBeDeleted);
	std::unordered_set<FaceConstItr> boundaryFaces;
	for (auto& each : boundary)
	{
		boundaryFaces.insert(each.owningFace());
	}
	auto model = qmlManager->getModelByID(dynamic_cast<const GLModel*>(mesh.entity())->ID);
	Hix::Debug::DebugRenderObject::getInstance().registerDebugColorFaces(model, boundaryFaces);


}


//void edgeRemoveDownFacing(Hix::Engine3D::Mesh& mesh, std::deque<HalfEdgeConstItr>& boundary, bool isBottEmpty)
//{
//	std::unordered_set<FaceConstItr> toBeDeleted;
//	std::unordered_set<FaceConstItr> fucking;
//
//	if (boundary.size() < 2)
//		return;
//	//if bott is empty, CCW,  CW otherwise
//	size_t totalRemovedCnt = 0;
//	size_t removeCnt;
//	for (auto& eachEdge : boundary)
//	{
//		qDebug() << "owning face: " << eachEdge.owningFace().index();
//	}
//
//	if (isBottEmpty)
//	{
//		do
//		{
//			removeCnt = 0;
//			auto e0 = boundary.begin();
//			while (e0 != boundary.end())
//			{
//				auto face0 = e0->owningFace();
//				constexpr float MIN_Z_NORMAL = -0.001f;
//
//				if (face0.localFn().z() < MIN_Z_NORMAL)
//				{
//
//					auto e1 = e0 + 1;
//					if (e1 != boundary.end() && e1->owningFace() == face0)
//					{
//						qDebug() << "deleting double edge boudnary" << e0->index();
//						qDebug() << "deleting double edge boudnary" << e1->index();
//						auto nonExposedEdge0 = e1->next();
//						e0 = boundary.erase(e0, e1 + 1);
//						e0 = boundary.insert(e0, *(nonExposedEdge0.twins().begin()));
//						qDebug() << "	inserting" << e0->index();
//
//					}
//					else
//					{
//						qDebug() << "deleting single edge boudnary" << e0->index();
//						auto nonExposedEdge1 = e0->next();
//						auto nonExposedEdge0 = nonExposedEdge1.next();
//						auto twins0 = nonExposedEdge0.twins();
//						auto twins1 = nonExposedEdge1.twins();
//						e0 = boundary.erase(e0);
//
//						if (twins1.size() != 0)
//						{
//
//							e0 = boundary.insert(e0, *(twins1.begin()));
//							qDebug() << "	inserting" << e0->index();
//
//						}
//						if (twins0.size() != 0)
//						{
//							//auto isNotFound = toBeDeleted.find(face0) == toBeDeleted.cend();
//							//auto twins2 = e0->twins();
//							//qDebug() << "attempting to find 0 twins" << nonExposedEdge0.index() << nonExposedEdge1.index() << isNotFound << face0.index();
//							e0 = boundary.insert(e0, *(twins0.begin()));
//							qDebug() << "	inserting" << e0->index();
//						}
//						if (twins1.size() == 0 || twins0.size() == 0)
//						{
//							fucking.insert(face0);
//							//mesh.removeFaces(toBeDeleted);
//							auto model = qmlManager->getModelByID(dynamic_cast<const GLModel*>(mesh.entity())->ID);
//							Hix::Debug::DebugRenderObject::getInstance().registerDebugColorFaces(model, toBeDeleted);
//							return;
//						}
//
//
//					}
//					auto isBoundaryCorrect = checkBoundary(boundary);
//					if (!isBoundaryCorrect)
//					{
//						qDebug() << "Boundary failed" << face0.index();
//
//					}
//					toBeDeleted.insert(face0);
//					qDebug() << "FACE deleted" << face0.index();
//					++removeCnt;
//				}
//				else
//				{
//					qDebug() << "skipping" << e0->index();
//
//					++e0;
//				}
//			}
//			totalRemovedCnt += removeCnt;
//			//break;
//		} while (removeCnt != 0);
//	}
//	mesh.removeFaces(toBeDeleted);
//
//
//}



void edgeRemoveDownFacing(Hix::Engine3D::Mesh& mesh, std::deque<HalfEdgeConstItr>& boundary, bool isBottEmpty)
{



	if (boundary.size() < 2)
		return;

	std::unordered_set<FaceConstItr> toBeDeleted;
	std::unordered_set<FaceConstItr> explored;
	std::unordered_set<HalfEdgeConstItr> exploredEdges;
	//if bott is empty, CCW,  CW otherwise
	size_t totalRemovedCnt = 0;
	size_t removeCnt;


	if (isBottEmpty)
	{
		do
		{
			removeCnt = 0;
			auto e0 = boundary.begin();
			while (e0 != boundary.end())
			{
				auto face0 = e0->owningFace();
				if (toBeDeleted.find(face0) != toBeDeleted.end() || explored.find(face0) != explored.end())
				{
					e0 = boundary.erase(e0);
					continue;
				}
				constexpr float MIN_Z_NORMAL = 0.1f;

				if (face0.localFn().z() < MIN_Z_NORMAL)
				{

					auto e1 = e0 + 1;
					if (e1 != boundary.end() && e1->owningFace() == face0)
					{
						qDebug() << "deleting double edge boudnary" << e0->index();
						qDebug() << "deleting double edge boudnary" << e1->index();
						auto nonExposedEdge0 = e1->next();
						auto twins = nonExposedEdge0.twins();
						e0 = boundary.erase(e0, e1 + 1);
						if (twins.size() != 0)
						{
							e0 = boundary.insert(e0, *(twins.begin()));
						}
						qDebug() << "	inserting" << e0->index();

					}
					else
					{
						qDebug() << "deleting single edge boudnary" << e0->index();
						auto nonExposedEdge1 = e0->next();
						auto nonExposedEdge0 = nonExposedEdge1.next();
						auto twins0 = nonExposedEdge0.twins();
						auto twins1 = nonExposedEdge1.twins();
						e0 = boundary.erase(e0);

						if (twins1.size() != 0)
						{

							e0 = boundary.insert(e0, *(twins1.begin()));
							qDebug() << "	inserting" << e0->index();

						}
						if (twins0.size() != 0)
						{
							//auto isNotFound = toBeDeleted.find(face0) == toBeDeleted.cend();
							//auto twins2 = e0->twins();
							//qDebug() << "attempting to find 0 twins" << nonExposedEdge0.index() << nonExposedEdge1.index() << isNotFound << face0.index();
							e0 = boundary.insert(e0, *(twins0.begin()));
							qDebug() << "	inserting" << e0->index();
						}
						//if (twins1.size() == 0 || twins0.size() == 0)
						//{
						//	fucking.insert(face0);
						//	//mesh.removeFaces(toBeDeleted);
						//	auto model = qmlManager->getModelByID(dynamic_cast<const GLModel*>(mesh.entity())->ID);
						//	Hix::Debug::DebugRenderObject::getInstance().registerDebugColorFaces(model, toBeDeleted);
						//	return;
						//}


					}
					toBeDeleted.insert(face0);
					qDebug() << "FACE deleted" << face0.index();
					++removeCnt;
				}
				else
				{
					qDebug() << "skipping" << e0->index();
					explored.insert(face0);
					exploredEdges.insert(*e0);
					++e0;
				}
			}
			totalRemovedCnt += removeCnt;
			//break;
		} while (removeCnt != 0);
	}
	auto delguard = mesh.removeFacesWithoutShifting(toBeDeleted);
	//use explored faces to form new boundaries, there maybe multiple boundaries due to parts of meshes becoming disconnected
	std::unordered_set<HalfEdgeConstItr> boundaryEdges;
	for (auto& each : explored)
	{
		auto edge = each.edge();
		for (size_t i = 0; i < 3; ++i)
		{
			if (edge.isBoundary())
			{
				boundaryEdges.insert(edge);
			}
			edge.moveNext();
		}
	}
	auto boundaries = formBoundaryPaths(boundaryEdges);
	auto largestItr = boundaries.begin();
	for (auto itr = boundaries.begin(); itr != boundaries.end(); ++itr)
	{
		if (itr->size() > largestItr->size())
			largestItr = itr;
	}
	
	//delete disconnected faces
	std::unordered_set<FaceConstItr> isolatedFaces;
	for (auto itr = boundaries.begin(); itr != boundaries.end(); ++itr)
	{
		if (itr != largestItr)
		{
			auto firstEdge = itr->front();
			auto connected = firstEdge.owningFace().findAllConnected();
			isolatedFaces.merge(std::move(connected));
		}
	}
	auto delguard2 = mesh.removeFacesWithoutShifting(isolatedFaces);
	delguard += std::move( delguard2);

	////auto boundaries = formBoundaryPaths(exploredEdges);
	////auto test = toBeDeleted;
	////for (auto& each : explored)
	////{
	////	test.insert(each);
	////}
	//
	//size_t deletedExplored = 0;
	//auto test = explored;
	//for (auto& each : toBeDeleted)
	//{
	//	if (test.find(each) != test.end())
	//	{
	//		++deletedExplored;
	//		test.erase(each);

	//	}
	//}
	//


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
	auto test = getBoundary(*boundary.begin());
	auto isBoundaryCorrect = checkBoundary(boundary);
	auto isBottEmpty = !isClockwise(boundary);
	//edgeRemoveConcavity(_model, boundary, isBottEmpty);
	edgeRemoveDownFacing(_model, boundary, isBottEmpty);
	////edgeSmoothing(_model, boundary, isBottEmpty);
	//edgeRemoveDownFacing(_model, boundary, isBottEmpty);
	////for(auto& )

	std::unordered_set<FaceConstItr> testF;
	//auto heEnd = _model.getHalfEdges().cend();
	//for (auto heItr = _model.getHalfEdges().cbegin(); heItr != heEnd; ++heItr)
	//{
	//	if (heItr.twins().size() == 2)
	//	{
	//		testF.insert(heItr.owningFace());
	//	}
	//}
	auto viEnd = _model.getVertices().cend();
	for (auto vi = _model.getVertices().cbegin(); vi != viEnd; ++vi)
	{
		auto leavingBoundary = vi.leavingBoundary();
		if (leavingBoundary.size() > 1)
		{
			for (auto& each : leavingBoundary)
			{
				testF.insert(each.owningFace());
			}
		}
	}


	auto glmodel = qmlManager->getModelByID(dynamic_cast<const GLModel*>(_model.entity())->ID);
	Hix::Debug::DebugRenderObject::getInstance().registerDebugColorFaces(glmodel, testF);



}

Hix::Engine3D::Mesh* Hix::Features::TwoManifoldBuilder::execute()
{
	Hix::Features::PoissonRemesh remesher;
	return remesher.remesh(_model);
}
