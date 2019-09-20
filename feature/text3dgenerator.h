#ifndef TEXT3DGEOMETRYGENERATOR_H
#define TEXT3DGEOMETRYGENERATOR_H

#include <QVector3D>
#include <QFont>
#include <QString>
#include <QPainterPath>
#include <QTransform>
#include <QTextLayout>

#include "DentEngine/src/mesh.h"
#include "feature/Extrude.h"
#include "render/SceneEntityWithMaterial.h"



namespace Hix {
	using namespace Features::Extrusion;
	using namespace ClipperLib;
	using namespace Polyclipping;
	using namespace Engine3D;

	namespace Labelling 
	{
		class Text3D : public Hix::Render::SceneEntityWithMaterial
		{
		public:
			Text3D(Qt3DCore::QEntity* parent = nullptr);
			Text3D(Qt3DCore::QEntity* parent, Text3D& from);
			virtual ~Text3D() {}
			void generateText3D(QFont font, QString text, Hix::Engine3D::Mesh* targetMesh, 
								QVector3D targetNormal, float scale);
			void setTranslation(QVector3D t);

			// properties
			QVector3D translation;
			QVector3D normal;
			QString text;
			QString fontName = "Arial";
			int fontWeight = QFont::Normal;
			int fontSize = 12;

		protected:
			QVector3D getPrimitiveColorCode(const Hix::Engine3D::Mesh* mesh, FaceConstItr faceItr)override;
		private:
			std::vector<Path> IntPaths;
			PolyTree polytree;
			Clipper clpr;
			QPainterPath painterPath;
			std::unordered_map<PolyNode*, std::vector<PolytreeCDT::Triangle>> _trigMap;
		};
	}
}

#endif // TEXT3DGEOMETRYGENERATOR_H
