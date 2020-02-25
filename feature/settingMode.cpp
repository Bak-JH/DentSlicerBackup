#include "settingMode.h"
#include "../qml/components/Inputs.h"
#include "../qml/components/ControlOwner.h"

#include "render/Color.h"
#include "glmodel.h"
#include "application/ApplicationManager.h"

using namespace Hix::Debug;
const QUrl EXTEND_POPUP_URL = QUrl("qrc:/Qml/FeaturePopup/PrintSettingPopup.qml");

Hix::Features::SettingMode::SettingMode() 
	:DialogedMode(EXTEND_POPUP_URL)
{
	auto& co = controlOwner();
	co.getControl(_extendValue, "extendvalue");
}

Hix::Features::SettingMode::~SettingMode()
{
}

void Hix::Features::SettingMode::applyButtonClicked()
{
}

