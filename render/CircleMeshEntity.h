#pragma once

#include "SceneEntityWithMaterial.h"
namespace Hix
{
	namespace Render
	{
		class CircleMeshEntity : public SceneEntityWithMaterial
		{
		public:
			CircleMeshEntity(float radius, size_t segCount, const QColor& color, Qt3DCore::QEntity* parent);
			virtual ~CircleMeshEntity();
		protected:
			QVector4D getPrimitiveColorCode(const Hix::Engine3D::Mesh* mesh, Hix::Engine3D::FaceConstItr faceItr)override;
		private:
			std::vector<QVector3D> _circle;

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