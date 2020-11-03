#include "mesh.h"

#include <QCoreApplication>
#include "utils/mathutils.h"
#include "../../common/Debug.h"
#include <list>
#include <set>
#include "../../render/SceneEntity.h"
#include "Bounds3D.h"

#if defined(_DEBUG) || defined(QT_DEBUG )
#define _STRICT_MESH
//#define _STRICT_MESH_NO_SELF_INTERSECTION
#endif
using namespace Hix::Utils::Math;
using namespace Hix;
using namespace Hix::Engine3D;
using namespace Hix::Render;
using namespace Hix::Debug;
using namespace ClipperLib;


QDebug Hix::Debug::operator<< (QDebug d, const VertexConstItr& obj) {
	d << "VertexConstItr index: " << obj.index() << "\n";
	d << "  pos:" << obj.localPosition() << "\n";
	d << "  vn:" << obj.localVn() << "\n";

	d << "  leaving edges indices:" << "\n";
	for (auto each : obj.leavingEdges())
	{
		d << "    " << each.index() << "\n";
	}
	d << "  arriving edges indices:" << "\n";
	for (auto each : obj.arrivingEdges())
	{
		d << "    " << each.index() << "\n";
	}
	return d;
}

QDebug Hix::Debug::operator<< (QDebug d, const HalfEdgeConstItr& obj) {
	d << "HalfEdgeConstItr index " << obj.index() <<"\n";
	d << "  owning face index:" << obj.owningFace().index() << "\n";
	d << "  twins indicies:" << "\n";
	auto twins = obj.twins();
	for (auto each : twins)
	{
		d << "    " << each.index() << "\n";
	}
	d << "  from vtx: " << "\n";
	d << obj.from()<< "\n";
	d << "  to vtx" << "\n";
	d << obj.to()<< "\n";
	return d;
}

QDebug Hix::Debug::operator<< (QDebug d, const FaceConstItr& obj) {
	d << "FaceConstItr index: " << obj.index() << "\n";
	d << "  fn: " << obj.localFn() << "\n";
	d << "  orderedZ: \n";
	auto orderedZ = obj.sortZ();
	for (auto each : orderedZ)
	{
		d << "    " << each << "\n";
	}
	d << "  face hEdges:" << "\n";
	auto hEdgeCirc = obj.edge();
	for (size_t i = 0; i < 3; ++i)
	{
		d << hEdgeCirc << "\n";
		hEdgeCirc.moveNext();
	}
	return d;
}

const TrackedIndexedList<MeshVertex, std::allocator<MeshVertex>, VertexItrFactory>& Mesh::getVertices()const
{
	return vertices;
}
const TrackedIndexedList<MeshFace, std::allocator<MeshFace>, FaceItrFactory>& Mesh::getFaces()const
{
	return faces;
}
const TrackedIndexedList<HalfEdge, std::allocator<HalfEdge>, HalfEdgeItrFactory>& Mesh::getHalfEdges()const
{
	return halfEdges;
}

TrackedIndexedList<MeshVertex, std::allocator<MeshVertex>, VertexItrFactory>& Mesh::getVertices()
{
	return vertices;
}
TrackedIndexedList<MeshFace, std::allocator<MeshFace>, FaceItrFactory>& Mesh::getFaces()
{
	return faces;
}
TrackedIndexedList<HalfEdge, std::allocator<HalfEdge>, HalfEdgeItrFactory>& Mesh::getHalfEdges()
{
	return halfEdges;
}

Hix::Engine3D::MeshVertex::MeshVertex(QVector3D pos) : position(pos)
{
}

Hix::Engine3D::MeshIteratorFactory::MeshIteratorFactory()
{
}
Hix::Engine3D::MeshIteratorFactory::MeshIteratorFactory(Mesh* mesh) : _mesh(mesh) {}


HalfEdgeItrFactory::iterator HalfEdgeItrFactory::buildIterator(size_t index, const HalfEdgeItrFactory::containerType* containerPtr) const
{
	return HalfEdgeItrFactory::iterator(index, _mesh);
}

HalfEdgeItrFactory::const_iterator HalfEdgeItrFactory::buildConstIterator(size_t index, const HalfEdgeItrFactory::containerType* containerPtr) const
{
	return HalfEdgeItrFactory::const_iterator(index, _mesh);
}
VertexItrFactory::iterator VertexItrFactory::buildIterator(size_t index, const VertexItrFactory::containerType* containerPtr) const
{
	return VertexItrFactory::iterator(index, _mesh);
}

VertexItrFactory::const_iterator VertexItrFactory::buildConstIterator(size_t index, const VertexItrFactory::containerType* containerPtr) const
{
	return VertexItrFactory::const_iterator(index, _mesh);
}
FaceItrFactory::iterator FaceItrFactory::buildIterator(size_t index, const FaceItrFactory::containerType* containerPtr) const
{
	return FaceItrFactory::iterator(index, _mesh);
}

FaceItrFactory::const_iterator FaceItrFactory::buildConstIterator(size_t index, const FaceItrFactory::containerType* containerPtr) const
{
	return FaceItrFactory::const_iterator(index, _mesh);
}




