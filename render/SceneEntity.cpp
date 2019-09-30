#include "SceneEntity.h"
#include <QRenderSettings>
#include <QString>
#include <QtMath>
#include <cfloat>
#include <exception>
#include <QtConcurrent/QtConcurrent>
#include <QFuture>
#include <QFileDialog>
#include <iostream>
#include <QDir>
#include <QMatrix3x3>
#include <Qt3DCore/qpropertyupdatedchange.h>
#include "utils/utils.h"
#include "qmlmanager.h"


#define ATTRIBUTE_SIZE_INCREMENT 200


using namespace Utils::Math;
using namespace Hix::Engine3D;
using namespace Hix::Render;

SceneEntity::SceneEntity(QEntity* parent)
	: QEntity(parent)
	, m_geometryRenderer(this)
	, m_geometry(this)
	, vertexBuffer(Qt3DRender::QBuffer::VertexBuffer, this)
	, indexBuffer(Qt3DRender::QBuffer::IndexBuffer, this)
	, positionAttribute(this)
	, normalAttribute(this)
	, colorAttribute(this)
	, indexAttribute(this)


{
	addComponent(&_transform);
	//initialize vertex buffers etc
	vertexBuffer.setUsage(Qt3DRender::QBuffer::DynamicDraw);
	indexBuffer.setUsage(Qt3DRender::QBuffer::DynamicDraw);
	vertexBuffer.setAccessType(Qt3DRender::QBuffer::AccessType::ReadWrite);
	indexBuffer.setAccessType(Qt3DRender::QBuffer::AccessType::ReadWrite);


	positionAttribute.setAttributeType(QAttribute::VertexAttribute);
	positionAttribute.setBuffer(&vertexBuffer);
	positionAttribute.setDataType(QAttribute::Float);
	positionAttribute.setDataSize(POS_SIZE);
	positionAttribute.setByteOffset(0);
	positionAttribute.setByteStride(VTX_SIZE);
	positionAttribute.setCount(0);
	positionAttribute.setName(QAttribute::defaultPositionAttributeName());

	normalAttribute.setAttributeType(QAttribute::VertexAttribute);
	normalAttribute.setBuffer(&vertexBuffer);
	normalAttribute.setDataType(QAttribute::Float);
	normalAttribute.setDataSize(NRM_SIZE);
	normalAttribute.setByteOffset(POS_SIZE * sizeof(float));
	normalAttribute.setByteStride(VTX_SIZE);
	normalAttribute.setCount(0);
	normalAttribute.setName(QAttribute::defaultNormalAttributeName());

	colorAttribute.setAttributeType(QAttribute::VertexAttribute);
	colorAttribute.setBuffer(&vertexBuffer);
	colorAttribute.setDataType(QAttribute::Float);
	colorAttribute.setDataSize(COL_SIZE);
	colorAttribute.setByteOffset((POS_SIZE + NRM_SIZE) * sizeof(float));
	colorAttribute.setByteStride(VTX_SIZE);
	colorAttribute.setCount(0);
	colorAttribute.setName(QAttribute::defaultColorAttributeName());


	indexAttribute.setVertexBaseType(QAttribute::VertexBaseType::UnsignedInt);
	indexAttribute.setAttributeType(QAttribute::IndexAttribute);
	indexAttribute.setBuffer(&indexBuffer);
	indexAttribute.setDataType(QAttribute::UnsignedInt);
	indexAttribute.setDataSize(1);
	indexAttribute.setByteOffset(0);
	indexAttribute.setByteStride(0);
	indexAttribute.setCount(0);
	indexAttribute.setVertexSize(1);


	m_geometryRenderer.setInstanceCount(1);
	m_geometryRenderer.setFirstVertex(0);
	m_geometryRenderer.setFirstInstance(0);
	m_geometryRenderer.setPrimitiveType(QGeometryRenderer::Triangles);
	m_geometryRenderer.setGeometry(&m_geometry);
	m_geometryRenderer.setVertexCount(0);

	m_geometry.addAttribute(&positionAttribute);
	m_geometry.addAttribute(&normalAttribute);
	m_geometry.addAttribute(&colorAttribute);

	m_geometry.addAttribute(&indexAttribute);

	clearMem();


}




