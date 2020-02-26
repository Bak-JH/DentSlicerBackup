#pragma once
#include "PrinterSetting.h"
#include "DeployInfo.h"
namespace Hix
{
	namespace Settings
	{
		class AppSetting:public JSONParsedSetting
		{
		public:
			AppSetting();
			~AppSetting();
			void refresh() override;
			void setPrinterPath(const std::string& path);
			bool enableErrorReport;
			std::string printerPresetPath;
			std::string version;
			PrinterSetting printerSetting;
			DeployInfo deployInfo;
		protected:
			void parseJSONImpl(const rapidjson::Document& doc)override;
			void initialize()override;
			void settingChanged();


		};
	}
}
