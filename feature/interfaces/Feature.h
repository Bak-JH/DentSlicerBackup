#pragma once
#include <unordered_set>
class GLModel;
namespace Hix
{
	namespace Features
	{


		class Feature
		{
		public:
			Feature();
			Feature(const std::unordered_set<GLModel*>& subjects);
			virtual ~Feature();
			virtual void undo() = 0;
		protected:
			std::unordered_set<GLModel*> _subjects;

		};
	}
}