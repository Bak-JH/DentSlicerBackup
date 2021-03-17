#pragma once
#include <filesystem>
#include "../common/rapidjson/document.h"
#include <string>
#include <unordered_map>
#include <algorithm>
#include <optional>
namespace Hix
{
	namespace Settings
	{

		class JSONWriteSetting
		{
		public:
			void setWritePath(std::filesystem::path writeJson);
			virtual void writeJSON();
		protected:
			virtual rapidjson::Document doc() = 0;
			std::filesystem::path _writeJsonPath;

		};
	}
}