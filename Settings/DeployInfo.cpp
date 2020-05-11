#include "DeployInfo.h"
#include <QDebug>
#include <QCoreApplication>
using namespace Hix::Settings::JSON;
using namespace Hix::Settings;

Hix::Settings::DeployInfo::DeployInfo()
{
	auto deployInfoPath = QCoreApplication::applicationDirPath();
	deployInfoPath.append("deploy.json");
	parseJSON(deployInfoPath.toStdString());
}

std::filesystem::path Hix::Settings::DeployInfo::printerPresetsDir() const
{
	std::filesystem::path presetPath = settingsDir / "PrinterPresets";
	return presetPath;
}

void Hix::Settings::DeployInfo::initialize()
{
	settingsDir = std::filesystem::current_path();
}

void Hix::Settings::DeployInfo::parseJSONImpl(const rapidjson::Document& doc)
{
	parse(doc, "version", version);
	tryParse(doc, "settingsDir", settingsDir);
}
