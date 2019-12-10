#pragma once
#include <deque>
#include <memory>
#include "feature/interfaces/Feature.h"

namespace Hix
{
	namespace Features
	{
		class FeatureHisroyManager
		{
		public:
			FeatureHisroyManager();
			void addFeature(Hix::Features::Feature* feature);
			void undo();
			void redo();

		private:
			std::deque<std::unique_ptr<Hix::Features::Feature>> _history;
			std::deque<std::unique_ptr<Hix::Features::Feature>>::iterator _itr;
		};
	}
}