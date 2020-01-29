#include "ModeDialogQMLParsed.h"
#include "../../qmlmanager.h"
#include "../components/PopupShell.h"
using namespace Hix::QML;
using namespace Hix::Features;
Hix::Features::ModeDialogQMLParsed::ModeDialogQMLParsed(QUrl qmlSourcePath): _component(qmlManager->engine, qmlSourcePath)
{
	auto popupShell = dynamic_cast<LeftPopupShell*>(_component.create());
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

