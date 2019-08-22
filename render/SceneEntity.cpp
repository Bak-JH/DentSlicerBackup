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
	_layer.setRecursive(true);
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

	QObject::connect(this, SIGNAL(_updateModelMesh()), this, SLOT(updateModelMesh()));

}


void SceneEntity::moveModelMesh(QVector3D direction, bool update) {
	_mesh->vertexMove(direction);
	qDebug() << "moved vertex";
	if (update)
	{
		updateModelMesh();
	}
}

void SceneEntity::rotationDone()
{
	_mesh->vertexRotate(quatToMat(m_transform.rotation()).inverted());
	m_transform.setRotationX(0);
	m_transform.setRotationY(0);
	m_transform.setRotationZ(0);

	_mesh->vertexMove(m_transform.translation());
	m_transform.setTranslation(QVector3D(0, 0, 0));
	updateModelMesh();
}



void SceneEntity::rotateByNumber(QVector3D& rot_center, int X, int Y, int Z)
{
	QMatrix4x4 rot;
	rot = m_transform.rotateAround(rot_center, X, (QVector3D(1, 0, 0).toVector4D() * m_transform.matrix()).toVector3D());
	m_transform.setMatrix(m_transform.matrix() * rot);
	rot = m_transform.rotateAround(rot_center, Y, (QVector3D(0, 1, 0).toVector4D() * m_transform.matrix()).toVector3D());
	m_transform.setMatrix(m_transform.matrix() * rot);
	rot = m_transform.rotateAround(rot_center, Z, (QVector3D(0, 0, 1).toVector4D() * m_transform.matrix()).toVector3D());
	m_transform.setMatrix(m_transform.matrix() * rot);

	_mesh->vertexRotate(quatToMat(m_transform.rotation()).inverted());
	m_transform.setRotationX(0);
	m_transform.setRotationY(0);
	m_transform.setRotationZ(0);
	_mesh->vertexMove(m_transform.translation());
	m_transform.setTranslation(QVector3D(0, 0, 0));
	updateModelMesh();
}

void SceneEntity::rotateModelMesh(QMatrix4x4 matrix, bool update) {
	_mesh->vertexRotate(matrix);
	if (update)
	{
		updateModelMesh();
	}
}


void SceneEntity::rotateModelMesh(int Axis, float Angle, bool update) {
	Qt3DCore::QTransform tmp;
	switch (Axis) {
	case 1: {
		tmp.setRotationX(Angle);
		break;
	}
	case 2: {
		tmp.setRotationY(Angle);
		break;
	}
	case 3: {
		tmp.setRotationZ(Angle);
		break;
	}
	}
	rotateModelMesh(tmp.matrix(), update);
}



void SceneEntity::scaleModelMesh(float scaleX, float scaleY, float scaleZ) {
	/* To fix center of the model */
	float centerX = (_mesh->x_max() + _mesh->x_min()) / 2;
	float centerY = (_mesh->y_max() + _mesh->y_min()) / 2;
	_mesh->vertexScale(scaleX, scaleY, scaleZ, centerX, centerY);
	updateModelMesh();
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


const Mesh* SceneEntity::getMesh()
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
			addComponent(&_layer);
		}
		else
		{
			removeComponent(&_layer);
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

Qt3DRender::QLayer* SceneEntity::getLayer()
{
	return &_layer;
}

