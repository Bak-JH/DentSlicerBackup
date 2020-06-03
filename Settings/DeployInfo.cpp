#include "DeployInfo.h"
#include <QDebug>
#include <QCoreApplication>
using namespace Hix::Settings::JSON;
using namespace Hix::Settings;

Hix::Settings::DeployInfo::DeployInfo()
{
	auto qStrPath = QCoreApplication::applicationDirPath();
	qStrPath += "deploy.json";
	auto* u16Path = reinterpret_cast<const wchar_t*>(qStrPath.utf16());
	
	parseJSON(std::filesystem::path(u16Path, u16Path + qStrPath.size()));
}

std::filesystem::path Hix::Settings::DeployInfo::printerPresetsDir() const
{
	auto qStrPath = QCoreApplication::applicationDirPath();
	qStrPath += "/PrinterPresets";
	auto* u16Path = reinterpret_cast<const wchar_t*>(qStrPath.utf16());

	return std::filesystem::path(u16Path, u16Path + qStrPath.size());;
}

void Hix::Settings::DeployInfo::initialize()
{
	auto qStrPath = QCoreApplication::applicationDirPath();
	auto* u16Path = reinterpret_cast<const wchar_t*>(qStrPath.utf16());
	settingsDir = std::filesystem::path(u16Path, u16Path + qStrPath.size());
}

void Hix::Settings::DeployInfo::parseJSONImpl(const rapidjson::Document& doc)
{
	parse(doc, "version", version);
	tryParse(doc, "settingsDir", settingsDir);
}
