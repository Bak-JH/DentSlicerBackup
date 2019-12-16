#include "shelloffset.h"
#include "qmlmanager.h"
#include "feature/repair/meshrepair.h"
#include "DentEngine/src/mesh.h"
#include "cut/ZAxialCut.h"
// offset shell with mm

using namespace Hix::Engine3D;
using namespace Hix::Features;
using namespace Hix::Features::Cut;
Hix::Features::ShellOffsetMode::ShellOffsetMode(GLModel* glmodel) : _subject(glmodel), _cuttingPlane(qmlManager->total)
{
	_modelBound = _subject->recursiveAabb();
	_cuttingPlane.enablePlane(true);

}

Hix::Features::ShellOffsetMode::~ShellOffsetMode()
{
}

void Hix::Features::ShellOffsetMode::getSliderSignal(double value)
{
	float zlength = _modelBound.lengthZ();
	_cuttingPlane.transform().setTranslation(QVector3D(0, 0, _modelBound.zMin() + value * zlength / 1.8));
}

Hix::Features::Feature* Hix::Features::ShellOffsetMode::doOffset(float offset)
{
	return new ShellOffset(_subject, offset, _cuttingPlane.transform().translation().z());
}





Hix::Features::ShellOffset::ShellOffset(GLModel* target, float offset, float zPlane) : _prevMesh(target->getMeshModd())
{
	Mesh offsetMesh(*target->getMeshModd());

	qDebug() << "copy, offset, reverse, add";
	qmlManager->setProgress(0.42);
	offsetMesh.vertexOffset(-offset);
	offsetMesh.reverseFaces();

	// 승환 60%
	qmlManager->setProgress(0.54);
	target->getMeshModd()->operator+=(offsetMesh);

	qDebug() << "cut and fill hole";

	qmlManager->setProgress(0.70);
	// 승환 100%
	qmlManager->setProgress(1);
	cut = new ZAxialCut(target, zPlane, ZAxialCut::Result::KeepTop);
}

Hix::Features::ShellOffset::~ShellOffset()
{
}
