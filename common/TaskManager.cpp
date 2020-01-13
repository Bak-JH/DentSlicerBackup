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
			bool success = false;
			//for small copy by value tasks and larger copy by pointer tasks
			if constexpr (std::is_same_v<T, Hix::Tasking::Task>)
			{
				success = tryTask(task);
			}
			else if constexpr (std::is_same_v<T, Hix::Tasking::Task*>)
			{
				success = tryTask(*task);
				if (!success)
					delete task;
			}
		}, taskVariant);
	}
}

void TaskManager::enqueTask(Task* task)
{
	if (task != nullptr)
	{
		_queue.enqueue(task);
	}
}

void Hix::Tasking::TaskManager::enqueTask(const Hix::Tasking::Task& task)
{
	_queue.enqueue(task);
}

TaskManager::~TaskManager()
{
	_end = true;
	EmptyTask lastTaskflow;
	enqueTask(&lastTaskflow);
	_taskThread.join();
}

bool Hix::Tasking::TaskManager::tryTask(Hix::Tasking::Task& task)
{
	bool success = false;
	try
	{
		task.run();
		success = true;
	}
	catch (std::exception & e)
	{
		//popup, log, send error report
	}
	catch (...)
	{
		//incase of custom unknown exception, gracefully exit
	}
	//if feature add to history
	if (success)
	{
		auto feature = dynamic_cast<Hix::Features::Feature*>(&task);
		if (feature)
		{
			qmlManager->featureHistoryManager().addFeature(feature);
		}
	}
	return success;
}
