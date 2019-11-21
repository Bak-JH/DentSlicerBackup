#include "SupportFeature.h"
#include "../../input/raycastcontroller.h"
#include "../../qmlmanager.h"
Hix::Features::SupportFeature::SupportFeature()
{
}

//void Hix::Features::SupportFeature::faceSelected(GLModel* selected, const Hix::Engine3D::FaceConstItr& selectedFace,
//	const Hix::Input::MouseEventData& mouse, const Qt3DRender::QRayCasterHit& hit)
//{
//	auto suppMode = qmlManager->supportRaftManager().supportEditMode();
//	if (suppMode == Hix::Support::EditMode::Manual) {
//		Hix::OverhangDetect::FaceOverhang newOverhang;
//		newOverhang.coord = selected->ptToRoot(hit.localIntersection());
//		newOverhang.face = selectedFace;
//		qmlManager->supportRaftManager().addSupport(newOverhang);
//	}
//}

Hix::Features::SupportFeature::~SupportFeature()
{
}
