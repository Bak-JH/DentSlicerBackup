#include "Task.h"
#include "../qmlmanager.h"
Hix::Tasking::Task::~Task()
{
}

Hix::Tasking::UITask::UITask(std::function<void()> functor)
{

	_flow.emplace([functor]() {
		QmlManager::postToObject(functor, qmlManager);
		});	
}

tf::Taskflow& Hix::Tasking::UITask::getFlow()
{
	return _flow;
}

Hix::Tasking::UITask::~UITask()
{
}

tf::Taskflow& Hix::Tasking::GenericTask::getFlow()
{
	return _flow;
}

Hix::Tasking::GenericTask::~GenericTask()
{
}
