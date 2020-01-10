#include "MoveWidget.h"
#include "../qmlmanager.h"
#include "../input/raycastcontroller.h"
#include "Widget3D.h"
#include "feature/move.h"

//const float MoveWidget::ROTATE_SPEED = 0.1;
//const float HALF_PI = M_PI / 2;
const QUrl Hix::UI::MoveWidget::ARROW_MESH_URL(QStringLiteral("qrc:/Resource/mesh/arrow.stl"));
const double TRANSLATE_MULT = 1;
Hix::UI::MoveWidget::MoveWidget(const QVector3D& axis, Qt3DCore::QEntity* parent):Widget(axis, parent)
{
	addComponent(&_mesh);
	addComponent(&_material);
	addComponent(&_transform);
	_transform.setScale(0.3);

	_mesh.setSource(ARROW_MESH_URL);
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

Hix::UI::MoveWidget::~MoveWidget()
{
	removeComponent(&_mesh);
	removeComponent(&_material);
	removeComponent(&_transform);
}

bool Hix::UI::MoveWidget::isDraggable(Hix::Input::MouseEventData& e, const Qt3DRender::QRayCasterHit& hit)
{
	return _parent->visible();
}

void Hix::UI::MoveWidget::dragStarted(Hix::Input::MouseEventData& e, const Qt3DRender::QRayCasterHit& hit)
{
	dynamic_cast<Features::MoveMode*>(_parent->mode())->featureStarted();
	_parent->setManipulated(true);
	setHighlight(true);
	_transform.setScale3D(QVector3D(0.3f, 0.3f, 1.0f));
	_mouseOrigin = e.position;
	_mousePrev = e.position;
	_mouseCurrent = e.position;
	_pastProj = calculateMove();
}

void Hix::UI::MoveWidget::setHighlight(bool enable)
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


double Hix::UI::MoveWidget::calculateMove()
{
	auto origin = qmlManager->world2Screen(_parent->transform()->translation());
	auto positivePt = qmlManager->world2Screen(_parent->transform()->translation() + _axis);
	auto prjVector = positivePt - origin;
	auto projectedVector = QVector2D(_mouseCurrent) - origin;
	//project mouse point to axis
	auto scalarProj = QVector2D::dotProduct(prjVector, projectedVector) / prjVector.length();
	//need to figure out how much a 1 unit movement from screen equals to world
	auto a = qmlManager->world2Screen(_axis);
	auto b = qmlManager->world2Screen(_axis*2);
	auto c = (b - a).length();
	return scalarProj/c;
}

void Hix::UI::MoveWidget::doDrag(Hix::Input::MouseEventData& e)
{
	_mouseCurrent = e.position;
	auto currProj = calculateMove();
	auto diff = currProj - _pastProj;
	qDebug() << diff;
	_pastProj = currProj;
	qmlManager->modelMoveWithAxis(_axis, diff * TRANSLATE_MULT);
}

void Hix::UI::MoveWidget::dragEnded(Hix::Input::MouseEventData& e)
{
	dynamic_cast<Features::MoveMode*>(_parent->mode())->featureEnded();
	_transform.setScale3D(QVector3D(0.3f, 0.3f, 0.3f));
	_parent->setManipulated(false);
	setHighlight(false);
    //qmlManager->totalMoveDone();
}




void Hix::UI::MoveWidget::onEntered()
{
	//only highlight when not being manipulated
	if (!_parent->isManipulated())
	{
		setHighlight(true);
		_material.setShininess(0);
	}
}

void Hix::UI::MoveWidget::onExited()
{
	if (!_parent->isManipulated())
	{
		setHighlight(false);
		_material.setShininess(0);
	}
}
