#pragma once
#include <string>
#include "JSONParsedSetting.h"
#include "../DentEngine/src/Bounds3D.h"
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
			std::string settingsDir;
			std::filesystem::path printerPresetsDir()const;

		protected:
			void initialize()override;
			void parseJSONImpl(const rapidjson::Document& doc)override;

		};
	}
}
