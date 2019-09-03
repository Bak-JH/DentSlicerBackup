#include "mesh.h"
#include "qmlmanager.h"
#include <QDebug>
#include <QCoreApplication>
#include "utils/mathutils.h"
#include "configuration.h"

#include <list>
#include <set>
#if defined(_DEBUG) || defined(QT_DEBUG )
#define _STRICT_MESH
//#define _STRICT_MESH_NO_SELF_INTERSECTION
#endif
using namespace Utils::Math;
using namespace Hix;
using namespace Hix::Engine3D;
using namespace Hix::Debug;
using namespace ClipperLib;


QDebug Hix::Debug::operator<< (QDebug d, const VertexConstItr& obj) {
	d << "VertexConstItr index: " << obj.index() << "\n";
	d << "  pos:" << obj->position << "\n";
	d << "  vn:" << obj->vn << "\n";

	d << "  leaving edges indices:" << "\n";
	for (auto each : obj->leavingEdges)
	{
		d << "    " << each.index() << "\n";
	}
	d << "  arriving edges indices:" << "\n";
	for (auto each : obj->arrivingEdges)
	{
		d << "    " << each.index() << "\n";
	}
	return d;
}

QDebug Hix::Debug::operator<< (QDebug d, const HalfEdgeConstItr& obj) {
	d << "HalfEdgeConstItr index " << obj.index() <<"\n";
	d << "  owning face index:" << obj->owningFace.index() << "\n";
	d << "  twins indicies:" << "\n";
	auto twins = obj->twins();
	for (auto each : twins)
	{
		d << "    " << each.index() << "\n";
	}
	d << "  from vtx: " << "\n";
	d << obj->from << "\n";
	d << "  to vtx" << "\n";
	d << obj->to << "\n";
	return d;
}

QDebug Hix::Debug::operator<< (QDebug d, const FaceConstItr& obj) {
	d << "FaceConstItr index: " << obj.index() << "\n";
	d << "  fn: " << obj->fn << "\n";
	d << "  orderedZ: " << obj->fn << "\n";
	auto orderedZ = obj->sortZ();
	for (auto each : orderedZ)
	{
		d << "    " << each << "\n";
	}
	d << "  face hEdges:" << "\n";
	auto hEdgeCirc = obj->edgeCirculator();
	for (size_t i = 0; i < 3; ++i)
	{
		d << hEdgeCirc.toItr() << "\n";
	}
	return d;
	}

//half edge circulator

Hix::Engine3D::HalfEdgeCirculator::HalfEdgeCirculator(HalfEdgeConstItr itr): _hEdgeItr(itr)
{
}

//HalfEdgeConstItr& Hix::Engine3D::HalfEdgeCirculator::toItrW() const
//{
//	return _hEdgeItr;
//}

HalfEdgeConstItr& Hix::Engine3D::HalfEdgeCirculator::toItr()
{
	return _hEdgeItr;
}




const HalfEdge& Hix::Engine3D::HalfEdgeCirculator::operator*() const
{
	return *_hEdgeItr;
}

void Hix::Engine3D::HalfEdgeCirculator::operator++()
{
	_hEdgeItr = _hEdgeItr->next;
}

void Hix::Engine3D::HalfEdgeCirculator::operator--()
{
	_hEdgeItr = _hEdgeItr->prev;
}

HalfEdgeCirculator Hix::Engine3D::HalfEdgeCirculator::operator--(int)
{
	auto tmp = *this;
	this->operator--();
	return tmp;
}

HalfEdgeCirculator Hix::Engine3D::HalfEdgeCirculator::operator++(int)
{
	auto tmp = *this;
	this->operator++();
	return tmp;
}

const HalfEdge* Hix::Engine3D::HalfEdgeCirculator::operator->() const
{
	return _hEdgeItr.operator->();
}

const HalfEdge* Hix::Engine3D::HalfEdgeCirculator::toPtr() const
{
	return _hEdgeItr.operator->();
}




std::array<VertexConstItr, 3> Hix::Engine3D::MeshFace::meshVertices() const
{
	std::array<VertexConstItr, 3> result;
	auto circulator = edgeCirculator();
	for (size_t i = 0; i < 3; ++i)
	{
		result[i] = circulator->from;
		++circulator;
	}
	return result;

}

