#include "DrawingPlane.h"
using namespace Hix::Input;
using namespace Hix::Cut;

DrawingPlane::DrawingPlane(GLModel* owner)
{
	initHitTest();
	setEnabled(false);
	setHitTestable(false);
}

void Hix::Cut::DrawingPlane::enablePlane(bool isEnable)
{
	if (isEnable != isEnabled())
	{
		setEnabled(isEnable);
		if (!isEnabled)
		{
			enableDrawing(false);
		}
	}
}



void Hix::Cut::DrawingPlane::enableDrawing(bool isEnable)
{
	if (isEnable != _drawingEnabled)
	{
		_drawingEnabled = isEnabled;
		setHitTestable(_drawingEnabled);
	}

}

Qt3DCore::QTransform& Hix::Cut::DrawingPlane::transform()
{
	return _transform;
}

void Hix::Cut::DrawingPlane::clicked(Hix::Input::MouseEventData& m, const Qt3DRender::QRayCasterHit& hit)
{
	qDebug() << "plane clicked";
	qDebug() << hit.localIntersection();
}

void Hix::Cut::DrawingPlane::initHitTest()
{
	addComponent(&_layer);
	_layer.setRecursive(false);

}