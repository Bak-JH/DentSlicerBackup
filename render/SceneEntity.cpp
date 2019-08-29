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
	_layer.setRecursive(false);
	addComponent(&_layer);
	addComponent(&m_transform);


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

	_mesh = new Mesh();

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

SceneEntity::~SceneEntity() 
{
}



void SceneEntity::setHitTestable(bool isEnable)
{
	if (_hitEnabled != isEnable)
	{
		_hitEnabled = isEnable;
		if (_hitEnabled)
		{
			qmlManager->getRayCaster().addInputLayer(&_layer);
		}
		else
		{
			qmlManager->getRayCaster().removeInputLayer(&_layer);
		}
	}
}

bool SceneEntity::isHitTestable()
{
	return _hitEnabled;

}


const Qt3DCore::QTransform* SceneEntity::getTransform() const
{
	return &m_transform;
}

void SceneEntity::setTranslation(const QVector3D& t)
{
	m_transform.setTranslation(t);
}

QVector3D SceneEntity::getTranslation() {
	return m_transform.translation();
}


void SceneEntity::setMatrix(const QMatrix4x4& matrix)
{
	m_transform.setMatrix(matrix);
}
