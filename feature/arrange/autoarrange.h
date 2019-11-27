#pragma once
#include <unordered_set>
class GLModel;
namespace Hix
{
	namespace Features
	{
		class Autoarrange
		{
		public:
			Autoarrange(const std::unordered_set<GLModel*>& selected);
		};
	}
}
