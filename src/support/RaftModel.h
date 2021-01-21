#pragma once

#include "render/SceneEntityWithMaterial.h"
#include "widget/Highlightable.h"
#include "feature/overhangDetect.h"


class GLModel;
namespace Hix
{
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