Mesh::Mesh():
	vertices(VertexItrFactory(this)), halfEdges(HalfEdgeItrFactory(this)), faces(FaceItrFactory(this))
{
	//add indexChanged callbacks for TrackedIndexLists

	vertices.addIndexChangedCallback(std::bind(&Mesh::vtxIndexChangedCallback, this, std::placeholders::_1, std::placeholders::_2));
	faces.addIndexChangedCallback(std::bind(&Mesh::faceIndexChangedCallback, this, std::placeholders::_1, std::placeholders::_2));
	halfEdges.addIndexChangedCallback(std::bind(&Mesh::hEdgeIndexChangedCallback, this, std::placeholders::_1, std::placeholders::_2));

}

Mesh::Mesh(const Mesh& o)
{
	_verticesHash = o._verticesHash;
	vertices = o.vertices;
	halfEdges = o.halfEdges;
	faces = o.faces;
	//datas themselves do not contain dependency to mesh object
	//but we use custom iterators to embed dependency to mesh object in order to tranverse to other datas
	//ie) face -> edge and so on...
	vertices.setItrFactory(VertexItrFactory(this));
	halfEdges.setItrFactory(HalfEdgeItrFactory(this));
	faces.setItrFactory(FaceItrFactory(this));
	//rebind callbacks;
	vertices.addIndexChangedCallback(std::bind(&Mesh::vtxIndexChangedCallback, this, std::placeholders::_1, std::placeholders::_2));
	faces.addIndexChangedCallback(std::bind(&Mesh::faceIndexChangedCallback, this, std::placeholders::_1, std::placeholders::_2));
	halfEdges.addIndexChangedCallback(std::bind(&Mesh::hEdgeIndexChangedCallback, this, std::placeholders::_1, std::placeholders::_2));
}
Mesh& Mesh::operator+=(const Mesh& o)
{
	auto end = o.getFaces().end();
    for(auto curr = o.getFaces().begin(); curr != end; ++curr)
    {
        addFace(curr);
    }
    return *this;
}
//Mesh& Mesh::operator=(const Mesh o)
//{
//}

/********************** Mesh Edit Functions***********************/

void Mesh::vertexApplyTransformation(const Qt3DCore::QTransform& transform)
{
	for (auto& vertex : vertices)
	{
		QVector4D posVec (vertex.position, 1);
		vertex.position = (transform.matrix() * posVec).toVector3D();
	};
	rehashVtcs();
}

void Mesh::vertexOffset(float factor){
	vertices.markChangedAll();
	auto end = vertices.end();
	for(auto vtxItr = vertices.begin(); vtxItr != end; ++vtxItr)
	{
		QVector3D tmp = vtxItr.localPosition() + vtxItr.localVn() * factor;
		vtxItr.ref().position = tmp;
	};
	rehashVtcs();
}

void Mesh::vertexRotate(const QQuaternion& rot) {
	for (auto& vertex : vertices)
	{
		vertex.position = rot.rotatedVector(vertex.position);
	};
	rehashVtcs();

}
void Hix::Engine3D::Mesh::rehashVtcs()
{
	_verticesHash.clear();
	auto vEnd = vertices.cend();
	for (auto itr = vertices.cbegin(); itr != vEnd; ++itr)
	{
		auto hashval = _vtxHasher(itr.localPosition());
		_verticesHash.insert({ hashval, itr });
	}
}

void Mesh::vertexMove(const QVector3D& direction) {
	vertices.markChangedAll();
	for (auto& vertex : vertices)
	{
		QVector3D tmp = direction + vertex.position;
		vertex.position = tmp;
	};
	rehashVtcs();

}

QVector3D Mesh::centerMesh(){
	Bounds3D bound;
	bound.localBoundUpdate(*this);
	QVector3D movement = -bound.centre();
	vertexMove(movement);
	return movement;
}




void Mesh::reverseFace(FaceItr faceItr)
{
	auto hEdge = faceItr.edge();
	std::array<HalfEdgeItr, 3> halfEItrs;
	std::array<uint32_t, 3> prevArray;

	for (size_t i = 0; i < 3; ++i)
	{
		halfEItrs[i] = hEdge;
		prevArray[(i+2)%3] = hEdge->next;
		hEdge.moveNext();
	}
	for (size_t i = 0; i < 3; ++i)
	{
		auto fromVtx = halfEItrs[i].from();
		auto toVtx = halfEItrs[i].to();
		auto currIndex = halfEItrs[i].index();
		fromVtx->leavingEdges.erase(currIndex);
		fromVtx->arrivingEdges.insert(currIndex);
		toVtx->leavingEdges.insert(currIndex);
		toVtx->arrivingEdges.erase(currIndex);
		halfEItrs[i]->next = prevArray[i];
		std::swap(halfEItrs[i]->from, halfEItrs[i]->to);
	}
}


void Mesh::reverseFaces(){

    halfEdges.markChangedAll();
    vertices.markChangedAll();
	size_t count = 0;
	auto end = faces.end();
	for (auto currFace = faces.begin(); currFace != end; ++currFace)
	{
		reverseFace(currFace);
	}
}


/********************** Mesh Generation Functions **********************/

void Hix::Engine3D::Mesh::clear()
{
	_verticesHash.clear();
	vertices.clear();
	halfEdges.clear();
	faces.clear();
}

