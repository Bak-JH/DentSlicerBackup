#include "AppSetting.h"
#include <fstream>
#include "../common/rapidjson/stringbuffer.h"
#include "../common/rapidjson/PrettyWriter.h"
#include "../common/rapidjson/ostreamwrapper.h"


using namespace Hix::Settings;
using namespace Hix::Settings::JSON;
typedef rapidjson::GenericStringBuffer<rapidjson::UTF8<>, rapidjson::MemoryPoolAllocator<>> StringBuffer;

Hix::Settings::AppSetting::AppSetting()
{
	auto appSettingsPath = deployInfo.settingsDir;
	appSettingsPath.append("settings.json");
	parseJSON(appSettingsPath);
}

Hix::Settings::AppSetting::~AppSetting()
{
}

void Hix::Settings::AppSetting::refresh()
{
	JSONParsedSetting::refresh();
	deployInfo.refresh();
	auto printerPath = std::filesystem::current_path();
	printerPath.append(printerPresetPath);
	printerSetting.parseJSON(printerPath);
}

void Hix::Settings::AppSetting::setPrinterPath(const std::string& path)
{
	//temp

	printerPresetPath = path;
	char cbuf[1024]; rapidjson::MemoryPoolAllocator<> allocator(cbuf, sizeof cbuf);
	rapidjson::Document doc(&allocator, 256);
	doc.SetObject();
	doc.AddMember("printerPresetPath", printerPresetPath, allocator);
	doc.AddMember("enableErrorReport", enableErrorReport, allocator);

	std::ofstream of(_jsonPath, std::ios_base::trunc);
	rapidjson::OStreamWrapper osw{ of };
	rapidjson::PrettyWriter<rapidjson::OStreamWrapper> writer{ osw };
	doc.Accept(writer);
	refresh();
}


void Hix::Settings::AppSetting::initialize()
{
	printerPresetPath = "PrinterPresets/HixCapsule.json";
	enableErrorReport = true;
}

void Hix::Settings::AppSetting::parseJSONImpl(const rapidjson::Document& doc)
{
	tryParse(doc, "printerPresetPath", printerPresetPath);
	tryParse(doc, "enableErrorReport", enableErrorReport);

}
