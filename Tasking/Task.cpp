#include "Task.h"
#include "../application/ApplicationManager.h"

//template <typename F>
//static void postToObject(F&& fun, QObject* obj = qApp) {
//	QMetaObject::invokeMethod(obj, std::forward<F>(fun));
//}
//
//
//template <typename F>
//static void postToThread(F&& fun, QThread* thread = qApp->thread()) {
//	auto* obj = QAbstractEventDispatcher::instance(thread);
//	Q_ASSERT(obj);
//	QMetaObject::invokeMethod(obj, std::forward<F>(fun));
//}

Hix::Tasking::Task::~Task()
{
}


void Hix::Tasking::Task::postUIthread(std::function<void()>&& func)
{
	QMetaObject::invokeMethod(&Hix::Application::ApplicationManager::getInstance().engine(), func, Qt::BlockingQueuedConnection);
}

QObject* Hix::Tasking::Task::uiThreadObject() const
{
	return &Hix::Application::ApplicationManager::getInstance().engine();
}



void Hix::Tasking::Task::setProgressManager(Hix::ProgressManager& manager)
{
	_progressManager = &manager;
}

Hix::Progress* Hix::Tasking::Task::progress()
{
	return &_progress;
}

Hix::Tasking::UITask::UITask(std::function<void()> functor): _f(functor)
{

}

void Hix::Tasking::UITask::run()
{
	//QmlManager::postToObject(_f, qmlManager);

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