bool Mesh::addFace(const QVector3D& v0, const QVector3D& v1, const QVector3D& v2){
	std::array<size_t, 3> fVtx;
    fVtx[0] = addOrRetrieveFaceVertex(v0);
	fVtx[1] = addOrRetrieveFaceVertex(v1);
	fVtx[2] = addOrRetrieveFaceVertex(v2);
	//if the face is too small and slicing option collapsed a pair of its vertices, don't add.
	if (fVtx[0] == fVtx[1] || fVtx[0] == fVtx[2] || fVtx[1] == fVtx[2])
		return false;
    Hix::Engine3D::MeshFace mf;
	faces.emplace_back(mf);
	addHalfEdgesToFace(fVtx, faces.size() - 1);
	return true;
}

bool Mesh::addFace(const FaceConstItr& face)
{
	auto origMVs = face.meshVertices();
	std::array<QVector3D, 3> vtxPos{
		origMVs[0].localPosition(),
		origMVs[1].localPosition(),
		origMVs[2].localPosition()
	};
	std::array<size_t, 3> fVtx;
	fVtx[0] = addOrRetrieveFaceVertex(vtxPos[0]);
	fVtx[1] = addOrRetrieveFaceVertex(vtxPos[1]);
	fVtx[2] = addOrRetrieveFaceVertex(vtxPos[2]);
	Hix::Engine3D::MeshFace mf;
	faces.emplace_back(mf);
	auto faceItr = faces.cend() - 1;
	addHalfEdgesToFace(fVtx, faces.size() - 1);
	return true;
}

MeshDeleteGuard Hix::Engine3D::Mesh::removeFacesWithoutShifting(const std::unordered_set<FaceConstItr>& faceItrs)
{
	MeshDeleteGuard delguardContainer(this);
	auto& vtxDelGuard = delguardContainer.vtxDeleteGuard;
	auto& hEdgeDelGuard = delguardContainer.hEdgeDeleteGuard;
	auto& faceDelGuard = delguardContainer.faceDeleteGuard;
	std::unordered_set<VertexItr> maybeEmptyVtcs;
	for (auto& faceConstItr : faceItrs)
	{
		auto faceItr = faces.toNormItr(faceConstItr);
		faceDelGuard.deleteLater(faceItr);
		//for each half edge, remove vertex relations
		auto edge = faceItr.edge();
		for (size_t i = 0; i < 3; ++i)
		{
			auto from = edge.from();
			auto to = edge.to();
			from.ref().leavingEdges.erase(edge.index());
			to.ref().arrivingEdges.erase(edge.index());
			maybeEmptyVtcs.insert(from);
			hEdgeDelGuard.deleteLater(edge);
			edge.moveNext();
		}
	}
	//remove disconnected vertices
	for (auto& vtxItr : maybeEmptyVtcs)
	{
		if (vtxItr->arrivingEdges.empty() && vtxItr->leavingEdges.empty())
		{
			removeVertexHash(vtxItr.localPosition());
			vtxDelGuard.deleteLater(vtxItr);
		}
	}
	return delguardContainer;
}



void Mesh::removeFaces(const std::unordered_set<FaceConstItr>& faceItrs){
	auto delguard = removeFacesWithoutShifting(faceItrs);
}




/********************** Helper Functions **********************/

//check if a,b are from same manifold
bool Hix::Engine3D::isCommonManifoldFace(const FaceConstItr& a, const FaceConstItr& b, std::unordered_set<FaceConstItr> pool)
{

	std::queue<FaceConstItr> frontier;
	frontier.push(a);
	while (!frontier.empty() && !pool.empty())
	{
		auto curr = frontier.front();
		frontier.pop();
		pool.erase(curr);
		if (curr == b)
			return true;
		else
		{
			auto edge = curr.edge();
			for (size_t i = 0; i < 3; ++i)
			{
				auto nFaces = edge.twinFaces();
				for (auto nFace : nFaces)
				{
					if (pool.find(nFace) != pool.end())
					{
						frontier.push(nFace);
					}
				}
			}
		}
	}
	return false;
}

//out of candidates, find face that is on commmon manifold as a. candidates is a subset of pool....
bool Hix::Engine3D::findCommonManifoldFace(
	FaceConstItr& result, const FaceConstItr& a, std::unordered_set<FaceConstItr>& candidates, std::unordered_set<FaceConstItr> pool)
{
	std::queue<FaceConstItr> frontier;
	frontier.push(a);
	while (!frontier.empty() && !pool.empty())
	{
		auto curr = frontier.front();
		frontier.pop();
		pool.erase(curr);
		if (candidates.find(curr) != candidates.end())
		{
			result = curr;
			return true;
		}
		else
		{
			auto edge = curr.edge();
			for (size_t i = 0; i < 3; ++i)
			{
				auto nFaces = edge.twinFaces();
				for (auto nFace : nFaces)
				{
					if (pool.find(nFace) != pool.end())
					{
						frontier.push(nFace);
					}
				}
				
			}
		}
	}
	return false;
}



void Mesh::removeVertexHash(QVector3D pos)
{
	auto toBeRemoved = _verticesHash.equal_range(_vtxHasher(pos));
	for (auto& nVtxItr = toBeRemoved.first; nVtxItr != toBeRemoved.second; ++nVtxItr)
	{
		if (nVtxItr->second.localPosition() == pos)
		{
			_verticesHash.erase(nVtxItr);
			break;
		}
	}
}


