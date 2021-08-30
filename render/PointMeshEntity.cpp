#include "PointMeshEntity.h"
#include "application/ApplicationManager.h"
using namespace Qt3DCore;
using namespace Qt3DRender;
using namespace Hix::Render;

Hix::Render::PointMeshEntity::PointMeshEntity(const QVector3D vertex, Qt3DCore::QEntity* parent)
	: PointMeshEntity(std::vector<QVector3D>({ vertex }), parent)
{
}

Hix::Render::PointMeshEntity::PointMeshEntity(const QVector3D vertex, Qt3DCore::QEntity* parent, const QColor& color)
	: PointMeshEntity(vertex, parent)
{
	_pointColorParameter->setValue(QColor::fromRgbF(color.redF(), color.greenF(), color.blueF(), color.alphaF()));
	_effect->addParameter(_pointColorParameter);
}

Hix::Render::PointMeshEntity::PointMeshEntity(const std::vector<QVector3D>& vertices, Qt3DCore::QEntity* parent)
	:Qt3DCore::QEntity(parent)
	, _geometryRenderer(this)
	, _geometry(this)
	, _vertexBuffer(new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer, this))
	, _positionAttribute(new Qt3DRender::QAttribute(this))
	, _shaderProgram(new Qt3DRender::QShaderProgram(this))
	, _renderPass(new Qt3DRender::QRenderPass(this))
	, _renderTechnique(new Qt3DRender::QTechnique(this))
	, _material(new Qt3DRender::QMaterial(this))
	, _effect(new Qt3DRender::QEffect(this))
	, _filterKey(new Qt3DRender::QFilterKey(this))
	, _pointColorParameter(new QParameter(QStringLiteral("color"), QColor(0, 0, 0)))
{
	//initialize vertex buffers etc
	QByteArray vertexBufferData;
	vertexBufferData.resize(vertices.size() * sizeof(QVector3D));
	float* rawVertexArray = reinterpret_cast<float*>(vertexBufferData.data());
	int idx = 0;

	for (auto vert : vertices)
	{
		rawVertexArray[idx++] = vert.x();
		rawVertexArray[idx++] = vert.y();
		rawVertexArray[idx++] = vert.z();
		
	}



	_vertexBuffer->setUsage(Qt3DRender::QBuffer::DynamicDraw);
	_vertexBuffer->setAccessType(Qt3DRender::QBuffer::AccessType::ReadWrite);
	_vertexBuffer->setData(vertexBufferData);

	_positionAttribute->setAttributeType(QAttribute::VertexAttribute);
	_positionAttribute->setBuffer(_vertexBuffer);
	_positionAttribute->setDataType(QAttribute::Float);
	_positionAttribute->setDataSize(3);
	_positionAttribute->setByteOffset(0);
	_positionAttribute->setByteStride(3 * sizeof(float));
	_positionAttribute->setCount(vertices.size());
	_positionAttribute->setName(QAttribute::defaultPositionAttributeName());

	_geometry.addAttribute(_positionAttribute);

	///
	_shaderProgram->setVertexShaderCode(QShaderProgram::loadSource(QUrl("qrc:/shaders/default.vert")));
	_shaderProgram->setFragmentShaderCode(QShaderProgram::loadSource(QUrl("qrc:/shaders/singleColorNoLight.frag")));
	_renderPass->setShaderProgram(_shaderProgram);

	_renderTechnique->addRenderPass(_renderPass);
	_renderTechnique->graphicsApiFilter()->setApi(QGraphicsApiFilter::OpenGL);
	_renderTechnique->graphicsApiFilter()->setMajorVersion(4);
	_renderTechnique->graphicsApiFilter()->setMinorVersion(3);
	_renderTechnique->graphicsApiFilter()->setProfile(QGraphicsApiFilter::CoreProfile);
	_filterKey->setName(QStringLiteral("renderingStyle"));
	_filterKey->setValue(QStringLiteral("forward"));
	_renderTechnique->addFilterKey(_filterKey);
	_effect->addTechnique(_renderTechnique);

	_material->setEffect(_effect);

	_geometryRenderer.setVertexCount(vertices.size());
	_geometryRenderer.setInstanceCount(1);
	_geometryRenderer.setFirstVertex(0);
	_geometryRenderer.setFirstInstance(0);
	_geometryRenderer.setPrimitiveType(QGeometryRenderer::Points);
	_geometryRenderer.setGeometry(&_geometry);

	addComponent(_material);
	addComponent(&_geometryRenderer);
}

Hix::Render::PointMeshEntity::PointMeshEntity(const std::vector<QVector3D>& vertices, Qt3DCore::QEntity* parent, const QColor& color)
	: PointMeshEntity(vertices, parent)
{
	_pointColorParameter->setValue(QColor::fromRgbF(color.redF(), color.greenF(), color.blueF(), color.alphaF()));
	_effect->addParameter(_pointColorParameter);
}

Hix::Render::PointMeshEntity::~PointMeshEntity()
{
	_effect->removeParameter(_pointColorParameter);
	_effect->removeTechnique(_renderTechnique);
	_renderTechnique->removeFilterKey(_filterKey);
	_renderTechnique->removeRenderPass(_renderPass);
	_filterKey->clearPropertyTracking("renderingStyle");
}
