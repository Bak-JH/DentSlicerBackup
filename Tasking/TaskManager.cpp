#include "TaskManager.h"
#include <stdexcept>
#include "Qml/ProgressManager.h"
#include <qmlmanager.h>
using namespace Hix::Tasking;


TaskManager::TaskManager(): _taskThread(&TaskManager::run, this)
{
	_progressManager = new Hix::ProgressManager();
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
				_progressManager->deletePopup();
			}
			else if constexpr (std::is_same_v<T, Hix::Tasking::Task*>)
			{
				task->run();
				_progressManager->deletePopup();
			}
		}, taskVariant);
	}
}

void TaskManager::enqueTask(Hix::Tasking::Task* task)
{
	if (task != nullptr)
	{
		task->setProgressManager(*_progressManager);
		_queue.enqueue(task);
		_progressManager->generatePopup();
		_progressManager->draw();
	}
}

void Hix::Tasking::TaskManager::enqueTask(std::unique_ptr<Hix::Tasking::Task> task)
{
	task->setProgressManager(*_progressManager);
	_queue.enqueue(std::move(task));
	_progressManager->generatePopup();
	_progressManager->draw();
}

TaskManager::~TaskManager()
{
	_end = true;
	EmptyTask lastTaskflow;
	enqueTask(&lastTaskflow);
	_taskThread.join();
}