void SceneEntity::clearMem() {
	QByteArray newVertexArray;
	QByteArray newIdxArray;
	QByteArray newPrimitiveColorArray;

	vertexBuffer.setData(newVertexArray);
	indexBuffer.setData(newIdxArray);

	positionAttribute.setCount(0);
	normalAttribute.setCount(0);
	colorAttribute.setCount(0);
	indexAttribute.setCount(0);

}


const Mesh* SceneEntity::getMesh()const
{
	return _mesh;
}

const Qt3DCore::QTransform& Hix::Render::SceneEntity::transform() const
{
	return _transform;
}

Qt3DCore::QTransform& Hix::Render::SceneEntity::transform()
{

	return _transform;

}

QVector3D Hix::Render::SceneEntity::toParentCoord(const VertexConstItr& vtx) const
{
	return toParentCoord(vtx->position);
}

QVector3D Hix::Render::SceneEntity::toParentCoord(const QVector3D& pos) const
{
	return pos * _transform.matrix();
}

QVector3D Hix::Render::SceneEntity::toRootCoord(const VertexConstItr& vtx) const
{
	auto coord = vtx->position;
	auto curr = this;
	while (curr)
	{
		coord = curr->toParentCoord(coord);
		curr = dynamic_cast<SceneEntity*>(curr->parentEntity());
	}
	return coord;
}

SceneEntity::~SceneEntity() 
{
	//mesh and SceneEntity lifetimes are decoupled
}

void Hix::Render::SceneEntity::setMesh(Mesh* newMesh)
{
	if (_mesh != newMesh && newMesh != nullptr)
	{
		_mesh = newMesh;
		updateEntireMesh(_mesh);
	}
}


void SceneEntity::updateEntireMesh(Mesh* mesh)
{
	//flush datas
	auto faceHistory = mesh->getFacesNonConst().flushChanges();
	auto verticesHistory = mesh->getVerticesNonConst().flushChanges();
	auto hEdgesHistory = mesh->getHalfEdgesNonConst().flushChanges();//not used...for now
	removeComponent(&m_geometryRenderer);
	auto& faces = mesh->getFaces();
	auto& vtxs = mesh->getVertices();
	//since renderer access data from background thread
	appendMeshVertex(mesh, faces.cbegin(), faces.cend());
	appendIndexArray(mesh, faces.cbegin(), faces.cend());
	addComponent(&m_geometryRenderer);

}


void SceneEntity::updateMesh(Mesh* mesh, bool force)
{
	//flush datas
	auto faceHistory = mesh->getFacesNonConst().flushChanges();
	auto verticesHistory = mesh->getVerticesNonConst().flushChanges();
	auto hEdgesHistory = mesh->getHalfEdgesNonConst().flushChanges();//not used...for now
	bool tooManyChanges = force;
	std::unordered_set<size_t> faceChangeSet;
	std::unordered_set<size_t> vtxChangeSet;
	std::unordered_set<size_t> hEdgesHistorySet;

	//check allChanged flag and skip to updateAll OR...
	//if the mesh being updated is not the same as the visible one, we need to redraw everything
	if (!tooManyChanges)
	{
		if (faceHistory.index() == 0 || verticesHistory.index() == 0 || hEdgesHistory.index() == 0)
		{
			tooManyChanges = true;
		}
		//skip if mesh being rendered is same and unmodified
		else if (faceHistory.index() == 1 && verticesHistory.index() == 1 && hEdgesHistory.index() == 1)
		{
			faceChangeSet = std::get<1>(faceHistory);
			vtxChangeSet = std::get<1>(verticesHistory);
			hEdgesHistorySet = std::get<1>(hEdgesHistory);
			if (faceChangeSet.size() + vtxChangeSet.size() + hEdgesHistorySet.size() == 0)
			{
				return;
			}
		}
	}
	if (!tooManyChanges)
	{
		if (faceHistory.index() == 1 && (std::get<1>(faceHistory).size() > mesh->getFaces().size() * 0.7))
		{
			tooManyChanges = true;
		}
		else if (verticesHistory.index() == 1 && (std::get<1>(verticesHistory).size() > mesh->getVertices().size() * 0.7))
		{
			tooManyChanges = true;
		}
		else if (hEdgesHistory.index() == 1 && (std::get<1>(hEdgesHistory).size() > mesh->getHalfEdges().size() * 0.7))
		{
			tooManyChanges = true;
		}
	}
	//partial buffer update doesn't seem to work now...
	tooManyChanges = true;
	if (tooManyChanges)
	{
		//if there are too many individual changes just reset the buffer
		clearMem();
		updateEntireMesh(mesh);
	}
	else
	{
		removeComponent(&m_geometryRenderer);
		updateVertices(vtxChangeSet, *mesh);
		updateFaces(faceChangeSet, *mesh);
		//if (_meshMaterial.shaderMode() == !ShaderMode::SingleColor)
		//{
		//	//m_meshMaterial.setColorCodes(_primitiveColorCodes);
		//}
		addComponent(&m_geometryRenderer);
	}
}


