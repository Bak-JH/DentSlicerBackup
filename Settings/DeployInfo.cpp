#include "DeployInfo.h"
#include <QDebug>
using namespace Hix::Settings::JSON;
using namespace Hix::Settings;

Hix::Settings::DeployInfo::DeployInfo()
{
	auto deployInfoPath = std::filesystem::current_path();
	deployInfoPath.append("deploy.json");
	parseJSON(deployInfoPath);

}

std::filesystem::path Hix::Settings::DeployInfo::printerPresetsDir() const
{
	std::filesystem::path presetPath = settingsDir + "/PrinterPresets";
	return presetPath;
}

void Hix::Settings::DeployInfo::initialize()
{
	settingsDir = std::filesystem::current_path().string();
}

void Hix::Settings::DeployInfo::parseJSONImpl(const rapidjson::Document& doc)
{
	parse(doc, "version", version);
	tryParse(doc, "settingsDir", settingsDir);
}