HalfEdgeCirculator Hix::Engine3D::MeshFace::edgeCirculator()const
{
	return HalfEdgeCirculator(edge);
}

std::array<size_t, 3> Hix::Engine3D::MeshFace::getVerticeIndices(const Mesh* owningMesh)const
{
	std::array<size_t, 3> result;
	auto faceVs = meshVertices();
	for (size_t i = 0; i < 3; ++i)
	{
		result[i] = faceVs[i] - owningMesh->getVertices().cbegin();
	}
	return result;
}

std::unordered_set<HalfEdgeConstItr> Hix::Engine3D::HalfEdge::twins()const
{
	std::unordered_set<HalfEdgeConstItr> twinEdges;
	for (auto oppDirEdge : to->leavingEdges)
	{
		if (oppDirEdge->to == from)
		{
			twinEdges.insert(oppDirEdge);
		}
	}
	return twinEdges;
}


//twins in same direction
std::unordered_set<HalfEdgeConstItr> Hix::Engine3D::HalfEdge::nonTwins()const
{
	std::unordered_set<HalfEdgeConstItr> nonTwins;
	for (auto sameDirEdge : from->leavingEdges)
	{
		if (sameDirEdge->to == to && sameDirEdge.operator->() != this)
		{
			nonTwins.insert(sameDirEdge);
		}
	}
	return nonTwins;
}
//twins + nonTwins
std::unordered_set<HalfEdgeConstItr> Hix::Engine3D::HalfEdge::allFromSameEdge()const
{
	auto allEdges = twins();
	auto tmp = nonTwins();
	allEdges.insert(tmp.begin(), tmp.end());
	return allEdges;
}

bool Hix::Engine3D::HalfEdge::isTwin(const HalfEdgeConstItr& other)const
{
	auto twns = twins();
	return twns.find(other) != twns.end();
}


std::vector<FaceConstItr> Hix::Engine3D::HalfEdge::nonOwningFaces()const
{
	std::vector<FaceConstItr> result;
	auto otherEdges = allFromSameEdge();
	for (auto each : otherEdges)
	{
		result.push_back(each->owningFace);
	}
	return result;
}
std::unordered_set<FaceConstItr> Hix::Engine3D::HalfEdge::twinFaces()const
{
	std::unordered_set<FaceConstItr> twnFaces;
	auto twns = twins();
	for (auto& each : twns)
	{
		twnFaces.insert(each->owningFace);
	}
	return twnFaces;
}

std::vector<FaceConstItr> Hix::Engine3D::MeshVertex::connectedFaces()const
{
	std::vector<FaceConstItr> result;
	for (auto each : leavingEdges)
	{
		result.push_back(each->owningFace);
	}
	return result;
}

std::unordered_set<VertexConstItr> Hix::Engine3D::MeshVertex::connectedVertices() const
{
	std::unordered_set<VertexConstItr> connected;
	for (auto& each : leavingEdges)
	{
		connected.insert(each->to);
	}
	for (auto& each : arrivingEdges)
	{
		connected.insert(each->from);
	}
	return connected;
}

bool MeshVertex::empty()const
{
	if (leavingEdges.empty() && arrivingEdges.empty())
		return true;
	else
		false;
}


void Hix::Engine3D::MeshVertex::calculateNormalFromFaces()
{
	vn = { 0,0,0 };
	auto faces = connectedFaces();
    for (auto& face : faces)
	{
		vn += face->fn;
	}
    vn.normalize();
}


