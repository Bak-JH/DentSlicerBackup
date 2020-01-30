#ifndef SLICINGENGINE_H
#define SLICINGENGINE_H


#include <QString>


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
	Result sliceModels (bool isTemp, float zMax, std::vector<std::reference_wrapper<const GLModel>>models, const Hix::Support::SupportRaftManager& suppRaft, QString filename = "");
};

//extern QmlManager qm;

#endif // SLICINGENGINE_H
