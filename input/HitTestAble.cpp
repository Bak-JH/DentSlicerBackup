#include "HitTestAble.h"
#include "../application/ApplicationManager.h"

using namespace Hix::Input;

void HitTestAble::setHitTestable(bool isEnable)
{
	if (_hitEnabled != isEnable)
	{
		_hitEnabled = isEnable;
		if (_hitEnabled)
		{
			Hix::Application::ApplicationManager::getInstance().getRayCaster().addInputLayer(&_layer);
		}
		else
		{
			Hix::Application::ApplicationManager::getInstance().getRayCaster().removeInputLayer(&_layer);
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
			Hix::Application::ApplicationManager::getInstance().getRayCaster().addHoverLayer(&_layer);
		}
		else
		{
			Hix::Application::ApplicationManager::getInstance().getRayCaster().removeHoverLayer(&_layer);
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

Qt3DRender::QLayer* Hix::Input::HitTestAble::getLayer()const
{
	return &_layer;
}
