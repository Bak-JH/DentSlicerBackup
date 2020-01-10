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

void Hix::Settings::DeployInfo::initialize()
{
}

void Hix::Settings::DeployInfo::parseJSONImpl(const rapidjson::Document& doc)
{
	parse(doc, "version", version);
	parse(doc, "settingsDir", settingsDir);
}
