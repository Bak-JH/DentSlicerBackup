#include "Task.h"
#include "../qmlmanager.h"

Hix::Tasking::Task::~Task()
{
}

void Hix::Tasking::Task::setProgressManager(Hix::ProgressManager& manager)
{
	_progressManager = &manager;
}

Hix::Tasking::UITask::UITask(std::function<void()> functor): _f(functor)
{

}

void Hix::Tasking::UITask::run()
{
	QmlManager::postToObject(_f, qmlManager);

}


Hix::Tasking::UITask::~UITask()
{
}


Hix::Tasking::EmptyTask::EmptyTask()
{
}

void Hix::Tasking::EmptyTask::run()
{
}

Hix::Tasking::EmptyTask::~EmptyTask()
{
}
