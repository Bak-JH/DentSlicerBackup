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
	_slideBar->setParentItem(slideArea);
}

void Hix::Features::SliderMode::setSliderVisible(bool isVisible)
{
	_slideBar->setVisible(isVisible);
}

Hix::Features::SliderMode::~SliderMode()
{
}