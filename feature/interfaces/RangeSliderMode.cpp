#include "RangeSliderMode.h"
#include "../../application/ApplicationManager.h"

const QUrl RURL = QUrl("qrc:/Qml/RangedSlider.qml");

Hix::Features::RangeSliderMode::RangeSliderMode(double min, double max)
{
	_component.reset(new QQmlComponent(&Hix::Application::ApplicationManager::getInstance().engine(), RURL));
#ifdef _DEBUG
	if (!_component->isReady())
	{
		qDebug() << _component->errors();
	}
#endif
	auto slideArea = Hix::Application::ApplicationManager::getInstance().featureManager().slideArea();
	auto qmlInstance = _component->create(qmlContext(slideArea));
	auto popupShell = dynamic_cast<Hix::QML::RangeSlideBarShell*>(qmlInstance);
	_slideBar.reset(popupShell);
	_slideBar->setMax(max);
	_slideBar->setMin(min);
	_slideBar->setParentItem(slideArea);
}



Hix::Features::RangeSliderMode::~RangeSliderMode()
{
}

Hix::QML::RangeSlideBarShell& Hix::Features::RangeSliderMode::slider()
{
	return *_slideBar;
}
