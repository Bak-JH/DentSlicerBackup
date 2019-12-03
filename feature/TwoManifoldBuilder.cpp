#include "TwoManifoldBuilder.h"
#include "Shapes2D.h"
#include "PoissonRemesh.h"
#include "../common/Debug.h"
#include "../qmlmanager.h"
#include <cmath>
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
}


QVector3D faceNormalAvg(const Mesh& mesh)
{
	QVector3D avg;
	auto fCend = mesh.getFaces().cend();
	for (auto itr = mesh.getFaces().cbegin(); itr != fCend; ++itr)
	{
		avg += itr.localFn();
	}
	avg.normalize();
	return avg;
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




void cuntourConcaveFill(Hix::Engine3D::Mesh& mesh, std::deque<HalfEdgeConstItr>& boundary, bool isBottEmpty)
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
				auto vtx0(e0->from());
				auto vtx1(e0->to());
				auto vtx2(e1->to());
				auto pos0 = vtx0.localPosition();
				auto pos1 = vtx1.localPosition();
				auto pos2 = vtx2.localPosition();
				std::vector<QVector2D> tri
				{
					QVector2D(pos0),
					QVector2D(pos1),
					QVector2D(pos2)
				};
				bool faceAdded = false;
				bool isUpFacing = Hix::Shapes2D::isClockwise(tri);
				if (isUpFacing)
				{
					auto e0Dir = e0->to().localPosition() - e0->from().localPosition();
					auto e1Dir = e1->from().localPosition() - e1->to().localPosition();

					auto dotProduct = QVector3D::dotProduct(e0Dir, e1Dir);
					auto angle = acos(dotProduct / (e0Dir.length() * e1Dir.length()));
					auto degreeAngle = angle * (180 / M_PI);

					if (degreeAngle < 120)
					{
						mesh.addFace(pos2, pos1, pos0);
						auto latestFace = mesh.getFaces().cend() - 1;

						HalfEdgeConstItr itr;
						bool isworking = latestFace.getEdgeWithVertices(itr, e0->from(), e1->to());
						//update boundary
						e0 = boundary.erase(e0, e1 + 1);
						e0 = boundary.insert(e0, itr);
						e1 = e0 + 1;
						faceAdded = true;
						++removeCnt;
					}

				}
				if (!faceAdded)
				{
					++e0;
					++e1;
				}
			}
			auto isBoundaryCorrect = checkBoundary(boundary);
			totalRemovedCnt += removeCnt;
		} while (removeCnt != 0);
	}
}

bool isDownard(const FaceConstItr& face)
{
	constexpr float MIN_Z_NORMAL = 0.1f;
	if (face.localFn().z() < MIN_Z_NORMAL)
	{
		return true;
	}
	return false;
}


bool isUpward(const FaceConstItr& face)
{
	constexpr float MAX_Z_NORMAL = -0.1f;
	if (face.localFn().z() > MAX_Z_NORMAL)
	{
		return true;
	}
	return false;
}


Hix::Render::PDPlane bestFittingPlane(const std::vector<QVector3D>& points)
{
	Hix::Render::PDPlane plane;
	if (points.size() < 3 ){
		return plane; // At least three points required
	}

	QVector3D sum;
	for (auto& p : points)
	{
		sum += p;
	}

	auto centroid = sum * (1.0 / points.size());
	float xx = 0, xy = 0, xz = 0, yy = 0, yz = 0, zz = 0;
	// Calc full 3x3 covariance matrix, excluding symmetries:
	for (auto& p : points)
	{
		auto r = p - centroid;
		xx += r.x() * r.x();
		xy += r.x() * r.y();
		xz += r.x() * r.z();
		yy += r.y() * r.y();
		yz += r.y() * r.z();
		zz += r.z() * r.z();
	}
	auto det_x = yy * zz - yz * yz;
	auto det_y = xx * zz - xz * xz;
	auto det_z = xx * yy - xy * xy;

	auto det_max = std::max({ det_x, det_y, det_z });
	if (det_max <= 0.0) {
		return plane; // The points don't span a plane
	}

	// Pick path with best conditioning:
	QVector3D dir;
	if( det_max == det_x){
		dir = QVector3D(
			det_x,
			xz * yz - xy * zz,
			xy * yz - xz * yy
			);
	}
	else if (det_max == det_y){
		dir = QVector3D(
			xz * yz - xy * zz,
			det_y,
			xy * xz - yz * xx
			);
	}
	else {
		dir = QVector3D(
			xy * yz - xz * yy,
			xy * xz - yz * xx,
			det_z
			);
	};
	plane.normal = dir.normalized();
	plane.point = centroid;
	return plane;
}

void reorientatePlane(Hix::Render::PDPlane& plane, bool isBottEmpty)
{
	if (isBottEmpty)
	{
		if (plane.normal.z() < 0)
		{
			plane.normal *= -1.0f;
		}
	}
	else
	{
		if (plane.normal.z() > 0)
		{
			plane.normal *= -1.0f;
		}

	}
}
Hix::Render::PDPlane bestFittingPlane(const std::deque<HalfEdgeConstItr>& edges)
{
	std::vector<QVector3D> points;
	points.reserve(edges.size());
	std::transform(edges.cbegin(), edges.cend(), std::back_inserter(points),
		[](const HalfEdgeConstItr& e)->QVector3D {
			return e.to().localPosition();
		});
	return bestFittingPlane(points);
}