Mesh::Mesh()
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

	_x_min = o._x_min;
	_x_max = o._x_max;
	_y_min = o._y_min;
	_y_max = o._y_max;
	_z_min = o._z_min;
	_z_max = o._z_max;


	vertices.addIndexChangedCallback(std::bind(&Mesh::vtxIndexChangedCallback, this, std::placeholders::_1, std::placeholders::_2));
	faces.addIndexChangedCallback(std::bind(&Mesh::faceIndexChangedCallback, this, std::placeholders::_1, std::placeholders::_2));
	halfEdges.addIndexChangedCallback(std::bind(&Mesh::hEdgeIndexChangedCallback, this, std::placeholders::_1, std::placeholders::_2));

	//update iterators to point to new containers
	for (auto& face : faces)
	{
		face.edge = getEquivalentItr(halfEdges, o.halfEdges, face.edge);
	}
	for (auto& hEdge : halfEdges)
	{
		//next
		hEdge.next = getEquivalentItr(halfEdges, o.halfEdges, hEdge.next);

		//prev
		hEdge.prev = getEquivalentItr(halfEdges, o.halfEdges, hEdge.prev);
		//from
		hEdge.from = getEquivalentItr(vertices, o.vertices, hEdge.from);
		//to
		hEdge.to = getEquivalentItr(vertices, o.vertices, hEdge.to);
		//owningFace
		hEdge.owningFace = getEquivalentItr(faces, o.faces, hEdge.owningFace);
	}
	for (auto& vtx : vertices)
	{
		for (auto& hEdge : vtx.leavingEdges)
		{
			hEdge = getEquivalentItr(halfEdges, o.halfEdges, hEdge);
		}
		for (auto& hEdge : vtx.arrivingEdges)
		{
			hEdge = getEquivalentItr(halfEdges, o.halfEdges, hEdge);
		}
	}
	for (auto& hashItr : _verticesHash)
	{
		hashItr.second = getEquivalentItr(vertices, o.vertices, hashItr.second);
	}

}
Mesh& Mesh::operator+=(const Mesh& o)
{
    for(auto& each: o.getFaces())
    {
        auto vertices = each.meshVertices();
        addFace(vertices[0]->position, vertices[1]->position, vertices[2]->position);
    }
    return *this;
}
//Mesh& Mesh::operator=(const Mesh o)
//{
//}

/********************** Mesh Edit Functions***********************/



void Mesh::vertexOffset(float factor){
	vertices.markChangedAll();
    int numberofVertices = vertices.size();
    _x_min = 99999;
    _x_max = 99999;
    _y_min = 99999;
    _y_max = 99999;
    _z_min = 99999;
    _z_max = 99999;
	size_t count = 0;
	for(auto& vertex: vertices)
	{
		if (count % 100 == 0)
			QCoreApplication::processEvents();
		QVector3D tmp = vertex.position - vertex.vn * factor;
		vertex.position = tmp;
		this->updateMinMax(vertex.position);
		++count;
	};
}

void Mesh::vertexMove(QVector3D direction){
	vertices.markChangedAll();
    int numberofVertices = vertices.size();
    _x_min = 99999;
    _x_max = 99999;
    _y_min = 99999;
    _y_max = 99999;
    _z_min = 99999;
    _z_max = 99999;
	size_t count = 0;
	for (auto& vertex : vertices)
	{
		if (count % 100 == 0)
			QCoreApplication::processEvents();
		QVector3D tmp = direction + vertex.position;
		vertex.position = tmp;
		updateMinMax(vertex.position);
		++count;
	};
}

Mesh* Mesh::vertexMoved(QVector3D direction)const
{
    Mesh* coppied = new Mesh(*this);
    coppied->vertexMove(direction);
    return coppied;
}

void Mesh::centerMesh(){
    float x_center = (_x_max+_x_min)/2;
    float y_center = (_y_max+_y_min)/2;
    float z_center = (_z_max+_z_min)/2;
    vertexMove(-QVector3D(x_center, y_center, z_center));
    _x_max = _x_max - x_center;
    _x_min = _x_min - x_center;
    _y_max = _y_max - y_center;
    _y_min = _y_min - y_center;
    _z_max = _z_max - z_center;
    _z_min = _z_min - z_center;
}

