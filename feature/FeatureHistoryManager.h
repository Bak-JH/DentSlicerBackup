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
			void addFeature(Hix::Features::Feature* feature);
			void undo();

		private:
			std::deque<std::unique_ptr<Hix::Features::Feature>> _history;
		};
	}
}