void SceneEntity::updateFaces(const std::unordered_set<size_t>& faceIndicies, const Hix::Engine3D::Mesh& mesh)
{
	//auto& faces = mesh.getFaces();
	//size_t oldFaceCount = indexAttribute.count()/3;
	//size_t newFaceCount = faces.size();
	//size_t smallerCount = std::min(oldFaceCount, newFaceCount);
	//size_t largerCount = std::max(oldFaceCount, newFaceCount);
	//size_t difference = largerCount - smallerCount;
	////update existing old values if they were changed
	//QByteArray updateArray;
	//updateArray.resize(FACE_SIZE);
	//uint* rawIndexArray = reinterpret_cast<uint*>(updateArray.data());
	//for (auto faceIdx : faceIndicies)
	//{
	//	if (faceIdx < smallerCount)
	//	{
	//		auto face = faces[faceIdx];
	//		size_t offset = faceIdx * FACE_SIZE;
	//		auto faceVertices = face.getVerticeIndices(&mesh);
	//		//for each indices
	//		for (size_t i = 0; i < 3; ++i)
	//		{
	//			rawIndexArray[i] = faceVertices[i];
	//		}
	//		indexBuffer.updateData(offset, updateArray);
	//		if (m_meshMaterial.shaderMode() == !ShaderMode::SingleColor)
	//		{
	//			//_primitiveColorCodes[faceIdx] = getPrimitiveColorCode(&mesh, faceIdx);
	//		}
	//	}
	//}
	//indexBuffer.setData(indexBuffer.data());

	////if new size is smaller, delete elements from back
	//if (newFaceCount < oldFaceCount)
	//{
	//	eraseBufferData(indexAttribute, indexBuffer, difference * FACE_SIZE, difference*3);
	//	//_primitiveColorCodes.erase(_primitiveColorCodes.end() - difference, _primitiveColorCodes.end());
	//	m_geometryRenderer.setVertexCount(indexAttribute.count());
	//}
	//else if (newFaceCount > oldFaceCount)
	//{
	//	appendMeshFace(&mesh, faces.cend() - difference, faces.cend(), 0);
	//}

	//indexAttribute.setCount(newFaceCount * 3);//3 indicies per face
	//m_geometryRenderer.setVertexCount(indexAttribute.count());

}


