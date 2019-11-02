#include "RotateWidget.h"
#include "../qmlmanager.h"
#include "../input/raycastcontroller.h"
#include "Widget3D.h"
using namespace Hix::UI;
using namespace Qt3DCore;
using namespace Qt3DRender;
using namespace Qt3DExtras;

const float RotateWidget::ROTATE_SPEED = 0.1;
const float HALF_PI = M_PI / 2;
RotateWidget::RotateWidget(const QVector3D& axis, Qt3DCore::QEntity* parent):QEntity(parent), _axis(axis),
_parent(dynamic_cast<Widget3D*>(parent))
{

	addComponent(&_torus);
	addComponent(&_material);
	addComponent(&_transform);

	_torus.setRadius(7);
	_torus.setMinorRadius(0.35);
	_torus.setRings(50);
	_torus.setSlices(10);

	setHighlight(false);
	_material.setShininess(0);
	//set rotation of torus according to the turning axis
	//z turning axis,  flat on xy.
	if (_axis == QVector3D(0, 0, 1))
	{
	}
	//y turning axis
	else if (_axis == QVector3D(0, 1, 0))
	{
		_transform.setRotationX(90);
	}
	//x turning axis
	else if (_axis == QVector3D(1, 0, 0))
	{
		_transform.setRotationY(90);
	}
	setEnabled(true);
	addComponent(&_parent->layer);

}

Hix::UI::RotateWidget::~RotateWidget()
{
	removeComponent(&_torus);
	removeComponent(&_material);
	removeComponent(&_transform);
}

bool Hix::UI::RotateWidget::isDraggable(Hix::Input::MouseEventData& e, const Qt3DRender::QRayCasterHit& hit)
{
	return _parent->visible();
}

void Hix::UI::RotateWidget::dragStarted(Hix::Input::MouseEventData& e, const Qt3DRender::QRayCasterHit& hit)
{
	_parent->setManipulated(true);
	setHighlight(true);
	_mouseOrigin = e.position;
	_mousePrev = e.position;
    _mouseCurrent = e.position;
	_pastAngle = calculateRot();
}

double Hix::UI::RotateWidget::calculateRot()
{
	//find angle between (center, mouse start) and (center, current mouse)
	auto origin = qmlManager->world2Screen(_parent->transform()->translation());
	auto origVector = QVector2D(_mouseOrigin) - origin;
	auto currVector = QVector2D(_mouseCurrent) - origin;
	auto dotProduct = QVector2D::dotProduct(origVector, currVector);
	auto determinant = origVector.x() * currVector.y() - origVector.y() * currVector.x();
	auto angle = std::atan2(determinant, dotProduct);
	double degreeangle = angle * 180 / M_PI;

	//find angle between (vector into camera) and (rotating axis)
	auto axisWorldVector = qmlManager->systemTransform->rotation().rotatedVector(_axis);
	auto cameraVector = qmlManager->cameraViewVector();

	auto cosAngle = QVector3D::dotProduct(axisWorldVector, cameraVector)
		/(axisWorldVector.length() + cameraVector.length()) ;
	auto angleCameraAndAxis = std::acos(cosAngle);
	if (angleCameraAndAxis < HALF_PI)
	{
		degreeangle = -1.0 * degreeangle;
	}
    qDebug() << "deg angle" << degreeangle;
	return degreeangle;
}

void Hix::UI::RotateWidget::setHighlight(bool enable)
{
	auto color = Qt::gray;
	if (enable)
	{
		color = Qt::yellow;
	}
	_material.setAmbient(color);
	_material.setDiffuse(color);
	_material.setSpecular(color);
}

void Hix::UI::RotateWidget::doDrag(Hix::Input::MouseEventData& e)
{
	_mouseCurrent = e.position;
	auto curAngle = calculateRot();
	auto dif = curAngle - _pastAngle;
	_pastAngle = curAngle;
	qmlManager->modelRotateWithAxis(_axis, dif);
}

void Hix::UI::RotateWidget::dragEnded(Hix::Input::MouseEventData& e)
{
	_parent->setManipulated(false);
	setHighlight(false);
    qmlManager->totalRotateDone();
}

void Hix::UI::RotateWidget::onEntered()
{
	//only highlight when not being manipulated
	if (!_parent->isManipulated())
	{
		setHighlight(true);
	}
}

void Hix::UI::RotateWidget::onExited()
{
	if (!_parent->isManipulated())
	{
		setHighlight(false);

	}
}
