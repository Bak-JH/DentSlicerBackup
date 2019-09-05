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

bool HitTestAble::isHitTestable()
{
	return _hitEnabled;
}

Qt3DRender::QLayer* Hix::Input::HitTestAble::getLayer()
{
	return &_layer;
}
