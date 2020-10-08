#include "TaskManager.h"
#include <stdexcept>
#include <sstream>
#include <iostream>
#include "../feature/sendError.h"
#include "application/ApplicationManager.h"
using namespace Hix::Tasking;


TaskManager::TaskManager(): _taskThread(&TaskManager::run, this), _progressManager()
{
}

void TaskManager::run()
{
	auto& engine = Hix::Application::ApplicationManager::getInstance().engine();
	TaskVariant taskVariant = nullptr;
	while (true)
	{
		bool isError = false;
		_queue.wait_dequeue(taskVariant);
		if (_end)
			return;
		Hix::Tasking::Task* rTask;
		std::visit([this, &rTask](auto&& task) {
			using T = std::decay_t<decltype(task)>;
			//for small copy by value tasks and larger copy by pointer tasks
			if constexpr (std::is_same_v<T, std::unique_ptr<Hix::Tasking::Task>>)
			{
				rTask = task.get();
			}
			else if constexpr (std::is_same_v<T, Hix::Tasking::Task*>)
			{
				rTask = task;
			}
		}, taskVariant);

		//temp
		if (!dynamic_cast<Hix::Features::SendError*>(rTask))
		{
			QMetaObject::invokeMethod(&engine, [this]() {
				_progressManager.generatePopup();
				}, Qt::BlockingQueuedConnection);
		}


		auto container = dynamic_cast<Hix::Features::FeatureContainer*>(rTask);
		if (container && container->progress()->getDisplayText() == "")
		{
			for (auto& each : container->getContainer())
			{
				QMetaObject::invokeMethod(&engine, [this, &each]() {
					_progressManager.addProgress(each->progress());
					}, Qt::BlockingQueuedConnection);
			}
		}
		else
		{
			QMetaObject::invokeMethod(&engine, [this, &rTask]() {
				_progressManager.addProgress(rTask->progress());
				}, Qt::BlockingQueuedConnection);
		}
		try
		{
			rTask->run();
		}
		catch (std::exception & e)
		{
			qDebug() << e.what();
			//popup, log, send error report

			QMetaObject::invokeMethod(&engine, [this]() {Hix::Application::ApplicationManager::getInstance().modalDialogManager().openOkCancelDialog(
				"Error has occured. Do you report this error?", "Report Error", "Ignore",
				[this]() {
					//report error pressed
					Hix::Application::ApplicationManager::getInstance().modalDialogManager().closeDialog();
					_progressManager.generateErrorPopup("Reporting Error");
					Hix::Application::ApplicationManager::getInstance().taskManager().enqueTask(new Hix::Features::SendError("title", "details"));
				},
				[this]() {
					Hix::Application::ApplicationManager::getInstance().modalDialogManager().closeDialog();
					_progressManager.deletePopup();
				}
				);
				}, Qt::BlockingQueuedConnection);

			isError = true;
		}
		catch (...)
		{
			//incase of custom unknown exception, gracefully exit
		}
		if (!isError)
		{
			QMetaObject::invokeMethod(&engine, [this, &rTask]() {
				_progressManager.deletePopup();
				}, Qt::BlockingQueuedConnection);
		}
	}
}

void TaskManager::enqueTask(Hix::Tasking::Task* task)
{
	if (task != nullptr)
	{
		task->setProgressManager(_progressManager);
		_queue.enqueue(task);
	}
}

void Hix::Tasking::TaskManager::enqueTask(std::unique_ptr<Hix::Tasking::Task> task)
{
	_queue.enqueue(std::move(task));
}

Hix::ProgressManager& Hix::Tasking::TaskManager::progressManager()
{
	return _progressManager;
}

TaskManager::~TaskManager()
{
	_end = true;
	EmptyTask lastTaskflow;
	enqueTask(&lastTaskflow);
	_taskThread.join();
}