void Hix::Engine3D::Mesh::setSceneEntity(const Render::SceneEntity* entity)
{
	_entity = entity;
}

const Hix::Render::SceneEntity* Hix::Engine3D::Mesh::entity() const
{
	return _entity;
}


QVector4D Hix::Engine3D::Mesh::toWorld(const QVector4D& local)const
{
	return _entity->toRootCoord(local);
}

QVector4D Hix::Engine3D::Mesh::toLocal(const QVector4D& world)const
{
	return _entity->toLocalCoord(world);
}

QVector3D Hix::Engine3D::Mesh::ptToWorld(const QVector3D& local) const
{
	return _entity->ptToRoot(local);
}

QVector3D Hix::Engine3D::Mesh::vectorToWorld(const QVector3D& local) const
{
	return _entity->vectorToRoot(local);
}

QVector3D Hix::Engine3D::Mesh::ptToLocal(const QVector3D& world) const
{
	return _entity->ptToLocal(world);
}

QVector3D Hix::Engine3D::Mesh::vectorToLocal(const QVector3D& world) const
{
	return _entity->vectorToLocal(world);
}

VertexConstItr Hix::Engine3D::Mesh::getVtxAtLocalPos(const QVector3D& pos) const
{
	auto hashval = _vtxHasher(pos);
	auto binVtxs = _verticesHash.equal_range(hashval);
	for (auto& curr = binVtxs.first; curr != binVtxs.second; ++curr)
	{
		if (curr->second.localPosition() == pos)
		{
			return curr->second;
		}
	}
	return VertexConstItr();
}

std::unordered_map<FaceConstItr, QVector3D> Hix::Engine3D::Mesh::cacheWorldFN() const
{
	std::unordered_map<FaceConstItr, QVector3D> result;
	result.reserve(faces.size());
	auto cend = faces.cend();
	for (auto itr = faces.cbegin(); itr != cend; ++itr)
	{
		result.emplace(std::make_pair(itr, itr.worldFn()));
	}
	return result;
}

std::unordered_map<VertexConstItr, QVector3D> Hix::Engine3D::Mesh::cacheWorldPos() const
{
	std::unordered_map<VertexConstItr, QVector3D> result;
	result.reserve(vertices.size());
	auto cend = vertices.cend();
	for (auto itr = vertices.cbegin(); itr != cend; ++itr)
	{
		result.emplace(std::make_pair(itr, itr.worldPosition()));
	}
	return result;
}

std::unordered_map<VertexConstItr, QVector3D> Hix::Engine3D::Mesh::cacheWorldVN() const
{
	std::unordered_map<VertexConstItr, QVector3D> result;
	result.reserve(vertices.size());
	auto cend = vertices.cend();
	for (auto itr = vertices.cbegin(); itr != cend; ++itr)
	{
		result.emplace(std::make_pair(itr, itr.worldVn()));
	}
	return result;
}


size_t Mesh::addOrRetrieveFaceVertex(const QVector3D& v){
	//find if existing vtx can be used
	auto hashval = _vtxHasher(v);
	auto binVtxs = _verticesHash.equal_range(hashval);
	for (auto& curr = binVtxs.first; curr != binVtxs.second; ++curr)
	{
		if (curr->second.localPosition() == v)
		{
			return curr->second.index();
		}
	}
    MeshVertex mv(v);
    vertices.emplace_back(mv);
	auto last = vertices.cend() - 1;
	_verticesHash.insert({ hashval, last });
    return  last.index();
}

void Hix::Engine3D::Mesh::addHalfEdgesToFace(std::array<size_t, 3> faceVertices, size_t faceIdx)
{
	//a mesh face aggregates half edges(3 to 1) so there is no need to hash half edges when adding them with new face
	halfEdges.emplace_back(HalfEdge());
	halfEdges.emplace_back(HalfEdge());
	halfEdges.emplace_back(HalfEdge());

	auto firstAddedItr = halfEdges.toNormItr(halfEdges.cend() - 3);
	size_t vtxIdx = 0;
	
	HalfEdgeItr nextItr;
	for (auto itr = firstAddedItr; itr != halfEdges.end(); ++itr)
	{
		//add vertices in (from, to) pairs, so (0,1) (1,2) (2,0)
		itr.ref().from= faceVertices[vtxIdx % 3];
		itr.ref().to = faceVertices[(vtxIdx + 1) % 3];

		//add "owning" face or face that the hEdge circuit creates
		itr.ref().owningFace = faceIdx;
		//for each vertices that the half edges are "leaving" from, add the half edge reference
		vertices[faceVertices[vtxIdx % 3]].leavingEdges.insert(itr.index());
		vertices[faceVertices[(vtxIdx + 1) % 3]].arrivingEdges.insert(itr.index());

		//add circular relationship for all half edges
		nextItr = itr + 1;
		//since we can't use % on itrs
		if (nextItr == halfEdges.end())
		{
			nextItr = firstAddedItr;
		}
		itr.ref().next = nextItr.index();
		++vtxIdx;

	}
	faces[faceIdx].edge = firstAddedItr.index();
}




