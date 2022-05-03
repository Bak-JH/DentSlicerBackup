#include "AppSetting.h"
#include <fstream>
#include "../common/rapidjson/stringbuffer.h"
#include "../common/rapidjson/PrettyWriter.h"
#include "../common/rapidjson/ostreamwrapper.h"
#include "../application/ApplicationManager.h"

using namespace Hix::Settings;
using namespace Hix::Settings::JSON;
typedef rapidjson::GenericStringBuffer<rapidjson::UTF8<>, rapidjson::MemoryPoolAllocator<>> StringBuffer;

Hix::Settings::AppSetting::AppSetting()
{

}

Hix::Settings::AppSetting::~AppSetting()
{
}

void Hix::Settings::AppSetting::parse()
{
	deployInfo.parseJSON();
	basicSetting.setWritePath(basicSetting.jsonPath());
	sliceSetting.setWritePath(sliceSetting.jsonPath());
	supportSetting.setWritePath(supportSetting.jsonPath());
	labelSetting.setWritePath(labelSetting.jsonPath());
	basicSetting.parseJSON();
	sliceSetting.parseJSON();
	supportSetting.parseJSON();
	labelSetting.parseJSON();
}

void Hix::Settings::AppSetting::toDefault()
{
	basicSetting.toDefault();
	sliceSetting.toDefault();
	supportSetting.toDefault();
	labelSetting.toDefault();
	basicSetting.writeJSON();
	sliceSetting.writeJSON();
	supportSetting.writeJSON();
	labelSetting.writeJSON();

}



