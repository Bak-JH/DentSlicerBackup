#pragma once
#include "JSONParsedSetting.h"
#include "JSONWriteSetting.h"
#include <unordered_map>
namespace Hix
{
	namespace Settings
	{
		/// <summary>
		///  Locally created setting, these settings are created dynamically on the installed computer, not deployed like other settings
		///  Is virutal, so override to use, acts as a guard, writing to file during deletion
		/// </summary>
		class LocalSetting :public JSONParsedSetting, public JSONWriteSetting
		{
		public:
			LocalSetting(const std::string& settingName);
			virtual ~LocalSetting();
		protected:
			static const std::filesystem::path  localSettingsDir;

		};
	}
}
