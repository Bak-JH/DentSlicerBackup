#pragma once
#include "PrinterSetting.h"
#include "DeployInfo.h"
#include "JSONWriteSetting.h"
#include "SliceSetting.h"
#include "SupportSetting.h"
#include "AdditionalSetting.h"

namespace Hix
{
	namespace Settings
	{
		enum Liscense {
			NONE,
			BASIC,
			PRO
		};

		class AppSetting:public JSONParsedSetting, public JSONWriteSetting
		{
		public:
			AppSetting();
			~AppSetting();
			void parseJSON() override;
			void settingChanged();
			void writeJSON()override;
			bool enableErrorReport;
			DeployInfo deployInfo;
			SliceSetting sliceSetting;
			SupportSetting supportSetting;
			PrinterSetting printerSetting;
			AdditionalSetting additionalSetting;
			std::string printerPresetPath;
			std::string version;
			Liscense liscense;

		protected:
			void parseJSONImpl(const rapidjson::Document& doc)override;
			void initialize()override;
			const std::filesystem::path& jsonPath()override;
			rapidjson::Document doc()override;


		};
	}
}
