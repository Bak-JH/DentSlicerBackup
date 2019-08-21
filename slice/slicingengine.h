#ifndef SLICINGENGINE_H
#define SLICINGENGINE_H


#include <QString>

namespace tf
{
	class Subflow;
}



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
	Result sliceModel (bool isTemp, tf::Subflow& subflow, Mesh* shellMesh=nullptr, Mesh* supportMesh=nullptr, Mesh* raftMesh=nullptr, QString filename = "");
};

//extern QmlManager qm;

#endif // SLICINGENGINE_H
