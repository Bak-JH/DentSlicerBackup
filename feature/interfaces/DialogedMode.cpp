#include "DialogedMode.h"
#include <qstring.h>
#include <qquickitem.h>
#include <qqmlcomponent.h>
#include "../../application/ApplicationManager.h"

#include "../qml/components/PopupShell.h"
#include "../qml/components/Buttons.h"
using namespace Hix::Features;
using namespace Hix::QML;

Hix::Features::DialogedMode::DialogedMode(const QUrl& dialogUrl)
{
	//QQmlComponent component(Hix::Application::ApplicationManager::getInstance().engine, dialogUrl);
	QQmlComponent*  component = new QQmlComponent(&Hix::Application::ApplicationManager::getInstance().engine(), dialogUrl);
	auto featureArea = Hix::Application::ApplicationManager::getInstance().featureManager().featureArea();
	auto qmlInstance = component->create(qmlContext(featureArea));
	auto popupShell = dynamic_cast<FeaturePopupShell*>(qmlInstance);
	_popup.reset(popupShell);
	_popup->setParentItem(featureArea);
	auto& closeButton = _popup->closeButton();
	auto& applyButton = _popup->applyButton();
	QObject::connect(&closeButton, &Hix::QML::Controls::Button::clicked, [this]() {
		Hix::Application::ApplicationManager::getInstance().featureManager().setMode(nullptr);
		});
	QObject::connect(&applyButton, &Hix::QML::Controls::Button::clicked, [this]() {
		applyButtonClicked();
		Hix::Application::ApplicationManager::getInstance().featureManager().setMode(nullptr);
		});
}

Hix::Features::DialogedMode::~DialogedMode()
{
}

Hix::QML::ControlOwner& Hix::Features::DialogedMode::controlOwner()
{
	return *_popup.get();
}


bool Hix::Features::DialogedMode::isReady() const
{
	return _popup->getApplyReady();
}
