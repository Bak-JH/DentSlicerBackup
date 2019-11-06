#pragma once
#include <Qt3DCore>
#include <Qt3DExtras>
#include <array>
#include <unordered_set>
class GLModel;


namespace Hix
{
	namespace Features
	{
		class CrossSectionPlane : public Qt3DCore::QEntity
		{
			Q_OBJECT
		public:
			CrossSectionPlane(Qt3DCore::QEntity* owner);
			virtual ~CrossSectionPlane();
			void enablePlane(bool isEnable);
			Qt3DCore::QTransform& transform();
			void loadTexture(QString value);
		private:
			std::array<Qt3DExtras::QPlaneMesh, 2> _planeMeshes;
			Qt3DCore::QTransform _transform;
			Qt3DRender::QTextureLoader* _textureLoader;
			Qt3DExtras::QTextureMaterial* _planeMaterial;
		};


	}
}
