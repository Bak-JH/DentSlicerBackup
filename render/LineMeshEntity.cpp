#include "LineMeshEntity.h"
using namespace Qt3DCore;
using namespace Qt3DRender;
using namespace Hix::Render;

LineMeshEntity::LineMeshEntity(const std::vector<QVector3D>& vertices, Qt3DCore::QEntity* parent) : QEntity(parent)
, _geometryRenderer(this)
, _geometry(this)
, _vertexBuffer(Qt3DRender::QBuffer::VertexBuffer, this)
, _positionAttribute(this)
{


	//initialize vertex buffers etc
	_vertexBuffer.setUsage(Qt3DRender::QBuffer::DynamicDraw);
	_vertexBuffer.setAccessType(Qt3DRender::QBuffer::AccessType::ReadWrite);
	QByteArray vertexBufferData;
	vertexBufferData.resize(vertices.size() * 3 * sizeof(float));
	float* rawVertexArray = reinterpret_cast<float*>(vertexBufferData.data());
	int idx = 0;
	for (const auto& v : vertices) {
		rawVertexArray[idx++] = v.x();
		rawVertexArray[idx++] = v.y();
		rawVertexArray[idx++] = v.z();
	}

	_vertexBuffer.setData(vertexBufferData);

	_positionAttribute.setAttributeType(QAttribute::VertexAttribute);
	_positionAttribute.setBuffer(&_vertexBuffer);
	_positionAttribute.setDataType(QAttribute::Float);
	_positionAttribute.setDataSize(3);
	_positionAttribute.setByteOffset(0);
	_positionAttribute.setByteStride(3* sizeof(float));
	_positionAttribute.setCount(vertices.size());
	_positionAttribute.setName(QAttribute::defaultPositionAttributeName());

	_geometry.addAttribute(&_positionAttribute);



	_geometryRenderer.setInstanceCount(1);
	_geometryRenderer.setFirstVertex(0);
	_geometryRenderer.setFirstInstance(0);
	_geometryRenderer.setPrimitiveType(QGeometryRenderer::LineStrip);
	_geometryRenderer.setGeometry(&_geometry);
	_geometryRenderer.setVertexCount(vertices.size());

	_material.setAmbient(QColor(0, 0, 0));
	_material.setDiffuse(QColor(0, 0, 0));
	_material.setSpecular(QColor(0, 0, 0));

	addComponent(&_material);
	addComponent(&_transform);
	addComponent(&_geometryRenderer);
}

Hix::Render::LineMeshEntity::LineMeshEntity(const std::vector<QVector3D>& vertices, Qt3DCore::QEntity* parent, const QVector4D& color): LineMeshEntity(vertices, parent)
{
	_material.setAmbient(QColor(color.x(), color.y(), color.z()));
	_material.setDiffuse(QColor(color.x(), color.y(), color.z()));
	_material.setSpecular(QColor(color.x(), color.y(), color.z()));
}


LineMeshEntity::LineMeshEntity(const std::vector<std::vector<QVector3D>>& vertices, Qt3DCore::QEntity* parent) : QEntity(parent)
, _geometryRenderer(this)
, _geometry(this)
, _vertexBuffer(Qt3DRender::QBuffer::VertexBuffer, this)
, _positionAttribute(this)
{
	size_t vtxCount = 0;
	for (auto& path : vertices)
	{
		auto edgeCount = path.size() - 1;
		vtxCount += edgeCount * 2;
	}

	//initialize vertex buffers etc
	_vertexBuffer.setUsage(Qt3DRender::QBuffer::DynamicDraw);
	_vertexBuffer.setAccessType(Qt3DRender::QBuffer::AccessType::ReadWrite);
	QByteArray vertexBufferData;
	vertexBufferData.resize(vtxCount * 3 * sizeof(float));
	float* rawVertexArray = reinterpret_cast<float*>(vertexBufferData.data());
	int idx = 0;

	for (auto& path : vertices)
	{
		auto cend = path.cend() - 1;
		for (auto itr = path.cbegin(); itr != cend; ++itr)
		{
			auto next = itr + 1;
			//from
			rawVertexArray[idx++] = itr->x();
			rawVertexArray[idx++] = itr->y();
			rawVertexArray[idx++] = itr->z();
			
			//to
			rawVertexArray[idx++] = next->x();
			rawVertexArray[idx++] = next->y();
			rawVertexArray[idx++] = next->z();
		}
	}



	_vertexBuffer.setData(vertexBufferData);

	_positionAttribute.setAttributeType(QAttribute::VertexAttribute);
	_positionAttribute.setBuffer(&_vertexBuffer);
	_positionAttribute.setDataType(QAttribute::Float);
	_positionAttribute.setDataSize(3);
	_positionAttribute.setByteOffset(0);
	_positionAttribute.setByteStride(3 * sizeof(float));
	_positionAttribute.setCount(vtxCount);
	_positionAttribute.setName(QAttribute::defaultPositionAttributeName());

	_geometry.addAttribute(&_positionAttribute);



	_geometryRenderer.setInstanceCount(1);
	_geometryRenderer.setFirstVertex(0);
	_geometryRenderer.setFirstInstance(0);
	_geometryRenderer.setPrimitiveType(QGeometryRenderer::Lines);
	_geometryRenderer.setGeometry(&_geometry);
	_geometryRenderer.setVertexCount(vtxCount);

	_material.setAmbient(QColor(0, 0, 0));
	_material.setDiffuse(QColor(0, 0, 0));
	_material.setSpecular(QColor(0, 0, 0));

	addComponent(&_material);
	addComponent(&_transform);
	addComponent(&_geometryRenderer);
}

Hix::Render::LineMeshEntity::LineMeshEntity(const std::vector<std::vector<QVector3D>>& vertices, Qt3DCore::QEntity* parent, const QVector4D& color) : LineMeshEntity(vertices, parent)
{
	_material.setAmbient(QColor(color.x(), color.y(), color.z()));
	_material.setDiffuse(QColor(color.x(), color.y(), color.z()));
	_material.setSpecular(QColor(color.x(), color.y(), color.z()));
}
