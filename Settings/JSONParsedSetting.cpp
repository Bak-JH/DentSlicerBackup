#include "JSONParsedSetting.h"
#include "../common/rapidjson/writer.h"
#include "../common/rapidjson/stringbuffer.h"
#include "../common/rapidjson/filereadstream.h"
#include <QDebug>
using namespace Hix::Settings;
void Hix::Settings::JSONParsedSetting::parseJSON(const std::filesystem::path& jsonPath)
{
	_jsonPath = jsonPath;
	parseJSON();
}

void Hix::Settings::JSONParsedSetting::parseJSON()
{
	//initialize to default values
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

//rapidjson::Value Hix::Settings::JSON::parseObj(const rapidjson::Document& doc, const std::string& key)
//{
//
//
//	auto& genVal = doc[key.c_str()];
//	if (genVal.IsObject())
//	{
//		char cbuf[1024]; rapidjson::MemoryPoolAllocator<> allocator(cbuf, sizeof cbuf);
//		rapidjson::Value subObject;
//		subObject.CopyFrom(genVal, allocator);
//
//		//for (rapidjson::Value::ConstMemberIterator  itr = input.MemberBegin(); itr != input.MemberEnd(); ++itr)
//		//{
//		//	rapidjson::Value val(itr->name);
//
//		//	out.AddMember(itr->name, itr->value, allocator);
//		//}
//
//		//out.SetObject();
//		////doc.AddMember("printerPresetPath", printerPresetPath, allocator);
//		//for (auto& m : genVal.GetObject())
//		//{
//		//	//out.AddMember(m.name.GetString(), m.value.GetString(), allocator);
//		//	out.AddMember("sfsdf", m.value, allocator);
//
//		//}
//
//
//		return genVal.GetObject();
//	}
//	else
//	{
//		throw std::runtime_error("failed to parse " + key);
//	}
//}

std::optional<rapidjson::Value> Hix::Settings::JSON::tryParseObj(const rapidjson::Document& doc, const std::string& key, rapidjson::MemoryPoolAllocator<>& allocator)
{
	try
	{
		if (doc.HasMember(key.c_str()))
		{
			auto& genVal = doc[key.c_str()];
			if (genVal.IsObject())
			{
				rapidjson::Value subObject;
				subObject.CopyFrom(genVal, allocator);
				return subObject;
			}
		}
	}
	catch (...)
	{
	}
	return std::optional<rapidjson::Value>();
}
template<>
bool Hix::Settings::JSON::tryParse<std::filesystem::path>(const rapidjson::Document& doc, const std::string& key, std::filesystem::path& value)
{
	std::string tmp;
	if (tryParse(doc, key, tmp))
	{
		value = tmp;
		return true;
	}
	return false;

}
template<>
void Hix::Settings::JSON::parse<std::filesystem::path>(const rapidjson::Document& doc, const std::string& key, std::filesystem::path& value)
{
	std::string tmp;
	parse(doc, key, tmp);
	value = tmp;
}