void edgeRemoveWrongZ(Hix::Engine3D::Mesh& mesh, std::deque<HalfEdgeConstItr>& boundary, bool isBottEmpty)
{
	if (boundary.size() < 2)
		return;
	auto originalBoundary = boundary;
	std::unordered_set<FaceConstItr> toBeDeleted;
	std::unordered_set<FaceConstItr> explored;
	std::unordered_set<HalfEdgeConstItr> exploredEdges;
	//if bott is empty, CCW,  CW otherwise
	size_t totalRemovedCnt = 0;
	size_t removeCnt;
	std::function<bool(const FaceConstItr&)> isWrongSide = isUpward;
	if (isBottEmpty)
	{
		isWrongSide = isDownard;
	}
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
			if (isWrongSide(face0))
			{
				auto e1 = e0 + 1;
				if (e1 != boundary.end() && e1->owningFace() == face0)
				{
					auto nonExposedEdge0 = e1->next();
					auto twins = nonExposedEdge0.twins();
					e0 = boundary.erase(e0, e1 + 1);
					if (twins.size() != 0)
					{
						e0 = boundary.insert(e0, *(twins.begin()));
					}
				}
				else
				{
					auto nonExposedEdge1 = e0->next();
					auto nonExposedEdge0 = nonExposedEdge1.next();
					auto twins0 = nonExposedEdge0.twins();
					auto twins1 = nonExposedEdge1.twins();
					e0 = boundary.erase(e0);
					if (twins1.size() != 0)
					{
						e0 = boundary.insert(e0, *(twins1.begin()));
					}
					if (twins0.size() != 0)
					{
						e0 = boundary.insert(e0, *(twins0.begin()));
					}
				}
				toBeDeleted.insert(face0);
				++removeCnt;
			}
			else
			{
				explored.insert(face0);
				exploredEdges.insert(*e0);
				++e0;
			}
		}
		totalRemovedCnt += removeCnt;
		//break;
	} while (removeCnt != 0);
	if (toBeDeleted.size() > mesh.getFaces().size() * 0.3)
	{
		//removing wrong-z facing faces might be too destructive
		return;
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
	//reform boundary after shifting faces
	//we use a boundary vertex as hint as vertices shouldn't be shifted(ones with faces connected)
	auto hintVtxPos = largestItr->begin()->to().localPosition();
	qDebug() << "new boundary hint at: " << hintVtxPos;
	delguard.flush();
	auto hintVtx = mesh.getVtxAtLocalPos(hintVtxPos);
	auto startingEdge = *hintVtx.leavingBoundary().begin();
	boundary.clear();
	boundary = getBoundary(startingEdge);




}

inline bool isBelowPlane(const Hix::Render::PDPlane& plane, const FaceConstItr& itr, std::unordered_map<VertexConstItr, bool>& cache)
{
	auto mvs = itr.meshVertices();
	for (auto& v : mvs)
	{
		auto cached = cache.find(v);
		if (cached != cache.end())
		{
			if (cached->second)
				return true;
		}
		else
		{
			bool isBelow = false;
			auto dist = v.localPosition().distanceToPlane(plane.point, plane.normal);
			if (dist < 0)
				isBelow = true;
			cache.insert(std::make_pair(v, isBelow));
			if (isBelow)
				return true;
		}
	}
	return false;
}

MeshDeleteGuard edgeRemoveOutlier(Hix::Engine3D::Mesh& mesh, std::deque<HalfEdgeConstItr>& boundary, const Hix::Render::PDPlane& plane)
{
	if (boundary.size() < 2)
		return MeshDeleteGuard(&mesh);
	auto originalBoundary = boundary;
	std::unordered_set<FaceConstItr> toBeDeleted;
	std::unordered_set<FaceConstItr> explored;
	std::unordered_set<HalfEdgeConstItr> exploredEdges;
	//if bott is empty, CCW,  CW otherwise
	size_t totalRemovedCnt = 0;
	size_t removeCnt;
	std::unordered_map<VertexConstItr, bool> vtxResultCache;
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
			if (isBelowPlane(plane, face0, vtxResultCache))
			{

				auto e1 = e0 + 1;
				if (e1 != boundary.end() && e1->owningFace() == face0)
				{
					auto nonExposedEdge0 = e1->next();
					auto twins = nonExposedEdge0.twins();
					e0 = boundary.erase(e0, e1 + 1);
					if (twins.size() != 0)
					{
						e0 = boundary.insert(e0, *(twins.begin()));
					}
				}
				else
				{
					auto nonExposedEdge1 = e0->next();
					auto nonExposedEdge0 = nonExposedEdge1.next();
					auto twins0 = nonExposedEdge0.twins();
					auto twins1 = nonExposedEdge1.twins();
					e0 = boundary.erase(e0);

					if (twins1.size() != 0)
					{
						e0 = boundary.insert(e0, *(twins1.begin()));
					}
					if (twins0.size() != 0)
					{
						e0 = boundary.insert(e0, *(twins0.begin()));
					}
				}
				toBeDeleted.insert(face0);
				++removeCnt;
			}
			else
			{
				explored.insert(face0);
				exploredEdges.insert(*e0);
				++e0;
			}
		}
		totalRemovedCnt += removeCnt;
		//break;
	} while (removeCnt != 0);
	//auto delguard = mesh.removeFacesWithoutShifting(toBeDeleted);
	//boundary = getBoundary(*exploredEdges.begin());
	//return delguard;
	//auto model = qmlManager->getModelByID(dynamic_cast<const GLModel*>(mesh.entity())->ID);
	//Hix::Debug::DebugRenderObject::getInstance().registerDebugColorFaces(model, toBeDeleted);

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
	delguard += std::move(delguard2);
	//reform boundary after shifting faces
	//we use a boundary vertex as hint as vertices shouldn't be shifted(ones with faces connected)
	auto hintVtxPos = largestItr->begin()->to().localPosition();
	auto hintVtx = mesh.getVtxAtLocalPos(hintVtxPos);
	auto startingEdge = *hintVtx.leavingBoundary().begin();
	boundary = getBoundary(startingEdge);
	return delguard;

}

