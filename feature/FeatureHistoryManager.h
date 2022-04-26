#pragma once
#include <deque>
#include <memory>
#include "feature/interfaces/Feature.h"

namespace Hix
{
	namespace Features
	{
		class FeatureHistoryManager
		{
		public:
			FeatureHistoryManager();
			void addFeature(Hix::Features::Feature* feature);
			void historySizeChanged(int newSize);
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