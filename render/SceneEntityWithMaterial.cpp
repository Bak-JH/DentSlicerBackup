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




void SceneEntityWithMaterial::appendMeshVertex(const Mesh* mesh,
	Hix::Engine3D::FaceConstItr begin, Hix::Engine3D::FaceConstItr end)
{

	if (_meshMaterial.shaderMode() == ShaderMode::PerPrimitiveColor)
	{
		appendMeshVertexPerPrimitive(mesh, begin, end);
	}
	else
	{
		SceneEntity::appendMeshVertex(mesh, begin, end);
	}

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
		auto faceVertices = itr.meshVertices();
		auto colorCode = getPrimitiveColorCode(mesh, itr);

		for (auto& vtxItr : faceVertices)
		{
			vertices << vtxItr.position() << vtxItr.vn() << colorCode;
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

