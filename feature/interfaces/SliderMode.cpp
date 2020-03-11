#include "SliderMode.h"
#include "../../application/ApplicationManager.h"

const QUrl CUT_SLIDER_URL = QUrl("qrc:/Qml/SlideBar.qml");

Hix::Features::SliderMode::SliderMode(double min, double max)
{
	QQmlComponent* component = new QQmlComponent(&Hix::Application::ApplicationManager::getInstance().engine(), CUT_SLIDER_URL);
	auto slideArea = Hix::Application::ApplicationManager::getInstance().featureManager().slideArea();
	auto qmlInstance = component->create(qmlContext(slideArea));
	auto popupShell = dynamic_cast<Hix::QML::SlideBarShell*>(qmlInstance);
	_slideBar.reset(popupShell);
	_slideBar->setRange(min, max);
	
}



Hix::Features::SliderMode::~SliderMode()
{
}

void Hix::Features::SliderMode::setSliderEnable(bool isEnable)
{
	if (isEnable)
	{
		auto slideArea = Hix::Application::ApplicationManager::getInstance().featureManager().slideArea();
		_slideBar->setParentItem(slideArea);
	}
	else
	{
		_slideBar->setParentItem(nullptr);
	}
}
