#include "JSONWriteSetting.h"
#include "../common/rapidjson/writer.h"
#include "../common/rapidjson/stringbuffer.h"
#include "../common/rapidjson/PrettyWriter.h"
#include "../common/rapidjson/ostreamwrapper.h"
#include <fstream>
#include <QDebug>

void Hix::Settings::JSONWriteSetting::writeJSON()
{
	//char cbuf[1024]; rapidjson::MemoryPoolAllocator<> allocator(cbuf, sizeof cbuf);
	//rapidjson::Document doc(&allocator, 256);
	//doc.SetObject();
	////doc.AddMember("printerPresetPath", printerPresetPath, allocator);
	////doc.AddMember("enableErrorReport", enableErrorReport, allocator);
	//doc.AddMember("printerPresetPath", printerPresetPath, doc.GetAllocator());
	//doc.AddMember("enableErrorReport", enableErrorReport, doc.GetAllocator());
	auto document = doc();
	std::ofstream of(jsonPath(), std::ios_base::trunc);
	rapidjson::OStreamWrapper osw{ of };
	rapidjson::PrettyWriter<rapidjson::OStreamWrapper> writer{ osw };
	writer.SetMaxDecimalPlaces(7);
	document.Accept(writer);
}