void Mesh::vtxIndexChangedCallback(size_t oldIdx, size_t newIdx)
{
	//vertex whose index has been changed
	auto& vtx = vertices[newIdx];
	//new position for the shifted vtx;
	//update leaving edges
	for (auto leavingEdge : vtx.leavingEdges)
	{
		auto& modLeavingEdge = halfEdges[leavingEdge];
		modLeavingEdge.from= newIdx;
		//get twin edges with opposite direction ie) arriving 
	}
	//update arrivingEdges
	for (auto arrivingEdge : vtx.arrivingEdges)
	{
		auto& modArrEdge = halfEdges[arrivingEdge];
		modArrEdge.to= newIdx;
	}
	auto newIndexItr = vertices.cbegin() + newIdx;
	//update hash value
	auto oldItr = vertices.cbegin() + oldIdx;
	auto hashVal = _vtxHasher(vtx.position);
	auto binVtxs = _verticesHash.equal_range(hashVal);
	for (auto& curr = binVtxs.first; curr != binVtxs.second; ++curr)
	{
		if (curr->second == oldItr)
		{
			_verticesHash.erase(curr);
			_verticesHash.insert({ hashVal , newIndexItr});
			break;
		}
	}
}

void Mesh::faceIndexChangedCallback(size_t oldIdx, size_t newIdx)
{
	//face whose index has been changed
	auto& face = faces[newIdx];
	//new position for the shifted face;
	//for each half edges "owned" by this face, update the iterator
	auto edge = HalfEdgeItr(face.edge, this);
	for (size_t i = 0; i < 3; ++i)
	{
		edge.ref().owningFace = newIdx;
		edge.moveNext();
	}
}

void Mesh::hEdgeIndexChangedCallback(size_t oldIdx, size_t newIdx)
{
	//halfEdge whose index has been changed
	HalfEdgeItr hEdge(newIdx,this);
	//update face that owns this half edge only if the itr owned by the face is this one
	if (hEdge.owningFace()->edge == oldIdx)
	{
		hEdge.owningFace()->edge = newIdx;
	}
	//update vertices that have reference to this edge
	auto& from = vertices[hEdge.from().index()];
	auto& to = vertices[hEdge.to().index()];
	from.leavingEdges.erase(oldIdx);
	from.leavingEdges.insert(newIdx);
	to.arrivingEdges.erase(oldIdx);
	to.arrivingEdges.insert(newIdx);
	hEdge.prev()->next = newIdx;
}






std::unordered_set<FaceConstItr> Mesh::findNearSimilarFaces(QVector3D normal, FaceConstItr  mf, float maxNormalDiff, size_t maxCount)const
{
	auto faceCond = [normal, maxNormalDiff, maxCount](const FaceConstItr& nf)->bool {
		return (nf.localFn() - normal).lengthSquared() < maxNormalDiff;
	};
	return findNearFaces(mf, faceCond, maxCount);
}

std::unordered_set<FaceConstItr> Hix::Engine3D::Mesh::findNearSimilarFaces(QVector3D pt, QVector3D normal, FaceConstItr mf, float maxNormalDiff, size_t maxCount, float radius) const
{
	auto radSquared = radius * radius;
	std::unordered_set<FaceConstItr> result;
	std::unordered_set<FaceConstItr> explored;
	std::deque<FaceConstItr>q;
	result.reserve(maxCount);
	explored.reserve(maxCount);
	q.emplace_back(mf);
	result.emplace(mf);
	explored.emplace(mf);
	while (!q.empty())
	{
		auto curr = q.front();
		q.pop_front();
		if (explored.size() == maxCount)
			break;
		auto edge = curr.edge();
		for (size_t i = 0; i < 3; ++i, edge.moveNext()) {
			auto nFaces = edge.twinFaces();
			for (auto nFace : nFaces)
			{
				if (explored.find(nFace) == explored.end())
				{
					explored.emplace(nFace);
					if ((nFace.localFn() - normal).lengthSquared() < maxNormalDiff)
					{

						auto vtcs = nFace.meshVertices();
						for (auto& v : vtcs)
						{
							//for optimized non-squarerooted distance, we subtract and find length
							auto ab = v.localPosition() - pt;
							if (ab.lengthSquared() < radSquared)
							{
								q.emplace_back(nFace);
								result.emplace(nFace);
								break;
							}
						}

					}
				}
				else
				{
					if (result.find(nFace) == result.end())
					{
						qDebug() << "overtaken face?";
					}
				}
			}
		}
	}
	return result;
}





QHash<uint32_t, Path>::iterator Hix::Engine3D::findSmallestPathHash(QHash<uint32_t, Path> pathHash){
    //QHashIterator<uint32_t, Path> j(pathHash);
    //QHashIterator<uint32_t, Path> prev_j(pathHash);
    QHash<uint32_t, Path>::iterator i = pathHash.begin();
    QHash<uint32_t, Path>::iterator biggest_i = pathHash.begin();
    for (i = pathHash.begin(); i!= pathHash.end(); ++i){
        if (biggest_i.value().size() >= i.value().size())
            biggest_i = i;
    }
    return biggest_i;
}


