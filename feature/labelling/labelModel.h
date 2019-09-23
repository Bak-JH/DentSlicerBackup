#ifndef LABELMODEL_H
#define LABELMODEL_H

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
		class LabelModel : public Hix::Render::SceneEntityWithMaterial
		{
		public:
			LabelModel(Qt3DCore::QEntity* parent = nullptr);
			LabelModel(Qt3DCore::QEntity* parent, LabelModel& from);
			virtual ~LabelModel() {}
			void generateLabelModel(QString text, Hix::Engine3D::Mesh* targetMesh, 
								QVector3D targetNormal, float scale);
			void setTranslation(QVector3D t);

			// properties
			QVector3D translation;
			QVector3D normal;
			QString text;
			QFont font = QFont("Arial", 12, QFont::Normal);
		protected:
			QVector3D getPrimitiveColorCode(const Hix::Engine3D::Mesh* mesh, FaceConstItr faceItr)override;
		private:
			std::unordered_map<PolyNode*, std::vector<PolytreeCDT::Triangle>> _trigMap;
		};
	}
}

#endif // LABELMODEL_H
