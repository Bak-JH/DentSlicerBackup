#pragma once
#include "PrinterSetting.h"
#include "DeployInfo.h"
#include "JSONWriteSetting.h"
namespace Hix
{
	namespace Settings
	{
		class AppSetting:public JSONParsedSetting, public JSONWriteSetting
		{
		public:
			AppSetting();
			~AppSetting();
			void parseJSON() override;
			void setPrinterPath(const std::string& path);
			void settingChanged();
			bool enableErrorReport;
			std::string printerPresetPath;
			std::string version;
			PrinterSetting printerSetting;
			DeployInfo deployInfo;
		protected:
			void parseJSONImpl(const rapidjson::Document& doc)override;
			void initialize()override;
			const std::filesystem::path& jsonPath()override;
			rapidjson::Document doc()override;


		};
	}
}
