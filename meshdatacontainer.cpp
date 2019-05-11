#include "meshdatacontainer.h"
#include "DentEngine/src/mesh.h"

MeshDataType::MeshDataType(MeshDataContainer* owner) :_owner(owner)
{
}
MeshDataContainer::MeshDataContainer()
{

}


/////////////MeshFace/////////////
MeshFace::MeshFace(MeshDataContainer* owner) :MeshDataType(owner), _neighboring_faces{ std::vector<const MeshFace*>(), std::vector<const MeshFace*>(), std::vector<const MeshFace*>() }
{}
MeshFace::MeshFace() : MeshFace(nullptr) {}



size_t MeshFace::idx() const
{
	return _idx;
}

QVector3D MeshFace::fn() const
{
	return _fn;
}

const std::array<std::vector<const MeshFace*>, 3> MeshFace::neighboring_faces() const
{
	return _neighboring_faces;
}

const std::array<const MeshVertex*, 3> MeshFace::mesh_vertex()const
{
	return _mesh_vertex;
}


void MeshFace::setFn(QVector3D normal)
{
	_fn = normal;
	markModified();
}

void MeshFace::markModified()
{
	if (_owner)
	{
		_owner->markFaceModified(this);
	}
}

/////////////MeshVertex/////////////

size_t MeshVertex::idx() const
{
	return _idx;
}

QVector3D MeshVertex::position() const
{
	return _position;
}

QVector3D MeshVertex::vn() const
{
	return _vn;
}

void MeshVertex::calculateNormalFromFaces()
{
	_vn = { 0,0,0 };
	for (auto& face : _connected_faces)
	{
		_vn += face->fn();
	}
	_vn.normalize();
	markModified();
}

void MeshVertex::setPosition(QVector3D position)
{
	_position = position;
	markModified();
}

MeshVertex::MeshVertex() :MeshDataType(nullptr)
{
}

MeshVertex::MeshVertex(MeshDataContainer* container, QVector3D position) : MeshDataType(container), _position(position)
{
}

MeshVertex::MeshVertex(MeshDataContainer* container) : MeshDataType(container)
{
}

void MeshVertex::markModified()
{
	if (_owner)
	{
		_owner->markVertexModified(this);
	}
}
