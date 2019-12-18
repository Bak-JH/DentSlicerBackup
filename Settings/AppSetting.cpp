#include "AppSetting.h"
using namespace Hix::Settings;
Hix::Settings::AppSetting::AppSetting()
{
	_printerFile = std::filesystem::current_path();
	_printerFile.append("PrinterPresets/HixCapsule.json");
	refresh();
}

void Hix::Settings::AppSetting::refresh()
{

	_printerSetting.parseJSON(_printerFile);
}

void Hix::Settings::AppSetting::setPrinterPath(const std::string& path)
{
	_printerFile = path;
	refresh();
}

const PrinterSetting& Hix::Settings::AppSetting::printerSetting()const
{
	return _printerSetting;
}
