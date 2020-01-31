#include "ModeDialogQMLParsed.h"
#include "../../qmlmanager.h"
#include "../components/PopupShell.h"

using namespace Hix::QML;
using namespace Hix::Features;

Hix::Features::ModeDialogQMLParsed::ModeDialogQMLParsed(const QUrl qmlSourcePath): _component(qmlManager->engine, qmlSourcePath)
{
	auto popupShell = dynamic_cast<FeaturePopupShell*>(_component.create());
	_popup.reset(popupShell);
	_popup->setParentItem(qmlManager->mainItem);
}

Hix::Features::ModeDialogQMLParsed::~ModeDialogQMLParsed()
{
}

Hix::QML::ControlOwner& Hix::Features::ModeDialogQMLParsed::controlOwner()
{
	return *_popup.get();
}

