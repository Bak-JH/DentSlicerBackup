#pragma once
#include <functional>
#include "feature/interfaces/Progress.h"

namespace Hix
{
	class ProgressManager;
	class Progress;
	namespace Tasking
	{
		class Task
		{
		public:
			virtual void run() = 0;
			virtual ~Task();
			void setProgressManager(Hix::ProgressManager& manager);
			Progress* progress();

		protected:
			Hix::ProgressManager* _progressManager;
			Progress _progress;
		};

		class EmptyTask : public Task
		{
		public:
			EmptyTask();
			void run()override;
			virtual ~EmptyTask();
		};


		class UITask: public Task
		{
		public:
			UITask(std::function<void()> functor);
			void run()override;
			virtual ~UITask();
		private:
			std::function<void()> _f;
		};
	}
}
