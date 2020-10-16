#pragma once
#include <filesystem>
#include "../common/rapidjson/document.h"
#include "../common/rapidjson/writer.h"
#include "../common/rapidjson/stringbuffer.h"
#include "../common/rapidjson/PrettyWriter.h"
#include <string>
#include <unordered_map>
#include <algorithm>
#include <optional>
#include <memory>

//namespace rapidjson
//{
//	class StringBuffer;
//}
namespace Hix
{
	namespace Settings
	{


		class JSONWriteSetting
		{
		public:
			virtual void writeJSON();
		protected:
			virtual const std::filesystem::path& jsonPath() = 0;
			virtual void writeObjects() = 0;
			std::unique_ptr< rapidjson::StringBuffer> _workingBuffer;

			//unlike parse, it's a member function
			template<typename ValType>
			void writeVal(const std::string& key, const ValType& val)
			{
				rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(_workingBuffer);
				writer.write
			}
		};
	}
}