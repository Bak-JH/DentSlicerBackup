#include "shelloffset.h"
#include "qmlmanager.h"
#include "feature/repair/meshrepair.h"
#include "DentEngine/src/mesh.h"
#include "cut/ZAxialCut.h"
// offset shell with mm

using namespace Hix::Engine3D;
using namespace Hix::Features;
using namespace Hix::Features::Cut;
Hix::Features::ShellOffset::ShellOffset(GLModel* glmodel) : _subject(glmodel), _cuttingPlane(qmlManager->total)
{
	_modelBound = _subject->recursiveAabb();
	_cuttingPlane.enablePlane(true);

}

Hix::Features::ShellOffset::~ShellOffset()
{
}

void Hix::Features::ShellOffset::getSliderSignal(double value)
{
	float zlength = _modelBound.lengthZ();
	_cuttingPlane.transform().setTranslation(QVector3D(0, 0, _modelBound.zMin() + value * zlength / 1.8));
}

void Hix::Features::ShellOffset::doOffset(float offset)
{
	doOffsetImpl(_subject, offset, _cuttingPlane.transform().translation().z());

}

void Hix::Features::ShellOffset::doOffsetImpl(GLModel* glModel, float offset, float zPlane)
{
	Mesh offsetMesh(*glModel->getMesh());

	qDebug() << "copy, offset, reverse, add";
	qmlManager->setProgress(0.42);
	offsetMesh.vertexOffset(-offset);
	offsetMesh.reverseFaces();

	// 승환 60%
	qmlManager->setProgress(0.54);
	glModel->getMeshModd()->operator+=(offsetMesh);

	qDebug() << "cut and fill hole";

	qmlManager->setProgress(0.70);
	// 승환 100%
	qmlManager->setProgress(1);
	ZAxialCut(glModel, zPlane, ZAxialCut::Result::KeepTop);

}
