#pragma once
#include <list>
#include <array>
#include <memory>
#include <thread>
#include <functional>
#include "Task.h"
#include "../common/moodycamel/readerwriterqueue.h"
#include <variant>

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
			void enqueTask(const Hix::Tasking::Task& task);

			~TaskManager();
		private:
			using TaskVariant = std::variant<Hix::Tasking::Task, Hix::Tasking::Task*>;
			bool tryTask(Hix::Tasking::Task& task);

			moodycamel::BlockingReaderWriterQueue<TaskVariant> _queue;
			bool _end = false;
			std::thread _taskThread;
		};
	}
}
