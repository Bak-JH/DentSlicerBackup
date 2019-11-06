#ifndef LABELMODEL_H
#define LABELMODEL_H

#include "DentEngine/src/mesh.h"
#include "feature/Extrude.h"
#include "render/SceneEntityWithMaterial.h"

#include "ui/Highlightable.h"
#include "input/Clickable.h"
#include "input/Hoverable.h"
#include "input/HitTestAble.h"

namespace Hix {
	using namespace Features::Extrusion;
	using namespace ClipperLib;
	using namespace Polyclipping;
	using namespace Engine3D;

	namespace Labelling 
	{
		class LabelModel : public Hix::Render::SceneEntityWithMaterial, public Hix::Input::HitTestAble, public Hix::Input::Clickable, Hix::Input::Hoverable, public Hix::UI::Highlightable
		{
		public:
			LabelModel(Qt3DCore::QEntity* parent = nullptr);
			LabelModel(Qt3DCore::QEntity* parent, LabelModel& from);
			virtual ~LabelModel() {}
			void generateLabel(QString text, Hix::Engine3D::Mesh* targetMesh, 
								QVector3D targetNormal, float scale);
			void setTranslation(QVector3D t);
			void clicked(Hix::Input::MouseEventData&, const Qt3DRender::QRayCasterHit&)override;
			void onEntered()override;
			void onExited() override;
			void setHighlight(bool enable) override;

			// properties
			QVector3D translation;
			QVector3D normal;
			QString text;
			QFont font = QFont("Arial", 12, QFont::Normal);
		protected:
			QVector4D getPrimitiveColorCode(const Hix::Engine3D::Mesh* mesh, FaceConstItr faceItr)override;
		private:
			void initHitTest()override;
			
		};
	}
}

#endif // LABELMODEL_H
