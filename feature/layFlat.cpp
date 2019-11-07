#include "layFlat.h"
#include "qmlmanager.h"
using namespace Hix::Features;
Hix::Features::LayFlat::LayFlat(const std::unordered_set<GLModel*>& selectedModels):Feature(selectedModels)
{
}

void Hix::Features::LayFlat::faceSelected(GLModel* selected, const Hix::Engine3D::FaceConstItr& selectedFace, const Hix::Input::MouseEventData& mouse, const Qt3DRender::QRayCasterHit& hit)
{
	auto listed = selected->getRootModel();
	auto neighbors = selected->getMesh()->findNearSimilarFaces(selectedFace.localFn(), selectedFace);
	PPShaderFeature::colorFaces(selected, neighbors);
	auto loooocal = selectedFace.localFn();
	auto worldFn = selectedFace.worldFn();
	_selectedNormal = listed->vectorToLocal(worldFn);
	_isReady = true;



}

void Hix::Features::LayFlat::generateLayFlat()
{
	if (!_isReady)
		return;
	for (auto each : _subjects)
	{
		constexpr QVector3D worldBot(0, 0, -1);
		QVector3D localBotNorml = each->vectorToLocal(worldBot);
		auto rot = QQuaternion::rotationTo(_selectedNormal, localBotNorml);
		each->transform().setRotation(each->transform().rotation() * rot);
		each->updateRecursiveAabb();
		each->setZToBed();
		qmlManager->resetLayflat();
	}
}

Hix::Features::LayFlat::~LayFlat()
{
}
