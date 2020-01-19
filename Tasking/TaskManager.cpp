#include "TaskManager.h"
#include <stdexcept>
#include <qmlmanager.h>
using namespace Hix::Tasking;


TaskManager::TaskManager(): _taskThread(&TaskManager::run, this)
{
}

void TaskManager::run()
{
	TaskVariant taskVariant = nullptr;
	while (!_end)
	{
		_queue.wait_dequeue(taskVariant);
		std::visit([this](auto&& task) {
			using T = std::decay_t<decltype(task)>;
			//for small copy by value tasks and larger copy by pointer tasks
			if constexpr (std::is_same_v<T, std::unique_ptr<Hix::Tasking::Task>>)
			{
				task->run();
			}
			else if constexpr (std::is_same_v<T, Hix::Tasking::Task*>)
			{
				task->run();
			}
		}, taskVariant);
	}
}

void TaskManager::enqueTask(Hix::Tasking::Task* task)
{
	if (task != nullptr)
	{
		_queue.enqueue(task);
	}
}

void Hix::Tasking::TaskManager::enqueTask(std::unique_ptr<Hix::Tasking::Task> task)
{
	_queue.enqueue(std::move(task));
}

TaskManager::~TaskManager()
{
	_end = true;
	EmptyTask lastTaskflow;
	enqueTask(&lastTaskflow);
	_taskThread.join();
}

