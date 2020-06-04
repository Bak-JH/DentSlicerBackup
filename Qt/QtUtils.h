#pragma once
#include <filesystem>
#include <qstring.h>

namespace Hix
{
	namespace QtUtils
	{
		//only for wstring, utf16 path strings
		inline std::filesystem::path toStdPath(QString path)
		{
			auto* u16Path = reinterpret_cast<const wchar_t*>(path.utf16());
			return std::filesystem::path(u16Path, u16Path + path.size());
		}
	}
}