void Mesh::vertexRotate(QMatrix4x4 tmpmatrix){
	vertices.markChangedAll();
	faces.markChangedAll();

    _x_min = 99999;
    _x_max = 99999;
    _y_min = 99999;
    _y_max = 99999;
    _z_min = 99999;
    _z_max = 99999;

	for (auto& vertex : vertices)
	{
		QVector4D tmpVertex;
		QVector3D tmpVertex2;
		tmpVertex = vertex.position.toVector4D();
		tmpVertex2.setX(QVector4D::dotProduct(tmpVertex, tmpmatrix.column(0)));
		tmpVertex2.setY(QVector4D::dotProduct(tmpVertex, tmpmatrix.column(1)));
		tmpVertex2.setZ(QVector4D::dotProduct(tmpVertex, tmpmatrix.column(2)));
		vertex.position = tmpVertex2;
		updateMinMax(vertex.position);
	};

	for (auto& face : faces)
	{
		auto meshVertices = face.meshVertices();
		face.fn = QVector3D::normal(meshVertices[0]->position,
		meshVertices[1]->position,
		meshVertices[2]->position);
	};

    for (auto& vertex : vertices)
	{
		vertex.calculateNormalFromFaces();
    };



}

void Mesh::vertexScale(float scaleX=1, float scaleY=1, float scaleZ=1, float centerX=0, float centerY=0){
	vertices.markChangedAll();
    if(fabs(scaleX) < FZERO || fabs(scaleY) < FZERO || fabs(scaleZ) < FZERO) {
        qmlManager->openResultPopUp("","Scale cannot be 0", "");
        return;
    }

    int numberofVertices = vertices.size();
    _x_min = 99999;
    _x_max = 99999;
    _y_min = 99999;
    _y_max = 99999;
    _z_min = 99999;
    _z_max = 99999;

    /* need to fix center of the model */
    float fixCenterX = centerX - (centerX*scaleX);
    float fixCenterY = centerY - (centerY*scaleY);

	size_t count = 0;
	for (auto& vertex : vertices)
	{
		if (count % 100 == 0)
			QCoreApplication::processEvents();
		QVector3D tmp;
		tmp.setX(vertex.position.x() * scaleX + fixCenterX);
		tmp.setY(vertex.position.y() * scaleY + fixCenterY);
		tmp.setZ(vertex.position.z() * scaleZ);
		vertex.position = tmp;
		updateMinMax(vertex.position);
		++count;
	}
}


void Mesh::reverseFace(FaceConstItr faceItr)
{
	auto edgeCirc = faceItr->edgeCirculator();
	HalfEdge* hEdge;
	for (size_t i = 0; i < 3; ++i)
	{
		hEdge = &*halfEdges.toNormItr(edgeCirc.toItr());
		auto tmpEdg = hEdge->next;
		hEdge->next = hEdge->prev;
		hEdge->prev = tmpEdg;

		auto oldFrom = hEdge->from;
		hEdge->from = hEdge->to;
		hEdge->to = oldFrom;
	}
}


void Mesh::reverseFaces(){

    halfEdges.markChangedAll();
    vertices.markChangedAll();

	size_t count = 0;
	for (auto hEdgeItr = halfEdges.begin(); hEdgeItr != halfEdges.end(); ++hEdgeItr)
	{
		HalfEdge& hEdge = *hEdgeItr;
		auto tmpEdg = hEdge.next;
		hEdge.next = hEdge.prev;
		hEdge.prev = tmpEdg;

		auto oldFrom = hEdge.from;
		hEdge.from = hEdge.to;
		hEdge.to = oldFrom;
	}

    for (auto vertexItr = vertices.begin(); vertexItr != vertices.end(); ++vertexItr){
        vertexItr->vn = -vertexItr->vn;
    }
}


/********************** Mesh Generation Functions **********************/

bool Mesh::addFace(QVector3D v0, QVector3D v1, QVector3D v2){
	std::array<VertexItr, 3> fVtx;
    fVtx[0] = addOrRetrieveFaceVertex(v0);
	fVtx[1] = addOrRetrieveFaceVertex(v1);
	fVtx[2] = addOrRetrieveFaceVertex(v2);
	//if the face is too small and slicing option collapsed a pair of its vertices, don't add.
	if (fVtx[0] == fVtx[1] || fVtx[0] == fVtx[2] || fVtx[1] == fVtx[2])
		return false;
    Hix::Engine3D::MeshFace mf;
	mf.fn = QVector3D::normal(fVtx[0]->position, fVtx[1]->position, fVtx[2]->position);
	faces.emplace_back(mf);
	auto faceItr = faces.cend() - 1;

	addHalfEdgesToFace(fVtx, faceItr);

	fVtx[0]->calculateNormalFromFaces();
	fVtx[1]->calculateNormalFromFaces();
	fVtx[2]->calculateNormalFromFaces();
	return true;
}


