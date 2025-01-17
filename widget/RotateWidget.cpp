#include "RotateWidget.h"

#include "../input/raycastcontroller.h"
#include "Widget3D.h"
#include "feature/rotate.h"
#include "../application/ApplicationManager.h"
#include "../glmodel.h"
using namespace Hix::UI;
using namespace Qt3DCore;
using namespace Qt3DRender;
using namespace Qt3DExtras;

const float RotateWidget::ROTATE_SPEED = 0.1;
const float HALF_PI = M_PI / 2;
RotateWidget::RotateWidget(const QVector3D& axis, Qt3DCore::QEntity* parent, const std::unordered_set<GLModel*>& models) :Widget(axis, parent), _models(models)
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

}

Hix::UI::RotateWidget::~RotateWidget()
{
	removeComponent(&_torus);
	removeComponent(&_material);
	removeComponent(&_transform);
}

bool Hix::UI::RotateWidget::isDraggable(Hix::Input::MouseEventData& e, const Qt3DRender::QRayCasterHit& hit)
{
	return _parent->visible() && e.button == Qt3DInput::QMouseEvent::Buttons::LeftButton;
}

void Hix::UI::RotateWidget::dragStarted(Hix::Input::MouseEventData& e, const Qt3DRender::QRayCasterHit& hit)
{
	dynamic_cast<Features::WidgetMode*>(_parent->mode())->featureStarted();
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
	auto origin = Hix::Application::ApplicationManager::getInstance().sceneManager().worldToScreen(_parent->transform()->translation());
	auto origVector = QVector2D(_mouseOrigin) - origin;
	auto currVector = QVector2D(_mouseCurrent) - origin;
	auto dotProduct = QVector2D::dotProduct(origVector, currVector);
	auto determinant = origVector.x() * currVector.y() - origVector.y() * currVector.x();
	auto angle = std::atan2(determinant, dotProduct);
	double degreeangle = angle * 180 / M_PI;

	//find angle between (vector into camera) and (rotating axis)
	auto axisWorldVector = Hix::Application::ApplicationManager::getInstance().sceneManager().systemTransform()->rotation().rotatedVector(_axis);
	//auto cameraVector = Hix::Application::ApplicationManager::getInstance().sceneManager().cameraViewVector();
	auto cameraVector = -Hix::Application::ApplicationManager::getInstance().sceneManager().camera()->viewVector();

	auto cosAngle = QVector3D::dotProduct(axisWorldVector, cameraVector)
		/(axisWorldVector.length() + cameraVector.length()) ;
	auto angleCameraAndAxis = std::acos(cosAngle);
	if (angleCameraAndAxis < HALF_PI)
	{
		degreeangle = -1.0 * degreeangle;
	}
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
	for (auto selectedModel : _models) {
		auto rotation = QQuaternion::fromAxisAndAngle(_axis, dif);
		selectedModel->rotateModel(rotation);
	}

}

void Hix::UI::RotateWidget::dragEnded(Hix::Input::MouseEventData& e)
{
	dynamic_cast<Features::WidgetMode*>(_parent->mode())->featureEnded();
	_parent->setManipulated(false);
	setHighlight(false);
    //Hix::Application::ApplicationManager::getInstance().totalRotateDone();
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