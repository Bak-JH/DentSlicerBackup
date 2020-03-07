#pragma once
#include <filesystem>
#include "../common/rapidjson/document.h"
#include "../common/magic_enum.hpp"

#include <string>
#include <unordered_map>
#include <algorithm>
#include <optional>
#include <filesystem>
namespace Hix
{
	namespace Settings
	{

		namespace JSON
		{

			template<typename ValType>
			bool tryParse(const rapidjson::Document& doc, const std::string& key, ValType& value)
			{
				try
				{
					if (doc.HasMember(key.c_str()))
					{
						auto& genVal = doc[key.c_str()];
						if (genVal.Is<ValType>())
						{
							value = genVal.Get<ValType>();
							return true;
						}
					}

				}
				catch (...)
				{
					return false;
				}
				return false;

			}
			template<>
			bool tryParse<std::filesystem::path>(const rapidjson::Document& doc, const std::string& key, std::filesystem::path& value);

			template<typename ValType>
			void parse(const rapidjson::Document& doc, const std::string& key, ValType& value)
			{
				auto& genVal = doc[key.c_str()];
				if (genVal.Is<ValType>())
				{
					value = genVal.Get<ValType>();
				}
				else
				{
					throw std::runtime_error("failed to parse " + key);
				}
			}
			template<>
			void parse<std::filesystem::path>(const rapidjson::Document& doc, const std::string& key, std::filesystem::path& value);



			//assume lower case strings for mapping
			template<typename ValType>
			void tryParseStrToEnum(const rapidjson::Document& doc, const std::string& key, ValType& value)
			{
				try
				{
					if (doc.HasMember(key.c_str()))
					{
						auto& genVal = doc[key.c_str()];
						std::string str;
						if (genVal.Is<std::string>())
						{
							str = genVal.Get<std::string>();
							auto optEnum = magic_enum::enum_cast<ValType>(str);
							if (optEnum)
							{
								value = optEnum.value();
							}
						}
					}
				}
				catch (...)
				{
				}
			}
			template<typename ValType>
			void parseStrToEnum(const rapidjson::Document& doc, const std::string& key, ValType& value)
			{
				auto& genVal = doc[key.c_str()];
				std::string str;
				if (genVal.Is<std::string>())
				{
					str = genVal.Get<std::string>();
					value = magic_enum::enum_cast<ValType>(str).value();
					return;

				}
				throw std::runtime_error("failed to parse " + key);
			}
			//rapidjson::Value parseObj(const rapidjson::Document& doc, const std::string& key);
			std::optional<rapidjson::Value> tryParseObj(const rapidjson::Document& doc, const std::string& key, rapidjson::MemoryPoolAllocator<>& allocator);
		}
		class JSONParsedSetting
		{
		public:
			void parseJSON(const std::filesystem::path& jsonPath);
			virtual void parseJSON();
		protected:
			//set default values
			virtual void initialize() = 0;
			virtual void parseJSONImpl(const rapidjson::Document& doc) = 0;
			std::filesystem::path _jsonPath;
		};
	}
}