TrackedIndexedList<MeshFace>::const_iterator Mesh::removeFace(FaceConstItr faceItr){
	//for each half edge, remove vertex relations
	auto edgeCirculator = faceItr->edgeCirculator();
	DeleteGuard<MeshVertex, std::allocator<MeshVertex>> vtxDelGuard = vertices.getDeleteGuard();
	DeleteGuard<HalfEdge, std::allocator<HalfEdge>> hEdgeDelGuard = halfEdges.getDeleteGuard();

	for (size_t i = 0; i < 3; ++i)
	{
		auto from = edgeCirculator->from;
		auto to = edgeCirculator->to;

		auto moddableLeavingVtx = vertices.toNormItr(from);
		auto moddableArrivingVtx = vertices.toNormItr(to);

		auto& leavingVtx = *moddableLeavingVtx;
		auto& arrivingVtx = *moddableArrivingVtx;

		auto delVectorItr = std::find(leavingVtx.leavingEdges.begin(), leavingVtx.leavingEdges.end(), edgeCirculator.toItr());
		leavingVtx.leavingEdges.erase(delVectorItr);
		delVectorItr = std::find(leavingVtx.arrivingEdges.begin(), leavingVtx.arrivingEdges.end(), edgeCirculator.toItr());
		arrivingVtx.arrivingEdges.erase(delVectorItr);

		//if the vertex is empty ie) there are no half edges connectint to it, delete the vertex
		if (leavingVtx.empty())
		{
			vtxDelGuard.deleteLater(moddableLeavingVtx);
			removeVertexHash(leavingVtx.position);
		}
		if (arrivingVtx.empty())
		{
			vtxDelGuard.deleteLater(moddableArrivingVtx);
			removeVertexHash(arrivingVtx.position);
		}
		++edgeCirculator;
	}
	//remove all half edges belonging to the mesh
	for (size_t i = 0; i < 3; ++i)
	{
		hEdgeDelGuard.deleteLater(edgeCirculator.toItr());
		++edgeCirculator;
	}
	return faces.swapAndErase(faceItr);
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
			auto edgeCirc = curr->edgeCirculator();
			for (size_t i = 0; i < 3; ++i)
			{
				auto nFaces = edgeCirc->twinFaces();
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
			auto edgeCirc = curr->edgeCirculator();
			for (size_t i = 0; i < 3; ++i)
			{
				auto nFaces = edgeCirc->twinFaces();
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
	auto toBeRemoved = _verticesHash.find(pos);
	if (toBeRemoved != _verticesHash.end())
	{
		_verticesHash.erase(toBeRemoved);
	}
}

VertexItr Mesh::addOrRetrieveFaceVertex(QVector3D v){
	//find if existing vtx can be used
	auto existingVtx = _verticesHash.find(v);

	if (existingVtx != _verticesHash.end())
	{
		return vertices.toNormItr(existingVtx->second);
    }
    MeshVertex mv(v);
    vertices.emplace_back(mv);
	auto last = vertices.cend() - 1;
	_verticesHash[v] = last;
    updateMinMax(v);
    return  vertices.toNormItr(last);
}

void Hix::Engine3D::Mesh::addHalfEdgesToFace(std::array<VertexItr, 3> faceVertices, FaceConstItr face)
{
	//a mesh face aggregates half edges(3 to 1) so there is no need to hash half edges when adding them with new face
	halfEdges.emplace_back(HalfEdge());
	halfEdges.emplace_back(HalfEdge());
	halfEdges.emplace_back(HalfEdge());

	auto firstAddedItr = halfEdges.end() - 3;
	size_t vtxIdx = 0;
	
	HalfEdgeConstItr nextItr;
	HalfEdgeConstItr prevItr;

	for (auto itr = firstAddedItr; itr != halfEdges.end(); ++itr)
	{
		//add vertices in (from, to) pairs, so (0,1) (1,2) (2,0)
		itr->from = VertexConstItr(faceVertices[vtxIdx % 3]);
		itr->to = VertexConstItr(faceVertices[(vtxIdx + 1) % 3]);

		//add "owning" face or face that the hEdge circuit creates
		itr->owningFace = face;
		//for each vertices that the half edges are "leaving" from, add the half edge reference
		faceVertices[vtxIdx % 3]->leavingEdges.push_back(itr);
		faceVertices[(vtxIdx + 1) % 3]->arrivingEdges.push_back(itr);

		//add circular relationship for all half edges
		nextItr = itr + 1;
		prevItr = itr;
		//since we can't use % on itrs
		if (nextItr == halfEdges.cend())
		{
			nextItr = firstAddedItr;
		}
		if (prevItr == firstAddedItr)
		{
			prevItr = halfEdges.cend() - 1;
		}
		else
		{
			--prevItr;
		}
		itr->next = nextItr;
		itr->prev = prevItr;
		++vtxIdx;

	}
	this->toNormItr(face)->edge = HalfEdgeConstItr(firstAddedItr);
}



std::array<float,6> Mesh::calculateMinMax(QMatrix4x4 rotmatrix, const Mesh* mesh) {
	qDebug() << "calculate minmax";
	std::array<float, 6> minmax{ 99999 };
	size_t count = 0;
	const auto &vertices = mesh->getVertices();
	for (const auto& vertex : vertices)
	{
		QVector4D tmpVertex;
		QVector3D tmpVertex2;
		if (count % 100 == 0)
			QCoreApplication::processEvents();
		tmpVertex = vertex.position.toVector4D();
		QVector3D v = QVector3D(QVector4D::dotProduct(tmpVertex, rotmatrix.column(0)), QVector4D::dotProduct(tmpVertex, rotmatrix.column(1)), QVector4D::dotProduct(tmpVertex, rotmatrix.column(2)));
		updateMinMax(v, minmax);
	}
    return minmax;
}

void Mesh::updateMinMax(QVector3D v)
{
    if (v.x() > _x_max || _x_max == 99999)
        _x_max = v.x();
    if (v.x() < _x_min || _x_min == 99999)
        _x_min = v.x();
    if (v.y() > _y_max || _y_max == 99999)
        _y_max = v.y();
    if (v.y() < _y_min || _y_min == 99999)
        _y_min = v.y();
    if (v.z() > _z_max || _z_max == 99999)
        _z_max = v.z();
    if (v.z() < _z_min || _z_min == 99999)
        _z_min = v.z();
}


void Mesh::vtxIndexChangedCallback(size_t oldIdx, size_t newIdx)
{
	//vertex whose index has been changed
	auto& vtx = vertices[newIdx];
	//new position for the shifted vtx;
	auto newIndexItr = vertices.cbegin() + newIdx;
	//update leaving edges
	for (auto leavingEdge : vtx.leavingEdges)
	{
		auto modLeavingEdge = halfEdges.toNormItr(leavingEdge);
		modLeavingEdge->from = newIndexItr;
		//get twin edges with opposite direction ie) arriving 
	}
	//update arrivingEdges
	for (auto arrivingEdge : vtx.arrivingEdges)
	{
		auto modArrEdge = halfEdges.toNormItr(arrivingEdge);
		modArrEdge->to = newIndexItr;
	}

	//update hash value
	auto oldItr = vertices.cbegin() + oldIdx;
	auto hashItr = _verticesHash.find(vtx.position);
	while (hashItr != _verticesHash.end()) {
		if (hashItr->second == oldItr)
		{
			_verticesHash[vtx.position] = newIndexItr;
			break;
		}
		++hashItr;
	}
}

void Mesh::faceIndexChangedCallback(size_t oldIdx, size_t newIdx)
{
	//face whose index has been changed
	auto& face = faces[newIdx];
	//new position for the shifted face;
	auto newIndexItr = faces.cbegin() + newIdx;
	//for each half edges "owned" by this face, update the iterator
	auto circ = face.edgeCirculator();
	for (size_t i = 0; i < 3; ++i)
	{
		auto modHEdge = halfEdges.toNormItr(circ.toItr());
		modHEdge->owningFace = newIndexItr;
	}
}

void Mesh::hEdgeIndexChangedCallback(size_t oldIdx, size_t newIdx)
{
	//halfEdge whose index has been changed
	auto& hEdge = halfEdges[newIdx];
	//new position for the shifted halfEdge;
	auto newIndexItr = halfEdges.cbegin() + newIdx;
	//update face that owns this half edge only if the itr owned by the face is this one
	if (hEdge.owningFace->edge != hEdge.next && hEdge.owningFace->edge != hEdge.prev)
	{
		auto modFace = faces.toNormItr(hEdge.owningFace);
		modFace->edge = newIndexItr;
	}
	//update vertices that have reference to this edge
	auto oldItr = halfEdges.cbegin() + oldIdx;
	auto modFrom = &*vertices.toNormItr(hEdge.from);
	auto foundItr = std::find(modFrom->leavingEdges.begin(), modFrom->leavingEdges.end(), oldItr);
	(*foundItr) = newIndexItr;
	auto modTo = &*vertices.toNormItr(hEdge.to);
	foundItr = std::find(modTo->arrivingEdges.begin(), modTo->arrivingEdges.end(), oldItr);
	(*foundItr) = newIndexItr;
}


void Mesh::updateMinMax(QVector3D v, std::array<float,6>& minMax){
//array order: _x_min, _x_max...._z_min, _z_max
    if (v.x() < minMax[0] || minMax[0] == 99999)
        minMax[0] = v.x();
    if (v.x() > minMax[1] || minMax[1] == 99999)
        minMax[1] = v.x();
    if (v.y() < minMax[2] || minMax[2] == 99999)
        minMax[2] = v.y();
    if (v.y() > minMax[3] || minMax[3] == 99999)
        minMax[3] = v.y();
    if (v.z() < minMax[4] || minMax[4] == 99999)
        minMax[4] = v.z();
    if (v.z() > minMax[5] || minMax[5] == 99999)
        minMax[5] = v.z();
}

//ascending/increasing order
std::array<float, 3> MeshFace::sortZ()const
{
	std::array<float, 3> orderedZ;
	auto mfVertices = meshVertices();
	for (int i = 0; i < mfVertices.size(); i++) {
		orderedZ[i] = mfVertices[i]->position.z();
	}
	std::sort(orderedZ.begin(), orderedZ.end());
	return orderedZ;
}


float MeshFace::getFaceZmin()const{
    float face__z_min=std::numeric_limits<float>::max();
	auto mfVertices = meshVertices();
    for (int i=0; i< mfVertices.size(); i++){
        float temp__z_min = mfVertices[i]->position.z();
        if (temp__z_min<face__z_min)
            face__z_min = temp__z_min;
    }
    return face__z_min;
}

float MeshFace::getFaceZmax()const{
    float face__z_max= std::numeric_limits<float>::lowest();
	auto mfVertices = meshVertices();
    for (int i=0; i< mfVertices.size(); i++){
        float temp__z_max = mfVertices[i]->position.z();
        if (temp__z_max>face__z_max)
            face__z_max = temp__z_max;
    }
    return face__z_max;
}

bool MeshFace::getEdgeWithVertices(HalfEdgeConstItr& result, const VertexConstItr& a, const VertexConstItr& b)const
{
	auto edgeCirc = edgeCirculator();
	for (size_t i = 0; i < 3; ++i)
	{
		auto edge = edgeCirc.toItr();
		if ((edge->to == a && edge->from == b) || (edge->to == b && edge->from == a))
		{
			result = edge;
			return true;
		}
		++edgeCirc;
	}
	return false;
}

bool MeshFace::isNeighborOf(const FaceConstItr& other)const
{
	auto edgeCirc = edgeCirculator();
	for (size_t i = 0; i < 3; ++i)
	{
		auto nFaces = edgeCirc->twinFaces();
		for (auto& each : nFaces)
		{
			if (each == other)
			{
				return true;
			}
		}
	}
	return false;
}

void Mesh::findNearSimilarFaces(QVector3D normal, FaceConstItr  mf,
	std::unordered_set<FaceConstItr>& result, float maxNormalDiff)const
{
	std::deque<FaceConstItr>q;
	q.emplace_back(mf);
	result.insert(mf);
	bool boundDetected = false;
	while (!q.empty())
	{
		auto curr = q.front();
		q.pop_front();
		result.insert(curr);
		auto edgeCirc = curr->edgeCirculator();
		for (size_t i = 0; i < 3; ++i, ++edgeCirc) {
			auto nFaces = edgeCirc->twinFaces();
			for (auto nFace : nFaces)
			{
				if (!boundDetected && result.find(nFace) == result.end())
				{
					if ((nFace->fn - normal).length() < maxNormalDiff)
					{
						q.emplace_back(nFace);
					}
					else
					{
						boundDetected = true;

					}
				}
			}
		}
	}
	return;
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
    for (MeshVertex cmv : contour){
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

MeshVertex* findAvailableMeshVertexFromContour(QHash<uint32_t, Path3D>* pathHash, std::vector<uint32_t>* hashList, Path3D* contour){
    for (auto& mv : *contour){
        if ((*pathHash)[Vertex2Hash(mv)].size()>=3){
            return &(*pathHash)[Vertex2Hash(mv)].at(1);
        }
    }
    return nullptr;
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
                if ((hole_edge1_it->end()-1)->position.distanceToPoint(hole_edge2_it->begin()->position) < VTX_INBOUND_DIST *0.05/ClipperLib::INT_PT_RESOLUTION){
                //if (Vertex2Hash(*(hole_edge1_it->end()-1)) == Vertex2Hash(*hole_edge2_it->begin())){
                    //qDebug() << "erase";
                    dirty = true;
                    hole_edge1_it->insert(hole_edge1_it->end(), hole_edge2_it->begin()+1, hole_edge2_it->end());
                    checked_its.push_back(hole_edge2_it);
                    //hole_edge2_it = hole_edges.erase(hole_edge2_it);
                    //qDebug() << "erased";
                } else if ((hole_edge1_it->end()-1)->position.distanceToPoint((hole_edge2_it->end()-1)->position) < VTX_INBOUND_DIST *0.05/ClipperLib::INT_PT_RESOLUTION){
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
            qDebug() << "result_edge : " << hole_edge_it->begin()->position << (hole_edge_it->end()-1)->position;
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



const TrackedIndexedList<MeshVertex>& Mesh::getVertices()const
{
	return vertices;
}
const TrackedIndexedList<MeshFace>& Mesh::getFaces()const
{
	return faces;
}
const TrackedIndexedList<HalfEdge>& Mesh::getHalfEdges()const
{
	return halfEdges;
}

TrackedIndexedList<MeshVertex>& Mesh::getVerticesNonConst()
{
	return vertices;
}
TrackedIndexedList<MeshFace>& Mesh::getFacesNonConst()
{
	return faces;
}
TrackedIndexedList<HalfEdge>& Mesh::getHalfEdgesNonConst()
{
	return halfEdges;
}


float Mesh::x_min()const
{
    return _x_min;
}
float Mesh::x_max()const
{
    return _x_max;

}
float Mesh::y_min()const
{
    return _y_min;

}
float Mesh::y_max()const
{
    return _y_max;

}
float Mesh::z_min()const
{
    return _z_min;
}
float Mesh::z_max()const
{
    return _z_max;

}


/************** Helper Functions *****************/

uint32_t  Hix::Engine3D::intPoint2Hash(IntPoint u) {
	QVector3D u_qv3 = QVector3D(u.X, u.Y, 0);
	uint32_t path_hash_u = std::hash<QVector3D>()(u_qv3);
	return path_hash_u;
}

uint32_t Hix::Engine3D::Vertex2Hash(MeshVertex& u)
{
	uint32_t path_hash_u = std::hash<QVector3D>()(u.position);
	return path_hash_u;
}

