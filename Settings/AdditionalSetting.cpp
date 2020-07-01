#include "AdditionalSetting.h"
#include <fstream>
#include "../common/rapidjson/stringbuffer.h"
#include "../common/rapidjson/PrettyWriter.h"
#include "../common/rapidjson/ostreamwrapper.h"
#include "../application/ApplicationManager.h"

using namespace Hix::Settings;
using namespace Hix::Settings::JSON;

constexpr auto SLICE_FILE("sliceSettings.json");

Hix::Settings::AdditionalSetting::AdditionalSetting(std::filesystem::path settingsPath)
{
	settingsPath.append(SLICE_FILE);
	_jsonPath = settingsPath;
}

Hix::Settings::AdditionalSetting::~AdditionalSetting()
{
}



void Hix::Settings::AdditionalSetting::parseJSONImpl(const rapidjson::Document& doc)
{
	parseAllStr(doc, keyVals);
}


void Hix::Settings::AdditionalSetting::initialize()
{
}



const std::filesystem::path& Hix::Settings::AdditionalSetting::jsonPath()
{
	return _jsonPath;
}

rapidjson::Document Hix::Settings::AdditionalSetting::doc()
{
	rapidjson::Document doc;
	doc.SetObject();
	for (auto& each : keyVals)
	{
		rapidjson::Value n(each.first.c_str(), doc.GetAllocator());
		doc.AddMember(n, each.second, doc.GetAllocator());

	}
	return doc;
}