void findAndDeleteHash(std::vector<uint32_t>* hashList, uint32_t hash){
    for (std::vector<uint32_t>::iterator h_it = hashList->begin(); h_it != hashList->end();){
        if (*h_it == hash){
            hashList->erase(h_it);
            break;
        }
        h_it ++;
    }
}

bool contourContains(Path3D contour, MeshVertex mv){
    for (MeshVertex& cmv : contour){
        if (cmv == mv)
            return true;
    }
    return false;
}

bool listContains(std::vector<uint32_t>* hashList, uint32_t hash){
    for (std::vector<uint32_t>::iterator h_it = hashList->begin(); h_it != hashList->end();){
        if (*h_it == hash){
            return true;
        }
        h_it ++;
    }
    return false;
}


// construct closed contour using segments created from identify step
Paths3D Hix::Engine3D::contourConstruct3D(Paths3D hole_edges){
    int iter = 0;
    std::vector<Paths3D::iterator> checked_its;
    bool dirty = true; // checks if iteration erased some contour

    while (dirty){
        dirty = false;
        qDebug() << iter;
        Paths3D::iterator hole_edge1_it;
        Paths3D::iterator hole_edge2_it;

        qDebug() << "hole edge size : " << hole_edges.size() << int(hole_edges.end() - hole_edges.begin());
        /*int cnt1 =0, cnt2 =0;
        for (hole_edge1_it = hole_edges.begin(); hole_edge1_it != hole_edges.end(); ++hole_edge1_it){
            cnt2 = 0;
            for (hole_edge2_it = hole_edges.begin(); hole_edge2_it != hole_edges.end(); ++hole_edge2_it){
                qDebug() << "checking" << cnt1 << cnt2;
                cnt2 ++;
            }
            cnt1 ++;
        }*/
        int edge_lookup = 0;
        for (hole_edge1_it = hole_edges.begin(); hole_edge1_it != hole_edges.end();){
            if (edge_lookup %50 ==0)
                QCoreApplication::processEvents();
            edge_lookup ++;

            bool checked = false;
            for (Paths3D::iterator checked_it : checked_its){
                if (checked_it == hole_edge1_it){
                    checked = true;
                }
            }
            if (checked){
                hole_edge1_it ++;
                continue;
            }


            for (hole_edge2_it = hole_edges.begin(); hole_edge2_it != hole_edges.end();){
                checked = false;
                for (Paths3D::iterator checked_it : checked_its){
                    if (checked_it == hole_edge2_it){
                        checked = true;
                    }
                }
                if (checked){
                    hole_edge2_it ++;
                    continue;
                }

                if (hole_edges.size() == 1)
                    return hole_edges;
                if (hole_edge1_it == hole_edge2_it){
                    //qDebug() << "same edge it";
                    hole_edge2_it ++;
                    continue;
                }
                // prolong hole_edge 1 if end and start matches
                if ((hole_edge1_it->end()-1)->position.distanceToPoint(hole_edge2_it->begin()->position) < VTX_INBOUND_DIST *0.05/Hix::Polyclipping::INT_PT_RESOLUTION){
                //if (Vertex2Hash(*(hole_edge1_it->end()-1)) == Vertex2Hash(*hole_edge2_it->begin())){
                    //qDebug() << "erase";
                    dirty = true;
                    hole_edge1_it->insert(hole_edge1_it->end(), hole_edge2_it->begin()+1, hole_edge2_it->end());
                    checked_its.push_back(hole_edge2_it);
                    //hole_edge2_it = hole_edges.erase(hole_edge2_it);
                    //qDebug() << "erased";
                } else if ((hole_edge1_it->end()-1)->position.distanceToPoint((hole_edge2_it->end()-1)->position) < VTX_INBOUND_DIST *0.05/Hix::Polyclipping::INT_PT_RESOLUTION){
                //} else if (Vertex2Hash(*(hole_edge1_it->end()-1)) == Vertex2Hash(*(hole_edge2_it->end()-1))){
                    //qDebug() << "erase";
                    dirty = true;
                    std::reverse(hole_edge2_it->begin(), hole_edge2_it->end());

                    hole_edge1_it->insert(hole_edge1_it->end(), hole_edge2_it->begin()+1, hole_edge2_it->end());
                    checked_its.push_back(hole_edge2_it);
                    //hole_edge2_it = hole_edges.erase(hole_edge2_it);
                    //qDebug() << "erased";
                }
                hole_edge2_it ++;
            }
            hole_edge1_it ++;
        }
        qDebug() << "hole edges size " << hole_edges.size();
    }

    // select contour if size > 2
    Paths3D result_edges;

    for (Paths3D::iterator hole_edge_it = hole_edges.begin(); hole_edge_it != hole_edges.end();){
        bool checked = false;
        for (Paths3D::iterator checked_it : checked_its){
            if (checked_it == hole_edge_it){
                checked = true;
            }
        }
        if (!checked){
            result_edges.push_back(*hole_edge_it);
            qDebug() << "result_edge : " << hole_edge_it->begin()->position<< (hole_edge_it->end()-1)->position;
        }
        hole_edge_it ++;
    }

    for (Path3D result_edge : result_edges){
        qDebug() << "hole_edge size : " << result_edge.size();
    }

    /*for (Path3D hole_edge : hole_edges){
        if (hole_edge.size() > 2){
            result_edges.push_back(hole_edge);
            qDebug() << "hole_edge size : " << hole_edge.size();
        }
    }*/

    qDebug() << "result edges : " << result_edges.size();

    return result_edges;
    /*// new trial if there's 분기점
    Paths3D contourList;

    QHash<uint32_t, Path3D> pathHash;
    std::vector<uint32_t> hashList;
    hashList.reserve(hole_edges.size());

    if (hole_edges.size() == 0)
        return contourList;

    int pathCnt = 0;
    for (int i=0; i<hole_edges.size(); i++){
        pathCnt += hole_edges[i].size();
    }
    qDebug() << pathCnt;
    pathHash.reserve(pathCnt*10);

    int debug_count=0;

    for (int i=0; i<hole_edges.size(); i++){
        Path3D p = hole_edges[i];
        //insertPathHash(pathHash, p[0], p[1]); // inserts opposite too

        uint32_t path_hash_u = Vertex2Hash(p[0]);
        uint32_t path_hash_v = Vertex2Hash(p[1]);

        if (! pathHash.contains(path_hash_u)){
            debug_count ++;
            pathHash[path_hash_u].reserve(10);
            pathHash[path_hash_u].push_back(p[0]);
            hashList.push_back(path_hash_u);
        }
        if (! pathHash.contains(path_hash_v)){
            debug_count ++;
            pathHash[path_hash_v].reserve(10);
            pathHash[path_hash_v].push_back(p[1]);
            hashList.push_back(path_hash_v);
        }
        pathHash[path_hash_u].push_back(p[1]);
        pathHash[path_hash_v].push_back(p[0]);
    }

    for (uint32_t hash : hashList){
        qDebug() << "hash " <<hash << " path size : "<< pathHash[hash].size();
    }

    qDebug() << "hashList.size : " << hashList.size();

    while (pathHash.size() > 0){
        qDebug() << "new contour start" << "chosen from "<< hashList.size();
        MeshVertex start, pj_prev, pj, pj_next, last, u, v;
        Path3D contour;
        start = pathHash[hashList[0]][0];
        contour.push_back(start);
        qDebug() << "inserted pj : " << start.position;
        pj_prev = start;
        Path3D* dest = &(pathHash[hashList[0]]);
        qDebug() << "new contour dest size : " << dest->size();

        if (pathHash[Vertex2Hash(start)].size() <= 2){
            pathHash.remove(Vertex2Hash(start));
            findAndDeleteHash(&hashList, Vertex2Hash(start));
            continue;
        }

        pj = findAvailableMeshVertex(&pathHash, &hashList, start);
        last = findAvailableMeshVertex(&pathHash, &hashList, start);

        qDebug() << "current selected pj : " << pj.position;
        qDebug() << "current selected last : " << last.position;

        if (pathHash[Vertex2Hash(start)].size() <= 2){
            pathHash.remove(Vertex2Hash(start));
            findAndDeleteHash(&hashList, Vertex2Hash(start));
        }

        //while (!contourContains(contour, pj)){
        while (pj != last){
            contour.push_back(pj);
            qDebug() << "inserted pj : " << pj.position;
            qDebug() << "current contour size :" << contour.size();
            for (MeshVertex mv : pathHash[Vertex2Hash(pj)]){
                qDebug() << "current adding meshvertex neighbors : "<< mv.position;
            }

            if (pathHash[Vertex2Hash(pj)].size() <= 2){
                pathHash.remove(Vertex2Hash(pj));
                findAndDeleteHash(&hashList, Vertex2Hash(pj));
                break;
            }

            u = findAvailableMeshVertex(&pathHash, &hashList, pj);
            v = findAvailableMeshVertex(&pathHash, &hashList, pj);

            if (pathHash[Vertex2Hash(pj)].size() <= 2){
                pathHash.remove(Vertex2Hash(pj));
                findAndDeleteHash(&hashList, Vertex2Hash(pj));
            }

            qDebug() << "current selected u : " << u.position;
            qDebug() << "current selected v : " << v.position;

            if (u == pj_prev){
                pj_next = v;
            } else {
                pj_next = u;
            }
            pj_prev = pj;
            pj = pj_next;

            if (contourContains(contour, pj)){
                Path3D new_contour;
                Path3D::iterator new_start;

                for (Path3D::iterator p_it = contour.begin(); p_it != contour.end();){
                    if (*p_it == pj){
                        new_start = p_it;
                        break;
                    }
                    p_it ++;
                }

                new_contour.insert(new_contour.end(), new_start, contour.end());
                new_contour.push_back(pj);
                contourList.push_back(new_contour);
                contour = Path3D(contour.begin(), new_start);
            }
        }

        uint32_t lastHash = Vertex2Hash(pj);
        for (Path3D::iterator mv_it = pathHash[lastHash].begin(); mv_it != pathHash[lastHash].end();){
            if (*mv_it == pj_prev || *mv_it == start)
                mv_it = pathHash[lastHash].erase(mv_it);
            else
                mv_it ++;
        }

        if (pathHash[Vertex2Hash(pj)].size() <= 2){
            pathHash.remove(Vertex2Hash(pj));
            findAndDeleteHash(&hashList, Vertex2Hash(pj));
        }

        contour.push_back(pj);
        contourList.push_back(contour);
    }

    return contourList;*/
}

