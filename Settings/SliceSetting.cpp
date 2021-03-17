#include "SliceSetting.h"
#include <fstream>
#include "../common/rapidjson/stringbuffer.h"
#include "../common/rapidjson/PrettyWriter.h"
#include "../common/rapidjson/ostreamwrapper.h"
#include "../application/ApplicationManager.h"

using namespace Hix::Settings;
using namespace Hix::Settings::JSON;

constexpr auto SLICE_FILE("sliceSettings.json");

Hix::Settings::SliceSetting::SliceSetting() : JSONParsedSetting(SLICE_FILE)
{
}

Hix::Settings::SliceSetting::~SliceSetting()
{
}



void Hix::Settings::SliceSetting::parseJSONImpl(const rapidjson::Document& doc)
{
	parse(doc, "layerHeight", layerHeight);
	parseStrToEnum(doc, "slicingMode", slicingMode);
	parse(doc, "useGPU", useGPU);
	parse(doc, "AAXY", AAXY);
	parse(doc, "AAZ", AAZ);
	parse(doc, "minHeight", minHeight);
}


void Hix::Settings::SliceSetting::initialize()
{
}

rapidjson::Document Hix::Settings::SliceSetting::doc()
{
	rapidjson::Document doc;
	doc.SetObject();
	doc.AddMember("layerHeight", layerHeight, doc.GetAllocator());
	doc.AddMember("slicingMode", std::string(magic_enum::enum_name(slicingMode)), doc.GetAllocator());
	doc.AddMember("useGPU", useGPU, doc.GetAllocator());
	doc.AddMember("AAXY", AAXY, doc.GetAllocator());
	doc.AddMember("AAZ", AAZ, doc.GetAllocator());
	doc.AddMember("minHeight", minHeight, doc.GetAllocator());
	return doc;
}
