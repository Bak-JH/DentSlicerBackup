#include "PrinterSetting.h"
#include <QDebug>
using namespace Hix::Settings::JSON;
using namespace Hix::Settings;
constexpr size_t MAX_NAME_LEN = 200;

const std::unordered_map<std::string, PrinterSetting::InfoFileType> __infoFileMap
	{ {"hix", PrinterSetting::InfoFileType::Hix},{"3delight", PrinterSetting::InfoFileType::ThreeDelight} };
const std::unordered_map<std::string, PrinterSetting::BedShape> __bedShapeMap
	{ {"circle", PrinterSetting::BedShape::Circle},{"rectangle", PrinterSetting::BedShape::Rect} };


float Hix::Settings::PrinterSetting::pixelPerMMX() const
{
	return sliceImageResolutionX / screenX;;
}

float Hix::Settings::PrinterSetting::pixelPerMMY() const
{
	return sliceImageResolutionY / screenY;
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
	tryParseStrToEnum(doc, "infoFileType", infoFileType, __infoFileMap);
	parse(doc, "sliceImageResolutionX", sliceImageResolutionX);
	parse(doc, "sliceImageResolutionY", sliceImageResolutionY);
	parse(doc, "screenX", screenX);
	parse(doc, "screenY", screenY);
	parse(doc, "bedHeight", bedHeight);
	parseStrToEnum(doc, "bedShape", bedShape, __bedShapeMap);
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

}