bool Hix::Engine3D::intPointInPath(IntPoint ip, Path p){
    for (IntPoint i : p){
        if ((ip.X == i.X) && (ip.Y == i.Y)){
            return true;
        }
    }
    return false;
};

bool Hix::Engine3D::pathInpath(Path3D target, Path3D in){
    for (IntPoint ip : target.projection){
        if (PointInPolygon(ip, in.projection)==0){ // if ip is not contained in in
            return false;
        }
    }
    return true;
}

std::vector<std::array<QVector3D, 3>> Hix::Engine3D::interpolate(Path3D from, Path3D to){
    std::vector<std::array<QVector3D, 3>> result_faces;
    if (from.size() != to.size()){
        qDebug() << "from and to size differs";
        return result_faces;
    }
    if (from.size() <3){
        qDebug() << "from path size small :" << from.size();
        return result_faces;
    }

    result_faces.reserve(from.size()*2);

    qDebug() << from.size() << to.size();

    // add odd number faces
    for (int i=1; i<from.size()-1; i++){
        if (i%2 != 0){
            // add right left new face
            std::array<QVector3D,3> temp_face1;
            temp_face1[0] = to[i].position;
            temp_face1[1] = from[i-1].position;
            temp_face1[2] = from[i].position;
            result_faces.push_back(temp_face1);
            std::array<QVector3D,3> temp_face2;
            temp_face2[0] = to[i].position;
            temp_face2[1] = from[i].position;
            temp_face2[2] = from[i+1].position;
            result_faces.push_back(temp_face2);
        }
    }
    qDebug() << "reserved odd number faces" << result_faces.size();

    // add even number faces
    for (int i=2; i<from.size()-1; i++){
        if (i%2 == 0){
            // add right left new face
            std::array<QVector3D,3> temp_face1;
            temp_face1[0] = to[i].position;
            temp_face1[1] = from[i-1].position;
            temp_face1[2] = from[i].position;
            result_faces.push_back(temp_face1);
            std::array<QVector3D,3> temp_face2;
            temp_face2[0] = to[i].position;
            temp_face2[1] = from[i].position;
            temp_face2[2] = from[i+1].position;
            result_faces.push_back(temp_face2);
        }
    }
    qDebug() << "reserved even number faces" << result_faces.size();

    // add right left new face
    std::array<QVector3D,3> temp_face1;
    temp_face1[0] = to[from.size()-1].position;
    temp_face1[1] = from[from.size()-2].position;
    temp_face1[2] = from[from.size()-1].position;
    result_faces.push_back(temp_face1);
    std::array<QVector3D,3> temp_face2;
    temp_face2[0] = to[from.size()-1].position;
    temp_face2[1] = from[from.size()-1].position;
    temp_face2[2] = from[0].position;
    result_faces.push_back(temp_face2);

    return result_faces;
}





