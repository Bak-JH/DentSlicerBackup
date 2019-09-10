#include "LineMeshEntity.h"
using namespace Qt3DCore;
using namespace Qt3DRender;
using namespace Hix::Render;

LineMeshEntity::LineMeshEntity(const std::vector<QVector3D>& vertices, Qt3DCore::QEntity* parent) : QEntity(parent)
, _points(vertices)
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
	for (const auto& v : _points) {
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
	//_positionAttribute.setByteStride(VTX_SIZE);
	_positionAttribute.setCount(_points.size());
	_positionAttribute.setName(QAttribute::defaultPositionAttributeName());

	_geometry.addAttribute(&_positionAttribute);



	_geometryRenderer.setInstanceCount(1);
	_geometryRenderer.setFirstVertex(0);
	_geometryRenderer.setFirstInstance(0);
	_geometryRenderer.setPrimitiveType(QGeometryRenderer::Lines);
	_geometryRenderer.setGeometry(&_geometry);
	_geometryRenderer.setVertexCount(0);

	_material.setAmbient(QColor(0, 0, 0));
	_material.setDiffuse(QColor(0, 0, 0));
	_material.setSpecular(QColor(0, 0, 0));

	addComponent(&_material);
	addComponent(&_transform);
	addComponent(&_geometryRenderer);
}

int LineMeshEntity::vertexCount()
{
	return _points.size();
}
