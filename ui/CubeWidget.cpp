#include "CubeWidget.h"
#include "CubeWidget.h"
#include "qmlmanager.h"
#include "input/raycastcontroller.h"
#include "Widget3D.h"

Hix::UI::CubeWidget::CubeWidget(QEntity* parent, QVector3D overhangPoint) : QEntity(parent), _parent(dynamic_cast<Widget3D*>(parent))
{
	addComponent(&_mesh);
	addComponent(&_material);
	addComponent(&_transform);

	_mesh.setXExtent(0.3f);
	_mesh.setYExtent(0.3f);
	_mesh.setZExtent(0.3f);

	_parent->setManipulated(true);

	setHighlight(false);
	_material.setShininess(0);

	_transform.setTranslation(overhangPoint);

	setEnabled(true);
	addComponent(&_parent->layer);
}

Hix::UI::CubeWidget::~CubeWidget()
{
    removeComponent(&_mesh);
    removeComponent(&_material);
    removeComponent(&_transform);
}

bool Hix::UI::CubeWidget::isDraggable(Hix::Input::MouseEventData& e, const Qt3DRender::QRayCasterHit& hit)
{
    return _parent->visible();
}

void Hix::UI::CubeWidget::dragStarted(Hix::Input::MouseEventData& e, const Qt3DRender::QRayCasterHit& hit)
{
}

void Hix::UI::CubeWidget::doDrag(Hix::Input::MouseEventData& e)
{
}

void Hix::UI::CubeWidget::dragEnded(Hix::Input::MouseEventData& e)
{
}

void Hix::UI::CubeWidget::onEntered()
{
	qDebug() << "Entered";
	if (!_parent->isManipulated())
	{
		setHighlight(true);
		_material.setShininess(0);
	}
}

void Hix::UI::CubeWidget::onExited()
{
	if (!_parent->isManipulated())
	{
		setHighlight(false);
		_material.setShininess(0);
	}
}

void Hix::UI::CubeWidget::setHighlight(bool enable)
{
    auto color = Qt::yellow;
    if (enable)
    {
        color = Qt::red;
    }
    _material.setAmbient(color);
    _material.setDiffuse(color);
    _material.setSpecular(color);
}
