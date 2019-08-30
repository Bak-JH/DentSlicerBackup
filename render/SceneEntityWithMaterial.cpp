#include "SceneEntityWithMaterial.h"
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



#define ATTRIBUTE_SIZE_INCREMENT 200


using namespace Utils::Math;
using namespace Hix::Engine3D;
using namespace Hix::Render;

SceneEntityWithMaterial::SceneEntityWithMaterial(QEntity* parent)
	: SceneEntity(parent)
	, _meshMaterial()
{
	addComponent(&_meshMaterial);

}






SceneEntityWithMaterial::~SceneEntityWithMaterial()
{
}

inline void eraseBufferData(QAttribute& attr, Qt3DRender::QBuffer& buffer, size_t amount, size_t count)
{
	QByteArray copy = buffer.data();
	copy.remove(copy.size() - amount, amount);
	buffer.setData(copy);
	attr.setCount(attr.count() - count);
	return;
}
void inline attrBufferResize(QAttribute& attr, Qt3DRender::QBuffer& attrBuffer, size_t dataUnitSize, size_t appendByteSize) {

	size_t currentUsedSize = attr.count() * dataUnitSize;
	size_t currentAllocSize = attrBuffer.data().size();
	if ((currentUsedSize + appendByteSize) > currentAllocSize) {
		size_t countPowerOf2 = 1;
		while (countPowerOf2 < currentUsedSize + appendByteSize) {
			countPowerOf2 <<= 1;
		}
		auto copy = attrBuffer.data();
		copy.resize(countPowerOf2);
		attrBuffer.setData(copy);
	}
}


void SceneEntityWithMaterial::setMesh(Mesh* mesh)
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


void SceneEntityWithMaterial::appendMeshVertex(const Mesh* mesh,
	Hix::Engine3D::FaceConstItr begin, Hix::Engine3D::FaceConstItr end)
{

	if (_meshMaterial.shaderMode() == ShaderMode::PerPrimitiveColor)
	{
		appendMeshVertexPerPrimitive(mesh, begin, end);
	}
	else
	{
		appendMeshVertexSingleColor(mesh, begin, end);
	}

}

void SceneEntityWithMaterial::appendMeshVertexSingleColor(const Mesh* mesh,
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

void SceneEntityWithMaterial::appendMeshVertexPerPrimitive(const Mesh* mesh,
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


	for (auto itr = begin; itr != end; ++itr)
	{
		auto faceVertices = itr->meshVertices();
		auto colorCode = getPrimitiveColorCode(mesh, itr);

		for (auto& vtxItr : faceVertices)
		{
			vertices << vtxItr->position << vtxItr->vn << colorCode;
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

void SceneEntityWithMaterial::appendIndexArray(const Mesh* mesh, Hix::Engine3D::FaceConstItr begin, Hix::Engine3D::FaceConstItr end)
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



void SceneEntityWithMaterial::updateMesh(Mesh* mesh, bool force)
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
		setMesh(mesh);
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




void SceneEntityWithMaterial::updateFaces(const std::unordered_set<size_t>& faceIndicies, const Hix::Engine3D::Mesh& mesh)
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


void SceneEntityWithMaterial::updateVertices(const std::unordered_set<size_t>& vtxIndicies, const Hix::Engine3D::Mesh& mesh)
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

