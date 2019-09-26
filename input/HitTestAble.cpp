#include "HitTestAble.h"
#include "../qmlmanager.h"
using namespace Hix::Input;

void HitTestAble::setHitTestable(bool isEnable)
{
	if (_hitEnabled != isEnable)
	{
		_hitEnabled = isEnable;
		if (_hitEnabled)
		{
			qmlManager->getRayCaster().addInputLayer(&_layer);
		}
		else
		{
			qmlManager->getRayCaster().removeInputLayer(&_layer);
		}
	}
}

void Hix::Input::HitTestAble::setHoverable(bool isEnable)
{
	if (_hoverEnabled != isEnable)
	{
		_hoverEnabled = isEnable;
		if (_hoverEnabled)
		{
			qmlManager->getRayCaster().addHoverLayer(&_layer);
		}
		else
		{
			qmlManager->getRayCaster().removeHoverLayer(&_layer);
		}
	}
}

bool HitTestAble::isHitTestable()
{
	return _hitEnabled;
}

bool Hix::Input::HitTestAble::isHoverable()
{
	return _hoverEnabled;
}

Qt3DRender::QLayer* Hix::Input::HitTestAble::getLayer()
{
	return &_layer;
}
