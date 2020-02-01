#include "ModeDialogQMLParsed.h"
#include <qstring.h>
#include <qquickitem.h>
#include "../../qmlmanager.h"
#include "../components/PopupShell.h"

using namespace Hix::QML;
using namespace Hix::Features;
Hix::Features::ModeDialogQMLParsed::ModeDialogQMLParsed(QUrl qmlSourcePath): _component(qmlManager->engine, qmlSourcePath)
{
	auto qmlInstance = _component.create(qmlContext(qmlManager->featureArea));
	auto popupShell = dynamic_cast<FeaturePopupShell*>(qmlInstance);
	_popup.reset(popupShell);
	_popup->setParentItem(qmlManager->featureArea);
}

Hix::Features::ModeDialogQMLParsed::~ModeDialogQMLParsed()
{
}

Hix::QML::ControlOwner& Hix::Features::ModeDialogQMLParsed::controlOwner()
{
	return *_popup.get();
}

