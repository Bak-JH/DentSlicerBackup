#include "DeployInfo.h"
#include <QDebug>
#include <QCoreApplication>
#include "../Qt/QtUtils.h"
using namespace Hix::Settings::JSON;
using namespace Hix::Settings;
using namespace Hix::QtUtils;
Hix::Settings::DeployInfo::DeployInfo():JSONParsedSetting("deploy.json"), defaultsDir(toStdPath(QCoreApplication::applicationDirPath()))
{
}


void Hix::Settings::DeployInfo::initialize()
{
}

std::filesystem::path Hix::Settings::DeployInfo::jsonPath()
{
	return defaultPath();
}

void Hix::Settings::DeployInfo::parseJSONImpl(const rapidjson::Document& doc)
{
	parse(doc, "version", version);
	tryParse(doc, "settingsDir", settingsDir);
}
