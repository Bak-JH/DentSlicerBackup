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

		namespace JSON
		{

			template<typename ValType>
			void writeVal(const ValType& val, )
			{

			}
		}
		class JSONWriteSetting
		{
		public:
			virtual void writeJSON();
		protected:
			virtual const std::filesystem::path& jsonPath() = 0;
			virtual rapidjson::Document doc() = 0;

		};
	}
}