/************** Helper Functions *****************/

size_t Hix::Engine3D::MeshVtxHasher::operator()(const QVector3D& hashed) const
{
	//range of int21 = -1048576, 1048575, roughly 4m on 2micron resolution
	//range of uint21 = 0, 2097151
	//2^21 - 1
	static constexpr int32_t INT21_MAX_HALF = 1048576;
	static constexpr uint64_t UINT_21_MASK = 2097151; //0b111111...for 21 bits
	//using uint21, so 63 bits for all
	uint64_t digest = 0;
	for (size_t i = 0; i < 3; ++i)
	{
		int32_t signedMicron = int32_t((hashed[i] + VTX_INBOUND_DITTER) / VTX_INBOUND_DIST);
		uint64_t unsignedMicron = std::max(0, (signedMicron + INT21_MAX_HALF));
		//discard top 11 bits so 21 bits valid, shift to correct position
		unsignedMicron = (unsignedMicron & UINT_21_MASK) << (21 * i);
		digest = digest | unsignedMicron;
	}
	return	digest;
}

Hix::Engine3D::MeshDeleteGuard::MeshDeleteGuard(Mesh* mesh): 
	vtxDeleteGuard(mesh->getVertices().getDeleteGuard()), hEdgeDeleteGuard(mesh->getHalfEdges().getDeleteGuard()), faceDeleteGuard(mesh->getFaces().getDeleteGuard())
{
}

void Hix::Engine3D::MeshDeleteGuard::flush()
{
	vtxDeleteGuard.flush();
	hEdgeDeleteGuard.flush();
	faceDeleteGuard.flush();
}

MeshDeleteGuard& Hix::Engine3D::MeshDeleteGuard::operator+=(MeshDeleteGuard&& other)
{
#ifdef _DEBUG
	if (vtxDeleteGuard.container() != other.vtxDeleteGuard.container() ||
		hEdgeDeleteGuard.container() != other.hEdgeDeleteGuard.container() ||
		faceDeleteGuard.container() != other.faceDeleteGuard.container())
	{
		throw std::runtime_error("MeshDeleteGuard appending delete guards of different containers");
	}
#endif
	vtxDeleteGuard += std::move(other.vtxDeleteGuard);
	hEdgeDeleteGuard += std::move(other.hEdgeDeleteGuard);
	faceDeleteGuard += std::move(other.faceDeleteGuard);
	return *this;
}
