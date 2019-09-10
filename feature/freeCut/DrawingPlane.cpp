#include "DrawingPlane.h"
#include "FreeCutPtWidget.h"
#include "../../qmlmanager.h"
using namespace Hix::Input;
using namespace Hix::Features::Cut;

DrawingPlane::DrawingPlane(GLModel* owner)
{
	initHitTest();
	setEnabled(false);
	setHitTestable(false);
}

void DrawingPlane::enablePlane(bool isEnable)
{
	if (isEnable != isEnabled())
	{
		setEnabled(isEnable);
		if (!isEnable)
		{
			enableDrawing(false);
		}
	}
}



void DrawingPlane::enableDrawing(bool isEnable)
{
	if (isEnable != _drawingEnabled)
	{
		_drawingEnabled = isEnable;
		setHitTestable(_drawingEnabled);
	}

}

Qt3DCore::QTransform& DrawingPlane::transform()
{
	return _transform;
}

void DrawingPlane::clicked(Hix::Input::MouseEventData& m, const Qt3DRender::QRayCasterHit& hit)
{
	qDebug() << "plane clicked";
	qDebug() << hit.localIntersection();
	QVector3D pos = hit.localIntersection();
	pos.setZ(_transform.translation().z());
	_ptWidgets.emplace(std::make_unique<FreeCutPtWidget>(this, pos));
	if (_ptWidgets.size() >= 2)
	{
		QMetaObject::invokeMethod(qmlManager->cutPopup, "colorApplyFinishButton", Q_ARG(QVariant, 2));
	}

}

void Hix::Features::Cut::DrawingPlane::removePt(FreeCutPtWidget* pt)
{
	auto next = pt->next;
	auto prev = pt->prev;
	if (next)
	{
		next->prev = prev;
		next->updateLineTo();
	}
	if (prev)
	{
		prev->next = next;
	}
	std::unique_ptr<FreeCutPtWidget> tmp(pt);
	_ptWidgets.erase(tmp);
	//don't double delete
	tmp.release();
	
	if (_ptWidgets.size() < 2)
	{
		QMetaObject::invokeMethod(qmlManager->cutPopup, "colorApplyFinishButton", Q_ARG(QVariant, 0));
	}

}

void DrawingPlane::initHitTest()
{
	addComponent(&_layer);
	_layer.setRecursive(false);

}