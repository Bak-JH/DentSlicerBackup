#pragma once
#include <QString>
#include <unordered_set>

namespace Hix
{
	namespace Support
	{
		class SupportRaftManager;
	}
}

class GLModel;

using namespace Hix::Engine3D;
namespace SlicingEngine
{
	struct Result
	{
		int time;
		int layerCount;
		QString size;
		float volume;
	};
	Result sliceModels (bool isTemp, float zMax, const std::unordered_set<GLModel*>& models, const Hix::Support::SupportRaftManager& suppRaft, QString filename = "");
};


