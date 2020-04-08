#pragma once
#include <list>
#include <array>
#include <memory>
#include <thread>
#include <functional>
#include "Task.h"
#include "../common/moodycamel/readerwriterqueue.h"
#include <variant>
#include "Qml/ProgressManager.h"

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
			void enqueTask(std::unique_ptr<Hix::Tasking::Task> task);
			Hix::ProgressManager& progressManager();
			~TaskManager();
		private:
			using TaskVariant = std::variant<std::unique_ptr<Hix::Tasking::Task>, Hix::Tasking::Task*>;

			moodycamel::BlockingReaderWriterQueue<TaskVariant> _queue;
			bool _end = false;
			std::thread _taskThread;
			Hix::ProgressManager _progressManager;
		};
	}
}
