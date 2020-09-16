#include "LocalSetting.h"
#include <fstream>
#include "../common/rapidjson/stringbuffer.h"
#include "../common/rapidjson/PrettyWriter.h"
#include "../common/rapidjson/ostreamwrapper.h"
#include "../application/ApplicationManager.h"

using namespace Hix::Settings;
using namespace Hix::Settings::JSON;



Hix::Settings::LocalSetting::LocalSetting(const std::string& settingName)
{
	auto settingPath = Hix::Application::ApplicationManager::getInstance().settings().deployInfo.localSettingsDir();
	settingPath.append(settingName);
	//create if doesn't exist
	if (!std::filesystem::is_regular_file(settingPath))
	{
		std::ofstream ofs(settingPath);
		ofs.close();
	}
	_jsonPath = settingPath;
}

Hix::Settings::LocalSetting::~LocalSetting()
{
}



const std::filesystem::path& Hix::Settings::LocalSetting::jsonPath()
{
	return _jsonPath;
}

