#pragma once
#include <Qt3DCore>
#include <Qt3DRender>
#include <Qt3DExtras>
#include <vector>

namespace Hix
{
	namespace Render
	{
		class PointMeshEntity :public Qt3DCore::QEntity
		{
			Q_OBJECT
		public:
			PointMeshEntity(const QVector3D vertex, Qt3DCore::QEntity* parent);
			PointMeshEntity(const QVector3D vertex, Qt3DCore::QEntity* parent, const QColor& color);

			PointMeshEntity(const std::vector<QVector3D>& vertices, Qt3DCore::QEntity* parent);
			PointMeshEntity(const std::vector<QVector3D>& vertices, Qt3DCore::QEntity* parent, const QColor& color);

			virtual ~PointMeshEntity();

		private:

			Qt3DRender::QBuffer* _vertexBuffer;
			Qt3DRender::QAttribute* _positionAttribute;

			Qt3DRender::QParameter* _pointColorParameter;

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
