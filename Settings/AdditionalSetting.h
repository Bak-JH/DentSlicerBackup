#pragma once
#include "JSONParsedSetting.h"
#include "JSONWriteSetting.h"
#include <unordered_map>
namespace Hix
{
	namespace Settings
	{
		class AdditionalSetting :public JSONParsedSetting, public JSONWriteSetting
		{
		public:

			AdditionalSetting(std::filesystem::path settingsPath);
			~AdditionalSetting();
			std::unordered_map<std::string, std::string> keyVals;



		protected:
			void parseJSONImpl(const rapidjson::Document& doc)override;
			void initialize()override;
			const std::filesystem::path& jsonPath()override;
			rapidjson::Document doc()override;
		};
	}
}
