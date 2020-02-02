#include "ModeDialogQMLParsed.h"
#include <qstring.h>
#include <qquickitem.h>
#include "../../qmlmanager.h"
#include "../components/PopupShell.h"
#include "../components/Buttons.h"
using namespace Hix::QML;
using namespace Hix::Features;
Hix::Features::ModeDialogQMLParsed::ModeDialogQMLParsed(Mode* mode, QUrl qmlSourcePath): _component(qmlManager->engine, qmlSourcePath), _mode(mode)
{
	auto qmlInstance = _component.create(qmlContext(qmlManager->featureArea));
	auto popupShell = dynamic_cast<FeaturePopupShell*>(qmlInstance);
	_popup.reset(popupShell);
	_popup->setParentItem(qmlManager->featureArea);
	auto& closeButton = _popup->closeButton();
	auto& applyButton = _popup->applyButton();
	QObject::connect(&closeButton, Hix::QML::Controls::Button::clicked, [this]() {
		qmlManager->setMode(nullptr);
	});
	QObject::connect(&closeButton, Hix::QML::Controls::Button::clicked, [this]() {
		_mode->apply();
	});

}

Hix::Features::ModeDialogQMLParsed::~ModeDialogQMLParsed()
{
}

Hix::QML::ControlOwner& Hix::Features::ModeDialogQMLParsed::controlOwner()
{
	return *_popup.get();
}

