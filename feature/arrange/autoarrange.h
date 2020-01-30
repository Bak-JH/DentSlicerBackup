#pragma once
#include <unordered_set>
#include "../interfaces/Feature.h"
#include "../interfaces/Mode.h"
class GLModel;
namespace Hix
{
	namespace Features
	{
		class AutoArrangeMode : public Hix::Features::Mode
		{
		public:
			AutoArrangeMode();
			virtual ~AutoArrangeMode();
		private:
		};

		class AutoArrange : public Hix::Features::FeatureContainer
		{
		public:
			AutoArrange(const std::unordered_set<GLModel*>& selected);
		protected:
			void runImpl()override;
		private:
			std::unordered_set<GLModel*> _selected;
		};

	}
}
