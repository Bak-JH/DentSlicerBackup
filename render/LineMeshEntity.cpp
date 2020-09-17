#include "LineMeshEntity.h"
#include "application/ApplicationManager.h"
using namespace Qt3DCore;
using namespace Qt3DRender;
using namespace Hix::Render;

Hix::Render::LineMeshEntity::LineMeshEntity(Qt3DCore::QEntity* parent, QByteArray& vertexData, size_t vertexCount)
	
{
	
}


LineMeshEntity::LineMeshEntity(const std::vector<QVector3D>& vertices, Qt3DCore::QEntity* parent) 
	: LineMeshEntity(std::vector<std::vector<QVector3D>>({vertices}), parent)
{
}

Hix::Render::LineMeshEntity::LineMeshEntity(const std::vector<QVector3D>& vertices, Qt3DCore::QEntity* parent, const QColor& color)
	: LineMeshEntity(vertices, parent)
{
	_lineColorParameter->setValue(QColor::fromRgbF(color.redF(), color.greenF(), color.blueF(), color.alphaF()));
	_effect->addParameter(_lineColorParameter);
}


LineMeshEntity::LineMeshEntity(const std::vector<std::vector<QVector3D>>& vertices, Qt3DCore::QEntity* parent) 
	:QEntity(parent)
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
	, _lineColorParameter(new QParameter(QStringLiteral("lineColor"), QColor(0, 0, 0)))
{
	size_t vtxCount = 0;
	for (auto& path : vertices)
	{
		auto edgeCount = path.size() - 1;
		vtxCount += edgeCount * 2;
	}

	//initialize vertex buffers etc
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


	_vertexBuffer->setUsage(Qt3DRender::QBuffer::DynamicDraw);
	_vertexBuffer->setAccessType(Qt3DRender::QBuffer::AccessType::ReadWrite);
	_vertexBuffer->setData(vertexBufferData);

	_positionAttribute->setAttributeType(QAttribute::VertexAttribute);
	_positionAttribute->setBuffer(_vertexBuffer);
	_positionAttribute->setDataType(QAttribute::Float);
	_positionAttribute->setDataSize(3);
	_positionAttribute->setByteOffset(0);
	_positionAttribute->setByteStride(3 * sizeof(float));
	_positionAttribute->setCount(vtxCount);
	_positionAttribute->setName(QAttribute::defaultPositionAttributeName());

	_geometry.addAttribute(_positionAttribute);

	///
	_shaderProgram->setVertexShaderCode(QShaderProgram::loadSource(QUrl("qrc:/shaders/default.vert")));
	_shaderProgram->setFragmentShaderCode(QShaderProgram::loadSource(QUrl("qrc:/shaders/lineColor.frag")));
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

	_geometryRenderer.setVertexCount(vtxCount);
	_geometryRenderer.setInstanceCount(1);
	_geometryRenderer.setFirstVertex(0);
	_geometryRenderer.setFirstInstance(0);
	_geometryRenderer.setPrimitiveType(QGeometryRenderer::Lines);
	_geometryRenderer.setGeometry(&_geometry);

	addComponent(_material);
	addComponent(&_geometryRenderer);
}

Hix::Render::LineMeshEntity::LineMeshEntity(const std::vector<std::vector<QVector3D>>& vertices, Qt3DCore::QEntity* parent, const QColor& color) 
	: LineMeshEntity(vertices, parent)

{
	_lineColorParameter->setValue(QColor::fromRgbF(color.redF(), color.greenF(), color.blueF(), color.alphaF()));
	_effect->addParameter(_lineColorParameter);
}

Hix::Render::LineMeshEntity::~LineMeshEntity()
{
	_effect->removeParameter(_lineColorParameter);
	_effect->removeTechnique(_renderTechnique);
	_renderTechnique->removeFilterKey(_filterKey);
	_renderTechnique->removeRenderPass(_renderPass);
	_filterKey->clearPropertyTracking("renderingStyle");
}
