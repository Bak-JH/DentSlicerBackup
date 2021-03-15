#include "PrinterSetting.h"
#include "../application/ApplicationManager.h"
#include <QDebug>
using namespace Hix::Settings::JSON;
using namespace Hix::Settings;
constexpr size_t MAX_NAME_LEN = 200;

const std::filesystem::path printerPresetFolder = "PrinterPresets";

Hix::Settings::PrinterSetting::PrinterSetting(): _allocator(&_buffer, _buffer.size())
{
}

std::filesystem::path Hix::Settings::PrinterSetting::printerPresetsPath()const
{
	return Hix::Application::ApplicationManager::getInstance().settings().deployInfo.settingsDir / printerPresetFolder;
}

double Hix::Settings::PrinterSetting::pixelPerMMX() const
{
	return sliceImageResolutionX / screenX;
}

double Hix::Settings::PrinterSetting::pixelPerMMY() const
{
	return sliceImageResolutionY / screenY;
}

double Hix::Settings::PrinterSetting::pixelSizeX() const
{
	return screenX/ sliceImageResolutionX;
}

double Hix::Settings::PrinterSetting::pixelSizeY() const
{
	return screenY/ sliceImageResolutionY;
}

void Hix::Settings::PrinterSetting::setPrinterPreset(std::string presetName)
{
	auto presetdir = printerPresetFolder / presetName;
	setJsonName(presetdir.string());
	parseJSON();
	Hix::Application::ApplicationManager::getInstance().sceneManager().drawBed();

}

void Hix::Settings::PrinterSetting::initialize()
{
	//default values
	infoFileType = InfoFileType::Hix;
	bedOffsetX = 0.0f;
	bedOffsetY = 0.0f;
	bedBound.reset();
	bedBound.setZMin(0.0f);

}

void Hix::Settings::PrinterSetting::parseJSONImpl(const rapidjson::Document& doc)
{
	parse(doc, "presetName", presetName);
	tryParseStrToEnum(doc, "infoFileType", infoFileType);
	parse(doc, "sliceImageResolutionX", sliceImageResolutionX);
	parse(doc, "sliceImageResolutionY", sliceImageResolutionY);
	parse(doc, "screenX", screenX);
	parse(doc, "screenY", screenY);
	parse(doc, "bedHeight", bedHeight);
	parseStrToEnum(doc, "bedShape", bedShape);
	tryParse(doc, "bedOffsetX", bedOffsetX);
	tryParse(doc, "bedOffsetY", bedOffsetY);
	bedBound.setZMax(bedHeight);
	if (bedShape == BedShape::Circle)
	{
		parse(doc, "bedRadius", bedRadius);
		bedBound.setXLength(bedRadius * 2);
		bedBound.setYLength(bedRadius * 2);
	}
	else
	{
		parse(doc, "bedX", bedX);
		parse(doc, "bedY", bedY);
		bedBound.setXLength(bedX);
		bedBound.setYLength(bedY);
	}
	printerConstants = tryParseObj(doc, "printerConstants", _allocator);
}
