#ifndef SLICINGENGINE_H
#define SLICINGENGINE_H


#include <QString>

namespace tf
{
	class Subflow;
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
	Result sliceModels (bool isTemp, tf::Subflow& subflow,float zMax, float zMin, std::vector<const GLModel*>models, QString filename = "");
};

//extern QmlManager qm;

#endif // SLICINGENGINE_H
