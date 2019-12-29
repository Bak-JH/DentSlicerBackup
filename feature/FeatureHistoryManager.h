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
			//gets the latest feature
			const Hix::Features::Feature& peek()const;
			bool empty()const;
			//remove latest feature
			void pop();
			void undo();
			void redo();

		private:
			std::deque<std::unique_ptr<Hix::Features::Feature>> _history;
			std::deque<std::unique_ptr<Hix::Features::Feature>>::iterator _itr;
		};
	}
}