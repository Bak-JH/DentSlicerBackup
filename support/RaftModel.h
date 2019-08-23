#pragma once

#include "render/SceneEntityWithMaterial.h"
#include "input/Highlightable.h"
#include "input/Clickable.h"
#include "input/Hoverable.h"
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
		class VerticalSupportModel : public Hix::Render::SceneEntityWithMaterial, public Hix::Input::Clickable
		{
		public:
			void clicked(Hix::Input::MouseEventData&, const Qt3DRender::QRayCasterHit&)override;
			VerticalSupportModel(GLModel* owner, std::variant<VertexConstItr, FaceOverhang> overhang);
			virtual ~VerticalSupportModel();

		};
	}
}
