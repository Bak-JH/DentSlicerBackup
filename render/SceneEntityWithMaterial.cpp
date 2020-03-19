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

using namespace Qt3DCore;
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
	float* rawVertexArray = reinterpret_cast<float*>(appendData.data());
	size_t idx = 0;

	for (auto itr = begin; itr != end; ++itr)
	{
		auto faceVertices = itr.meshVertices();
		auto colorCode = getPrimitiveColorCode(mesh, itr);

		for (auto& vtxItr : faceVertices)
		{
			auto pos = vtxItr.localPosition();
			rawVertexArray[idx++] = pos[0];
			rawVertexArray[idx++] = pos[1];
			rawVertexArray[idx++] = pos[2];
			rawVertexArray[idx++] = colorCode[0];
			rawVertexArray[idx++] = colorCode[1];
			rawVertexArray[idx++] = colorCode[2];
			rawVertexArray[idx++] = colorCode[3];
		}
	}
	//update data/count
	QByteArray totalData = vertexBuffer.data() + appendData;

	vertexBuffer.setData(totalData);
	size_t currCount = (oldFaceCount + appendFaceCount) * 3;
	positionAttribute.setCount(currCount);
	colorAttribute.setCount(currCount);

}

void SceneEntityWithMaterial::setMaterialMode(const Hix::Render::ShaderMode mode)
{
	_meshMaterial.changeMode(mode);
	callRecursive(this, &SceneEntityWithMaterial::setMaterialMode, mode);
}

void SceneEntityWithMaterial::setMaterialColor(const QVector4D color)
{
	_meshMaterial.setColor(color);
	callRecursive(this, &SceneEntityWithMaterial::setMaterialColor, color);
}



void Hix::Render::SceneEntityWithMaterial::setMaterialParamter(const std::string& key, const QVariant& value)
{
	_meshMaterial.setParameterValue(key, value);
	callRecursive(this, &SceneEntityWithMaterial::setMaterialParamter, key, value);
}
