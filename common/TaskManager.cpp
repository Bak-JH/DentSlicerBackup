#include "TaskManager.h"
#include <stdexcept>
#include <qmlmanager.h>
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

void TaskManager::enqueUITask(std::function<void()> f)
{
	auto tf = new tf::Taskflow();
	tf->emplace([f]() {
		QmlManager::postToObject(f, qmlManager);
	});
	enqueTask(tf);
}

TaskManager::~TaskManager()
{
	_end = true;
	tf::Taskflow* lastTaskflow= new tf::Taskflow();
	auto task = lastTaskflow->emplace([]() {});
	enqueTask(lastTaskflow);
	_taskThread.join();
}
