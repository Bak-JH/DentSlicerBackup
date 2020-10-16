#include "JSONWriteSetting.h"
#include "../common/rapidjson/writer.h"
#include "../common/rapidjson/stringbuffer.h"
#include "../common/rapidjson/PrettyWriter.h"
#include "../common/rapidjson/ostreamwrapper.h"
#include "../common/rapidjson/stringbuffer.h"

#include <fstream>
#include <QDebug>


void Hix::Settings::JSONWriteSetting::writeJSON()
{

	_workingBuffer.reset(new rapidjson::StringBuffer());
	rapidjson::PrettyWriter<rapidjson::StringBuffer> objWriter;
	objWriter.StartObject();
	writeObjects();
	objWriter.EndObject();

	std::ofstream of(jsonPath(), std::ios_base::trunc);
	std::string json(_workingBuffer->GetString(), _workingBuffer->GetSize());
	of << json;
}
