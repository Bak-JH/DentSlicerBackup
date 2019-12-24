#include "JSONParsedSetting.h"
#include "../common/rapidjson/writer.h"
#include "../common/rapidjson/stringbuffer.h"
#include "../common/rapidjson/filereadstream.h"
#include <QDebug>
using namespace Hix::Settings;
void Hix::Settings::JSONParsedSetting::parseJSON(const std::filesystem::path& jsonPath)
{
	_jsonPath = jsonPath;
	refresh();
}

void Hix::Settings::JSONParsedSetting::refresh()
{
	initialize();
	//check path
	auto status = std::filesystem::status(_jsonPath);
	if (status.type() == std::filesystem::file_type::regular)
	{
		auto file =
#ifdef _MSC_VER
			_wfopen(_jsonPath.c_str(), L"r");
#else
			std::fopen(p.c_str(), "r");
#endif

		char buffer[1000];
		rapidjson::FileReadStream is(file, buffer, sizeof(buffer));
		rapidjson::Document document;
		document.ParseStream<0, rapidjson::UTF8<>, rapidjson::FileReadStream>(is);
		if (document.HasParseError())
		{
			qDebug() << "json parse failed";
		}
		else
		{
			parseJSONImpl(document);
		}
	}
}
