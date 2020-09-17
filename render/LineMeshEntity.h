#pragma once
#include <Qt3DCore>
#include <Qt3DRender>
#include <Qt3DExtras>
#include <vector>

namespace Hix
{
	namespace Render
	{
		class LineMeshEntity :public Qt3DCore::QEntity
		{
			Q_OBJECT
		public:
			LineMeshEntity(const std::vector<QVector3D>& vertices, Qt3DCore::QEntity* parent);
			LineMeshEntity(const std::vector<QVector3D>& vertices, Qt3DCore::QEntity* parent, const QColor& color);

			LineMeshEntity(const std::vector<std::vector<QVector3D>>& vertices, Qt3DCore::QEntity* parent);
			LineMeshEntity(const std::vector<std::vector<QVector3D>>& vertices, Qt3DCore::QEntity* parent, const QColor& color);
		
			virtual ~LineMeshEntity();
		private:
			void initialize(Qt3DCore::QEntity* parent, QByteArray& vertexData, size_t vertexCount);

			Qt3DRender::QBuffer* _vertexBuffer;
			Qt3DRender::QAttribute* _positionAttribute;

			Qt3DRender::QParameter* _lineColorParameter;

			Qt3DRender::QShaderProgram* _shaderProgram;
			Qt3DRender::QRenderPass* _renderPass;
			Qt3DRender::QFilterKey* _filterKey;
			Qt3DRender::QTechnique* _renderTechnique;

			Qt3DRender::QEffect* _effect;
			Qt3DRender::QMaterial* _material;

			Qt3DRender::QGeometryRenderer _geometryRenderer;
			Qt3DRender::QGeometry _geometry;

		};


	}
}
