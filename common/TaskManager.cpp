#include "TaskManager.h"
#include <stdexcept>
#include <qmlmanager.h>
using namespace Hix::Tasking;


TaskManager::TaskManager(): _taskThread(&TaskManager::run, this)
{
}

void TaskManager::run()
{
	Task* currentTask = nullptr;
	while (!_end)
	{
		_queue.wait_dequeue(currentTask);
		_executor.run(currentTask->getFlow()).wait();
		delete currentTask;
	}
}

void TaskManager::enqueTask(Task* task)
{
	if (task != nullptr)
	{
		_queue.enqueue(task);
	}
}

void TaskManager::enqueUITask(std::function<void()> f)
{
	auto uiTask = new UITask(f);
	enqueTask(uiTask);
}

TaskManager::~TaskManager()
{
	_end = true;
	auto lastTaskflow= new GenericTask();
	enqueTask(lastTaskflow);
	_taskThread.join();
}
