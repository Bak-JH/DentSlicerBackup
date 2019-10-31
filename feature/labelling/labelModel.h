#ifndef LABELMODEL_H
#define LABELMODEL_H

#include "DentEngine/src/mesh.h"
#include "feature/Extrude.h"
#include "glmodel.h"

using namespace Hix::Features::Extrusion;
using namespace ClipperLib;
using namespace Hix::Polyclipping;
using namespace Hix::Engine3D;

namespace Hix
{
	class LabelModel : public GLModel
	{
	public:
		LabelModel(Qt3DCore::QEntity* parent = nullptr);
		LabelModel(Qt3DCore::QEntity* parent, LabelModel& from);
		virtual ~LabelModel() {}
		void generateLabel(QString text, Hix::Engine3D::Mesh* targetMesh, QVector3D targetNormal);
		void setTranslation(QVector3D t);

		// properties
		QString text;
		QFont font = QFont("Arial", 12, QFont::Normal);
	};
}

#endif // LABELMODEL_H