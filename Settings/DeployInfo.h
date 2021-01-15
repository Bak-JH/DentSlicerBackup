#pragma once
#include <string>
#include "JSONParsedSetting.h"
#include "../render/Bounds3D.h"
#include <filesystem>
namespace Hix
{
	namespace Settings
	{
		class DeployInfo:public JSONParsedSetting
		{
		public:
			DeployInfo();
			std::string version;
			std::filesystem::path settingsDir;
			std::filesystem::path localSettingsDir()const;
			std::filesystem::path printerPresetsDir()const;

		protected:
			void initialize()override;
			void parseJSONImpl(const rapidjson::Document& doc)override;

		};
	}
}
