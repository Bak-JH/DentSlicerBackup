#pragma once

#include "render/SceneEntityWithMaterial.h"
#include "ui/Highlightable.h"
#include "input/Clickable.h"
#include "input/Hoverable.h"
#include "input/HitTestAble.h"

#include "feature/overhangDetect.h"
using namespace Qt3DCore;
using namespace Qt3DRender;
using namespace Qt3DExtras;


class GLModel;
namespace Hix
{
	using namespace Engine3D;
	using namespace OverhangDetect;
	namespace Support
	{
		class SupportRaftManager;
		class SupportModel : public Hix::Render::SceneEntityWithMaterial, public Hix::Input::HitTestAble, public Hix::Input::Clickable,public Hix::Input::Hoverable, public Hix::UI::Highlightable
		{
		public:
			void clicked(Hix::Input::MouseEventData&, const Qt3DRender::QRayCasterHit&)override;
			void onEntered()override;
			void onExited() override;
			void setHighlight(bool enable) override;
			virtual QVector3D getBasePt() = 0;
			const std::variant<VertexConstItr, FaceOverhang>& overhang();
			SupportModel(SupportRaftManager* manager, std::variant<VertexConstItr, FaceOverhang> overhang);
			virtual ~SupportModel();
		protected:
			void initHitTest()override;
			SupportRaftManager* _manager;
			std::variant<VertexConstItr, FaceOverhang> _overhang;
			QVector3D _basePt;
		};
	}
}
