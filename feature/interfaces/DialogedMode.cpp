#include "DialogedMode.h"
#include <qstring.h>
#include <qquickitem.h>
#include <qqmlcomponent.h>
#include "../../application/ApplicationManager.h"
#include "../../qmlmanager.h"
#include "../qml/components/PopupShell.h"
#include "../qml/components/Buttons.h"
using namespace Hix::Features;
using namespace Hix::QML;

Hix::Features::DialogedMode::DialogedMode(const QUrl& dialogUrl)
{
	//QQmlComponent component(qmlManager->engine, dialogUrl);
	QQmlComponent*  component = new QQmlComponent(&Hix::Application::ApplicationManager::getInstance().engine(), dialogUrl);

	auto qmlInstance = component->create(qmlContext(qmlManager->featureArea));
	auto popupShell = dynamic_cast<FeaturePopupShell*>(qmlInstance);
	_popup.reset(popupShell);
	_popup->setParentItem(qmlManager->featureArea);
	auto& closeButton = _popup->closeButton();
	auto& applyButton = _popup->applyButton();
	QObject::connect(&closeButton, &Hix::QML::Controls::Button::clicked, [this]() {
		qmlManager->setMode(nullptr);
		});
	QObject::connect(&applyButton, &Hix::QML::Controls::Button::clicked, [this]() {
		apply();
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
