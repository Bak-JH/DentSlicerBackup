#include "LabelSetting.h"
#include <fstream>
#include "../common/rapidjson/stringbuffer.h"
#include "../common/rapidjson/PrettyWriter.h"
#include "../common/rapidjson/ostreamwrapper.h"
#include "../application/ApplicationManager.h"

using namespace Hix::Settings;
using namespace Hix::Settings::JSON;

constexpr auto LABEL_FILE("labelSettings.json");

Hix::Settings::LabelSetting::LabelSetting() : JSONParsedSetting(LABEL_FILE)
{
}

Hix::Settings::LabelSetting::~LabelSetting()
{
}

void Hix::Settings::LabelSetting::parseJSONImpl(const rapidjson::Document& doc)
{
	parse(doc, "labelFontFamily", labelFontFamily);
	parse(doc, "labelFontSize", labelFontSize);
	parse(doc, "labelDepth", labelDepth);
	parse(doc, "isEmboss", isEmboss);
}

void Hix::Settings::LabelSetting::initialize()
{
}

rapidjson::Document Hix::Settings::LabelSetting::doc()
{
	rapidjson::Document doc;
	doc.SetObject();
	doc.AddMember("labelFontFamily", labelFontFamily, doc.GetAllocator());
	doc.AddMember("labelFontSize", labelFontSize, doc.GetAllocator());
	doc.AddMember("labelDepth", labelDepth, doc.GetAllocator());
	doc.AddMember("isEmboss", isEmboss, doc.GetAllocator());
	return doc;
}
