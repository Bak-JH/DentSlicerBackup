#include "settingMode.h"
#include "../qml/components/Inputs.h"
#include "../qml/components/Buttons.h"
#include "../qml/components/ControlOwner.h"

#include "render/Color.h"
#include "glmodel.h"
#include "application/ApplicationManager.h"

using namespace Hix::Debug;
using namespace Hix::Settings;
namespace fs = std::filesystem;


const QUrl EXTEND_POPUP_URL = QUrl("qrc:/Qml/FeaturePopup/PrintSettingPopup.qml");

Hix::Features::SettingMode::SettingMode() 
	:DialogedMode(EXTEND_POPUP_URL)
{
	auto& co = controlOwner();
	co.getControl(_printerPresets, "printerPreset");
	co.getControl(_snapshotSize, "snaphotSize");
	co.getControl(_profileBttn, "profileButton");
	co.getControl(_updateBttn, "updateButton");
	co.getControl(_resetBttn, "resetSettings");

	//get settings dir
	auto printerPresetsDir = Hix::Application::ApplicationManager::getInstance().settings().printerSetting.printerPresetsPath();
	fs::directory_iterator presetItr(printerPresetsDir);
	QStringList presets;
	for (auto& e : presetItr)
	{
		if (e.is_regular_file() && e.path().extension() == ".json")
		{
			_presetPaths.emplace_back(e.path());
			presets.push_back(QString::fromStdString(e.path().filename().string()));
		}
	}
	fs::path oldPreset = Hix::Application::ApplicationManager::getInstance().settings().basicSetting.printerPresetPath;
	QString oldPresetStr = QString::fromStdString(oldPreset.filename().string());
	_oldIndex = presets.indexOf(oldPresetStr);
	_printerPresets->setList(presets);
	_printerPresets->setIndex(_oldIndex);

	int snapshotSize = Hix::Application::ApplicationManager::getInstance().settings().basicSetting.snapshotSize;
	_snapshotSize->setValue(snapshotSize);

	QObject::connect(_profileBttn, &Hix::QML::Controls::Button::clicked, [this]() {
		auto& auth = Hix::Application::ApplicationManager::getInstance().auth();
		auth.profile();
		});
	QObject::connect(_updateBttn, &Hix::QML::Controls::Button::clicked, [this]() {
		Hix::Application::ApplicationManager::getInstance().updater().forceCheckForUpdates();
		});

	QObject::connect(_resetBttn, &Hix::QML::Controls::Button::clicked, [this]() {
		auto& moddableSetting = Hix::Application::SettingsChanger::settings(Hix::Application::ApplicationManager::getInstance());
		moddableSetting.toDefault();
		});

}

Hix::Features::SettingMode::~SettingMode()
{
}

void Hix::Features::SettingMode::applyButtonClicked()
{
	if (isDirty())
	{
		//set settings
		auto path = _presetPaths[_printerPresets->getIndex()];
		auto& moddableSetting = Hix::Application::SettingsChanger::settings(Hix::Application::ApplicationManager::getInstance());
		moddableSetting.basicSetting.printerPresetPath = path.filename().string();
		moddableSetting.basicSetting.snapshotSize = _snapshotSize->getValue();
		//save settings
		moddableSetting.basicSetting.writeJSON();
	}
}

bool Hix::Features::SettingMode::isDirty() const
{
	auto& moddableSetting = Hix::Application::SettingsChanger::settings(Hix::Application::ApplicationManager::getInstance());
	return _printerPresets->getIndex() != _oldIndex || moddableSetting.basicSetting.snapshotSize != _snapshotSize->getValue();
}

