#pragma once
#include <Qt3DCore>
#include <Qt3DRender>
#include <Qt3DExtras>
#include <vector>
namespace Hix
{
	namespace Render
	{
		class LineMeshEntity : Qt3DCore::QEntity
		{
			Q_OBJECT
		public:
			LineMeshEntity(const std::vector<QVector3D>& vertices, Qt3DCore::QEntity* parent);
			int vertexCount();
		private:
			std::vector<QVector3D> _points;
			Qt3DCore::QTransform _transform;
			Qt3DExtras::QPhongMaterial _material;

			Qt3DRender::QGeometryRenderer _geometryRenderer;
			Qt3DRender::QGeometry _geometry;

			//3 vectors per data, each for position, normal, color
			Qt3DRender::QBuffer _vertexBuffer;
			//defines mesh faces by 3 indices to the vertexArray
			//separates SSBO buffer for Per-primitive colors
			//Qt3DRender::QBuffer primitiveColorBuffer;
			Qt3DRender::QAttribute _positionAttribute;


		};


	}
}
