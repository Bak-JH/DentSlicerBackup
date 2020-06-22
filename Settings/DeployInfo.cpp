#include "DeployInfo.h"
#include <QDebug>
#include <QCoreApplication>
#include "../Qt/QtUtils.h"
using namespace Hix::Settings::JSON;
using namespace Hix::Settings;
using namespace Hix::QtUtils;
Hix::Settings::DeployInfo::DeployInfo()
{
	auto qStrPath = QCoreApplication::applicationDirPath();
	qStrPath += "/deploy.json";
	parseJSON(toStdPath(qStrPath));
}

std::filesystem::path Hix::Settings::DeployInfo::printerPresetsDir() const
{
	return settingsDir/ "PrinterPresets";
}

void Hix::Settings::DeployInfo::initialize()
{
	auto qStrPath = QCoreApplication::applicationDirPath();
	settingsDir = toStdPath(qStrPath);
}

void Hix::Settings::DeployInfo::parseJSONImpl(const rapidjson::Document& doc)
{
	parse(doc, "version", version);
	tryParse(doc, "settingsDir", settingsDir);
}
