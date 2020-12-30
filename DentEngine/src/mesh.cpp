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
#include "../../common/Debug.h"
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

std::optional<std::array<size_t, 3>> Mesh::addFace(const QVector3D& v0, const QVector3D& v1, const QVector3D& v2){
	std::array<size_t, 3> fVtx;
    fVtx[0] = addOrRetrieveFaceVertex(v0);
	fVtx[1] = addOrRetrieveFaceVertex(v1);
	fVtx[2] = addOrRetrieveFaceVertex(v2);
	//if the face is too small and slicing option collapsed a pair of its vertices, don't add.
	if (fVtx[0] == fVtx[1] || fVtx[0] == fVtx[2] || fVtx[1] == fVtx[2])
		return {};
    Hix::Engine3D::MeshFace mf;
	faces.emplace_back(mf);
	addHalfEdgesToFace(fVtx, faces.size() - 1);
	return fVtx;
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
		modLeavingEdge.from = newIdx;
		//get twin edges with opposite direction ie) arriving 
	}
	//update arrivingEdges
	for (auto arrivingEdge : vtx.arrivingEdges)
	{
		auto& modArrEdge = halfEdges[arrivingEdge];
		modArrEdge.to = newIdx;
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
			_verticesHash.insert({ hashVal , newIndexItr });
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
	HalfEdgeItr hEdge(newIdx, this);
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
	std::unordered_set<VertexConstItr> bannedVtcs;
	std::unordered_set<FaceConstItr> fails;

	auto faceCond = [normal, maxNormalDiff, maxCount, &fails](const FaceConstItr& nf)->bool {

		if ((nf.localFn() - normal).lengthSquared() < maxNormalDiff)
		{
			return true;
		}
		else
		{
			fails.insert(nf);
			return false;
		}
	};
	auto res = findNearFaces(mf, faceCond, maxCount);
	//auto entity = dynamic_cast<SceneEntityWithMaterial*>(const_cast<SceneEntity*> (this->entity()));
	//auto& debugger = Hix::Debug::DebugRenderObject::getInstance();
	//debugger.clear();
	//debugger.registerDebugColorFaces(entity, fails, Hix::Render::Colors::DebugRed);
	//debugger.colorDebugFaces();
	return res;
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
