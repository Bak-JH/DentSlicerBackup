#include "SliceSetting.h"
#include <fstream>
#include "../common/rapidjson/stringbuffer.h"
#include "../common/rapidjson/PrettyWriter.h"
#include "../common/rapidjson/ostreamwrapper.h"
#include "../application/ApplicationManager.h"

using namespace Hix::Settings;
using namespace Hix::Settings::JSON;

constexpr auto SLICE_FILE("\\sliceSettings.json");

Hix::Settings::SliceSetting::SliceSetting(std::filesystem::path settingsPath)
{
	settingsPath.append(SLICE_FILE);
	_jsonPath = settingsPath;
}

Hix::Settings::SliceSetting::~SliceSetting()
{
}



void Hix::Settings::SliceSetting::parseJSONImpl(const rapidjson::Document& doc)
{
	parse(doc, "layerHeight", layerHeight);
	parse(doc, "invertX", invertX);
	parseStrToEnum(doc, "slicingMode", slicingMode);

}


void Hix::Settings::SliceSetting::initialize()
{
}



const std::filesystem::path& Hix::Settings::SliceSetting::jsonPath()
{
	return _jsonPath;
}

rapidjson::Document Hix::Settings::SliceSetting::doc()
{
	rapidjson::Document doc;
	doc.SetObject();
	doc.AddMember("layerHeight", layerHeight, doc.GetAllocator());
	doc.AddMember("invertX", invertX, doc.GetAllocator());
	doc.AddMember("slicingMode", std::string(magic_enum::enum_name(slicingMode)), doc.GetAllocator());

	return doc;
}