void SceneEntity::updateVertices(const std::unordered_set<size_t>& vtxIndicies, const Hix::Engine3D::Mesh& mesh)
{
	//auto& vtcs = mesh.getVertices();
	//size_t oldVtxCount = positionAttribute.count();
	//size_t newVtxCount = vtcs.size();
	//size_t smallerCount = std::min(oldVtxCount, newVtxCount);
	//size_t largerCount = std::max(oldVtxCount, newVtxCount);
	//size_t difference = largerCount - smallerCount;
	////update existing old values if they were changed
	//QByteArray updateArray;
	//updateArray.resize(VTX_SIZE);
	//float* rawVertexArray = reinterpret_cast<float*>(updateArray.data());

	////vertexBuffer.blockSignals(true);
	////vertexBuffer.blockNotifications(true);
	//for (auto vtxIdx : vtxIndicies)
	//{
	//	if (vtxIdx < smallerCount)
	//	{
	//		auto vtx = vtcs[vtxIdx];
	//		QVector<QVector3D> vtxData;
	//		vtxData << vtx.position  << vtx.vn;
	//		size_t idx = 0;
	//		for (const QVector3D& v : vtxData) {
	//			rawVertexArray[idx++] = v.x();
	//			rawVertexArray[idx++] = v.y();
	//			rawVertexArray[idx++] = v.z();
	//		}
	//		size_t offset = vtxIdx * VTX_SIZE;
	//		vertexBuffer.updateData(offset, updateArray);
	//	}
	//}
	////vertexBuffer.blockSignals(false);
	////vertexBuffer.blockNotifications(false);
	////vertexBuffer.setData(vertexBuffer.data());
	//emit vertexBuffer.dataChanged(vertexBuffer.data());
	////if new size is smaller, delete elements from back
	//if (newVtxCount < oldVtxCount)
	//{
	//	eraseBufferData(positionAttribute, vertexBuffer, difference * VTX_SIZE, difference);
	//	//set other attribute count as well
	//	normalAttribute.setCount(positionAttribute.count());
	//}
	//else if (newVtxCount > oldVtxCount)
	//{
	//	appendMeshVertex(&mesh, vtcs.cend() - difference, vtcs.cend());
	//}
	//positionAttribute.setCount(newVtxCount);
	//normalAttribute.setCount(positionAttribute.count());
}




void SceneEntity::appendIndexArray(const Mesh* mesh, Hix::Engine3D::FaceConstItr begin, Hix::Engine3D::FaceConstItr end)
{
	//we need to get maximum index of the current rendered mesh when we are appending to non-empty mesh.
	size_t startingVtxIndex = indexAttribute.count();
	size_t oldFaceCount = startingVtxIndex / 3;

	size_t appendFaceCount = end - begin;
	size_t appendFaceByteSize = appendFaceCount * FACE_SIZE;
	size_t appendVtxCount = appendFaceCount * IDX_SIZE;
	//resize attr buffer for new append
	//attrBufferResize(indexAttribute, indexBuffer, FACE_SIZE, appendByteSize);
	QByteArray indexBufferData;
	indexBufferData.resize(appendFaceByteSize);
	uint* rawIndexArray = reinterpret_cast<uint*>(indexBufferData.data());
	for (size_t i = 0; i < appendVtxCount; ++i)
	{
		rawIndexArray[i] = i + startingVtxIndex;
	}

	QByteArray totalData = indexBuffer.data() + indexBufferData;
	indexBuffer.setData(totalData);
	indexAttribute.setCount((oldFaceCount + appendFaceCount) * 3);//3 indicies per face
	m_geometryRenderer.setVertexCount(indexAttribute.count());

}


void SceneEntity::appendMeshVertex(const Mesh* mesh,
	Hix::Engine3D::FaceConstItr begin, Hix::Engine3D::FaceConstItr end)
{

	size_t oldFaceCount = indexAttribute.count() / 3;
	size_t appendFaceCount = end - begin;
	size_t appendFaceVerticesByteSize = appendFaceCount * IDX_SIZE * VTX_SIZE;
	//size_t appendVtxCount = appendFaceCount * IDX_SIZE;

	//data to be appended
	QByteArray appendData;
	appendData.resize(appendFaceVerticesByteSize);
	//add data to the append data
	QVector<QVector3D> vertices;
	QVector3D empty(0.0f, 0.0f, 0.0f);
	for (auto itr = begin; itr != end; ++itr)
	{
		auto faceVertices = itr->meshVertices();
		for (auto& vtxItr : faceVertices)
		{
			vertices << vtxItr->position << vtxItr->vn;
			//do color
			vertices << empty;
		}
	}

	float* rawVertexArray = reinterpret_cast<float*>(appendData.data());
	size_t idx = 0;
	for (const QVector3D& v : vertices) {
		rawVertexArray[idx++] = v.x();
		rawVertexArray[idx++] = v.y();
		rawVertexArray[idx++] = v.z();
	}
	//update data/count
	QByteArray totalData = vertexBuffer.data() + appendData;

	vertexBuffer.setData(totalData);
	size_t currCount = (oldFaceCount + appendFaceCount) * 3;
	positionAttribute.setCount(currCount);
	normalAttribute.setCount(currCount);
	colorAttribute.setCount(currCount);

}