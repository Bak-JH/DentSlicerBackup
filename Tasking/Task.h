#pragma once
#include <functional>

namespace Hix
{
	namespace Tasking
	{
		class Task
		{
		public:
			virtual void run() = 0;
			virtual ~Task();
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
