#include "DrawingPlane.h"
using namespace Hix::Input;
using namespace Hix::Cut;

DrawingPlane::DrawingPlane(GLModel* owner)
{
	initHitTest();
}

void Hix::Cut::DrawingPlane::enablePlane(bool isEnable)
{
}

void Hix::Cut::DrawingPlane::setTransform(const Qt3DCore::QTransform& transform)
{
}

void Hix::Cut::DrawingPlane::enableDrawing(bool isEnable)
{
}

void Hix::Cut::DrawingPlane::clicked(Hix::Input::MouseEventData&, const Qt3DRender::QRayCasterHit&)
{
}

void Hix::Cut::DrawingPlane::initHitTest()
{
}
