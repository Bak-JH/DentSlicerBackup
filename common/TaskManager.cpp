#include "TaskManager.h"
#include <stdexcept>

TaskManager::TaskManager(): _taskThread(&TaskManager::run, this)
{
}

void TaskManager::run()
{
	tf::Taskflow* currentTask = nullptr;
	while (!_end)
	{
		_queue.wait_dequeue(currentTask);
		_executor.run(*currentTask).wait();
		delete currentTask;
	}
}

void TaskManager::enqueTask(tf::Taskflow* task)
{
	_queue.enqueue(task);
}

TaskManager::~TaskManager()
{
	_end = true;
	tf::Taskflow* lastTaskflow= new tf::Taskflow();
	auto task = lastTaskflow->emplace([]() {});
	enqueTask(lastTaskflow);
	_taskThread.join();
}
