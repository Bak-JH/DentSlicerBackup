#include "SupportSetting.h"
#include <fstream>
#include "../common/rapidjson/stringbuffer.h"
#include "../common/rapidjson/PrettyWriter.h"
#include "../common/rapidjson/ostreamwrapper.h"
#include "../application/ApplicationManager.h"

using namespace Hix::Settings;
using namespace Hix::Settings::JSON;

constexpr auto SUPPORT_FILE("supportSettings.json");

Hix::Settings::SupportSetting::SupportSetting(std::filesystem::path settingsPath)
{
	settingsPath.append(SUPPORT_FILE);
	_jsonPath = settingsPath;
}

Hix::Settings::SupportSetting::~SupportSetting()
{
}



void Hix::Settings::SupportSetting::parseJSONImpl(const rapidjson::Document& doc)
{
	parse(doc, "supportRadiusMax", supportRadiusMax);
	parse(doc, "supportRadiusMin", supportRadiusMin);
	parse(doc, "supportDensity", supportDensity);
	parse(doc, "supportBaseHeight", supportBaseHeight);
	parse(doc, "raftThickness", raftThickness);
	parseStrToEnum(doc, "supportType", supportType);
	parseStrToEnum(doc, "raftType", raftType);
}

void Hix::Settings::SupportSetting::initialize()
{
}




const std::filesystem::path& Hix::Settings::SupportSetting::jsonPath()
{
	return _jsonPath;
}

rapidjson::Document Hix::Settings::SupportSetting::doc()
{
	rapidjson::Document doc;
	doc.SetObject();
	doc.AddMember("supportRadiusMax", supportRadiusMax, doc.GetAllocator());
	doc.AddMember("supportRadiusMin", supportRadiusMin, doc.GetAllocator());
	doc.AddMember("supportDensity", supportDensity, doc.GetAllocator());
	doc.AddMember("supportBaseHeight", supportBaseHeight, doc.GetAllocator());
	doc.AddMember("raftThickness", raftThickness, doc.GetAllocator());
	doc.AddMember("supportType", std::string(magic_enum::enum_name(supportType)), doc.GetAllocator());
	doc.AddMember("raftType", std::string(magic_enum::enum_name(raftType)), doc.GetAllocator());
	return doc;
}

