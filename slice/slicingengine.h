#pragma once
#include <QString>
#include <unordered_set>
#include "DentEngine/src/slicer.h"

namespace Hix
{
	namespace Support
	{
		class SupportRaftManager;
	}
	namespace Render
	{
		class SceneEntity;
	}
}

class GLModel;

namespace SlicingEngine
{
	//slice models, automatically include supports and raft
	std::vector<Hix::Slicer::LayerGroup> sliceModels (const std::unordered_set<GLModel*>& models, const Hix::Support::SupportRaftManager& suppRaft, float delta);
	std::vector<Hix::Slicer::LayerGroup> sliceEntities(const std::unordered_set<const Hix::Render::SceneEntity*>& models, float delta);
	std::unordered_set<const Hix::Render::SceneEntity*> selectedToEntities(const std::unordered_set<GLModel*>& models, const Hix::Support::SupportRaftManager& suppRaft);

};


