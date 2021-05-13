#include "LocalSetting.h"
#include <fstream>
#include "../common/rapidjson/stringbuffer.h"
#include "../common/rapidjson/PrettyWriter.h"
#include "../common/rapidjson/ostreamwrapper.h"
#include "../application/ApplicationManager.h"

using namespace Hix::Settings;
using namespace Hix::Settings::JSON;

const std::filesystem::path LocalSetting::localSettingsDir = "LocalSettings";


Hix::Settings::LocalSetting::LocalSetting(const std::string& settingName): JSONParsedSetting((localSettingsDir / settingName).string())
{
	//auto settingPath = localSettingsDir;
	//settingPath.append(settingName);
	//create if doesn't exist
	auto jsp = jsonPath();
	if (!std::filesystem::is_regular_file(jsp))
	{
		std::error_code ec;
		std::filesystem::create_directory(Hix::Application::ApplicationManager::getInstance().settings().deployInfo.settingsDir / localSettingsDir, ec);

		
		std::ofstream ofs(jsp);
		ofs.close();
	}

	setWritePath(jsp);

}

Hix::Settings::LocalSetting::~LocalSetting()
{
}
