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

Hix::Settings::BasicSetting::BasicSetting() : JSONParsedSetting(SETTING_FILE), enableErrorReport(true)
{
}

Hix::Settings::BasicSetting::~BasicSetting()
{
}



void Hix::Settings::BasicSetting::parseJSONImpl(const rapidjson::Document& doc)
{
	parse(doc, "printerPresetPath", printerPresetPath);
	auto& moddableSetting = Hix::Application::SettingsChanger::settings(Hix::Application::ApplicationManager::getInstance());
	moddableSetting.printerSetting.setPrinterPreset(printerPresetPath);
	tryParse(doc, "enableErrorReport", enableErrorReport);
	tryParse(doc, "latestFilePath", latestFilePath);
}


void Hix::Settings::BasicSetting::initialize()
{
	enableErrorReport = true;
}




rapidjson::Document Hix::Settings::BasicSetting::doc()
{
	rapidjson::Document doc;
	doc.SetObject();
	doc.AddMember("printerPresetPath", printerPresetPath, doc.GetAllocator());
	doc.AddMember("enableErrorReport", enableErrorReport, doc.GetAllocator());
	doc.AddMember("latestFilePath", latestFilePath, doc.GetAllocator());
	auto& moddableSetting = Hix::Application::SettingsChanger::settings(Hix::Application::ApplicationManager::getInstance());
	moddableSetting.printerSetting.setPrinterPreset(printerPresetPath);
	return doc;
}
