#pragma once

#include "render/SceneEntityWithMaterial.h"
#include "ui/Highlightable.h"
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
		class SupportRaftManager;
		class RaftModel : public Hix::Render::SceneEntityWithMaterial
		{
		public:
			RaftModel(SupportRaftManager* manager);
			virtual ~RaftModel();
		protected:
			SupportRaftManager* _manager;
		};
	}
}
