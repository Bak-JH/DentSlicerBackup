#pragma once
#include <list>
#include <array>
#include <memory>
#include <thread>
#include <functional>
#include "Task.h"
#include "moodycamel/readerwriterqueue.h"


namespace Hix
{
	namespace Tasking
	{
		class TaskManager
		{
		public:
			TaskManager();
			void run();
			void enqueTask(Hix::Tasking::Task* task);
			void enqueUITask(std::function<void()> f);
			~TaskManager();
		private:
			tf::Executor _executor;
			moodycamel::BlockingReaderWriterQueue<Hix::Tasking::Task*> _queue;
			bool _end = false;
			std::thread _taskThread;
		};
	}
}
