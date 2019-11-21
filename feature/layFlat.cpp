#include "layFlat.h"
#include "qmlmanager.h"
using namespace Hix::Features;




//void Hix::Features::LayFlat::faceSelected(GLModel* selected, const Hix::Engine3D::FaceConstItr& selectedFace, const Hix::Input::MouseEventData& mouse, const Qt3DRender::QRayCasterHit& hit)
//{
//	auto listed = selected->getRootModel();
//	auto neighbors = selected->getMesh()->findNearSimilarFaces(selectedFace.localFn(), selectedFace);
//	PPShaderFeature::colorFaces(selected, neighbors);
//	auto loooocal = selectedFace.localFn();
//	auto worldFn = selectedFace.worldFn();
//	_selectedNormal = listed->vectorToLocal(worldFn);
//	_isReady = true;
//
//
//
//}

Hix::Features::LayFlat::~LayFlat()
{
}

Hix::Features::LayFlatMode::LayFlatMode(const std::unordered_set<GLModel*>& selectedModels)
	: PPShaderFeature(selectedModels)
{
}

Hix::Features::LayFlatMode::~LayFlatMode()
{
}

void Hix::Features::LayFlatMode::faceSelected(GLModel* selected, const Hix::Engine3D::FaceConstItr& selectedFace, const Hix::Input::MouseEventData& mouse, const Qt3DRender::QRayCasterHit& hit)
{
}

std::unique_ptr<LayFlat> Hix::Features::LayFlatMode::applyLayFlat()
{
	return std::unique_ptr<LayFlat>();
}

Hix::Features::LayFlat::LayFlat(const std::unordered_set<GLModel*>& selectedModels, QVector3D normal, bool isReady)
{
	//if (!isReady)
	//	return;
	//for (auto each : _subjects)
	//{
	//	constexpr QVector3D worldBot(0, 0, -1);
	//	QVector3D localBotNorml = each->vectorToLocal(worldBot);
	//	auto rot = QQuaternion::rotationTo(normal, localBotNorml);
	//	each->transform().setRotation(each->transform().rotation() * rot);
	//	each->updateRecursiveAabb();
	//	each->setZToBed();
	//	qmlManager->resetLayflat();
	//}
}