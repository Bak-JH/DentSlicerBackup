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
}

class GLModel;

namespace SlicingEngine
{

	std::vector<Hix::Slicer::Slice> sliceModels (float zMax, const std::unordered_set<GLModel*>& models, const Hix::Support::SupportRaftManager& suppRaft);
};


