#include "CubeWidget.h"
#include "qmlmanager.h"
#include "input/raycastcontroller.h"
#include "Widget3D.h"

Hix::UI::CubeWidget::CubeWidget(QEntity* parent) : QEntity(parent)
{
	addComponent(&_mesh);
	addComponent(&_material);
	addComponent(&_transform);

	_mesh.setXExtent(0.3f);
	_mesh.setYExtent(0.3f);
	_material.setShininess(0);
	initHitTest();
	setEnabled(true);
}

Hix::UI::CubeWidget::~CubeWidget()
{
    removeComponent(&_mesh);
    removeComponent(&_material);
    removeComponent(&_transform);
}

void Hix::UI::CubeWidget::initHitTest()
{
	addComponent(&_layer);
	_layer.setRecursive(false);
}
