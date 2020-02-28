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
			void tryParse(const rapidjson::Document& doc, const std::string& key, ValType& value)
			{
				try
				{
					if (doc.HasMember(key.c_str()))
					{
						auto& genVal = doc[key.c_str()];
						if (genVal.Is<ValType>())
						{
							value = genVal.Get<ValType>();
						}
					}

				}
				catch (...)
				{
				}

			}
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

			//assume lower case strings for mapping
			template<typename ValType>
			void tryParseStrToEnum(const rapidjson::Document& doc, const std::string& key, ValType& value, const std::unordered_map<std::string, ValType>& map)
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
							std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return std::tolower(c); });
							auto enumResult = map.find(str);
							if (enumResult != map.cend())
							{
								value = enumResult->second;
							}
						}
					}
				}
				catch (...)
				{
				}
			}
			template<typename ValType>
			void parseStrToEnum(const rapidjson::Document& doc, const std::string& key, ValType& value, const std::unordered_map<std::string, ValType>& map)
			{
				auto& genVal = doc[key.c_str()];
				std::string str;
				if (genVal.Is<std::string>())
				{
					str = genVal.Get<std::string>();
					std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return std::tolower(c); });
					auto enumResult = map.find(str);
					if (enumResult != map.cend())
					{
						value = enumResult->second;
						return;
					}
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
			virtual void refresh();
		protected:
			virtual void initialize() = 0;
			virtual void parseJSONImpl(const rapidjson::Document& doc) = 0;
			std::filesystem::path _jsonPath;
		};
	}
}