void zCylinder(Hix::Engine3D::Mesh& model, std::deque<HalfEdgeConstItr>& boundary, bool isBottEmpty, float zVal)
{
	std::vector<QVector3D> bottomPath;
	bottomPath.reserve(boundary.size() + 1);
	for (auto& each : boundary)
	{
		auto pos = each.from().localPosition();
		bottomPath.emplace_back(QVector3D(pos.x(), pos.y(), zVal));
	}
	auto last = boundary.front().from().localPosition();
	bottomPath.emplace_back(QVector3D(last.x(), last.y(), zVal));

	//create actual cylinder
	auto dItr = bottomPath.cbegin();
	for (auto bItr = boundary.cbegin(); bItr != boundary.cend(); ++bItr, ++dItr)
	{
		auto& b0 = *dItr;
		auto& b1 = *(dItr + 1);
		auto t0 = bItr->from().localPosition();
		auto t1 = bItr->to().localPosition();
		//topTriFace
		model.addFace(t1, t0, b1);
		//bot face
		model.addFace(t0, b0, b1);

	}
	

}

void buildBott(Hix::Engine3D::Mesh& mesh, const  std::deque<HalfEdgeConstItr>& boundary, bool isBottEmpty, float zVal)
{
	std::vector<std::vector<QVector3D>> shapes;
	auto& latest = shapes.emplace_back();
	for (auto& each : boundary)
	{
		auto pos = each.from().localPosition();
		latest.emplace_back(QVector3D(pos.x(), pos.y(), 0));

	}
	auto paths =  Hix::Shapes2D::combineContour(shapes);
	auto& outer = *paths.begin();
	std::vector<QVector2D> contour;

	for (auto& each : outer)
	{
		auto flPt = Hix::Polyclipping::toFloatPt(each);
		contour.emplace_back(QVector2D(flPt));
	}

	Hix::Shapes2D::generateCapZPlane(&mesh, contour, zVal, isBottEmpty);

}






Hix::Features::TwoManifoldBuilder::TwoManifoldBuilder(Hix::Engine3D::Mesh& model):_model(model)
{

	//auto avg = faceNormalAvg(_model);
	//Hix::Render::PDPlane plane;
	//plane.normal = avg;
	//plane.point = QVector3D(0, 0, 0);
	//Hix::Debug::DebugRenderObject::getInstance().displayPlane(plane);
	//return;
	auto boundary = getLongestBoundary(_model);
	auto isBottEmpty = !isClockwise(boundary);
	bool boundaryCorrect = false;
	Hix::Engine3D::MeshDeleteGuard deleteGuard(&_model);
	for (size_t i = 0; i < 2; ++i)
	{
		auto bestFitPlane = bestFittingPlane(boundary);
		reorientatePlane(bestFitPlane, isBottEmpty);
		Hix::Debug::DebugRenderObject::getInstance().displayPlane(bestFitPlane);
		auto delFaces = edgeRemoveOutlier(_model, boundary, bestFitPlane);
		deleteGuard += std::move(delFaces);
	}
	//edgeRemoveWrongZ(_model, boundary, isBottEmpty);
	//cuntourConcaveFill(_model, boundary, isBottEmpty);
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
	constexpr float Z_OFFSET = 0.2f;
	float zValue;
	if (isBottEmpty)
	{
		zValue = zMin - Z_OFFSET;
	}
	else
	{
		zValue = zMax + Z_OFFSET;
	}
	zCylinder(_model, boundary, isBottEmpty, zValue);
	buildBott(_model, boundary, isBottEmpty, zValue);
	deleteGuard.flush();


}

Hix::Engine3D::Mesh* Hix::Features::TwoManifoldBuilder::execute()
{
	Hix::Features::PoissonRemesh remesher;
	return remesher.remesh(_model);
}
