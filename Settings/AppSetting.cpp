#include "AppSetting.h"
#include <fstream>
#include "../common/rapidjson/stringbuffer.h"
#include "../common/rapidjson/PrettyWriter.h"
#include "../common/rapidjson/ostreamwrapper.h"
#include "../application/ApplicationManager.h"

using namespace Hix::Settings;
using namespace Hix::Settings::JSON;
typedef rapidjson::GenericStringBuffer<rapidjson::UTF8<>, rapidjson::MemoryPoolAllocator<>> StringBuffer;
constexpr auto SETTING_FILE("settings.json");
constexpr auto SUPPORT_FILE("/supportSettings.json");

Hix::Settings::AppSetting::AppSetting(): sliceSetting(deployInfo.settingsDir), supportSetting(deployInfo.settingsDir)
{
	auto appSettingsPath = deployInfo.settingsDir;
	//appSettingsPath.append("/settings.json");
	//parseJSON(appSettingsPath);
	appSettingsPath.append(SETTING_FILE);
	_jsonPath = appSettingsPath;
}

Hix::Settings::AppSetting::~AppSetting()
{
}

void Hix::Settings::AppSetting::parseJSON()
{
	//auto printerPath = std::filesystem::current_path();
//printerPath.append(printerPresetPath);
//printerSetting.parseJSON(printerPath);
	deployInfo.parseJSON();
	JSONParsedSetting::parseJSON();
	printerSetting.parseJSON(printerPresetPath);
	sliceSetting.parseJSON();
	supportSetting.parseJSON();
	settingChanged();
}



void Hix::Settings::AppSetting::parseJSONImpl(const rapidjson::Document& doc)
{
	parse(doc, "printerPresetPath", printerPresetPath);
	tryParse(doc, "enableErrorReport", enableErrorReport);
}


void Hix::Settings::AppSetting::initialize()
{
	//auto defaultPreset = deployInfo.settingsDir;
	//defaultPreset.append("PrinterPresets/HixCapsule.json");
	//printerPresetPath = defaultPreset;
	enableErrorReport = true;
}

void Hix::Settings::AppSetting::settingChanged()
{
	Hix::Application::ApplicationManager::getInstance().sceneManager().drawBed();
}

void Hix::Settings::AppSetting::writeJSON()
{
	__super::writeJSON();
	//sliceSetting.writeJSON();
	//supportSetting.parseJSON();

}

const std::filesystem::path& Hix::Settings::AppSetting::jsonPath()
{
	return _jsonPath;
}

rapidjson::Document Hix::Settings::AppSetting::doc()
{
	rapidjson::Document doc;
	doc.SetObject();
	doc.AddMember("printerPresetPath", printerPresetPath, doc.GetAllocator());
	doc.AddMember("enableErrorReport", enableErrorReport, doc.GetAllocator());
	return doc;
}
