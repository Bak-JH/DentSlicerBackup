#include "TaskManager.h"
#include <stdexcept>
#include <sstream>
#include <iostream>

#include "application/ApplicationManager.h"
using namespace Hix::Tasking;


TaskManager::TaskManager(): _taskThread(&TaskManager::run, this), _progressManager()
{
}

void TaskManager::run()
{
	TaskVariant taskVariant = nullptr;
	while (!_end)
	{
		_queue.wait_dequeue(taskVariant);

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

		_progressManager.generatePopup();

		auto container = dynamic_cast<Hix::Features::FeatureContainer*>(rTask);
		if (container && container->progress()->getDisplayText() == "")
		{
			for (auto& each : container->getContainer())
			{
				_progressManager.addProgress(each->progress());
			}
		}
		else
		{
			_progressManager.addProgress(rTask->progress());
		}

		try
		{
			rTask->run();
		}
		catch (std::exception & e)
		{
			qDebug() << e.what();
			//popup, log, send error report
			auto generateError = [this]() {Hix::Application::ApplicationManager::getInstance().modalDialogManager().openOkCancelDialog(
				"Error has occured. Do you report this error?", "Report Error", "Ignore",
				[this]() {
					//report error pressed
					Hix::Application::ApplicationManager::getInstance().modalDialogManager().closeDialog();
					_progressManager.generateErrorPopup("Reporting Error");
					
				},
				[this]() {
					Hix::Application::ApplicationManager::getInstance().modalDialogManager().closeDialog();
				}
				);
			};

			QMetaObject::invokeMethod(&Hix::Application::ApplicationManager::getInstance().engine(), generateError, Qt::BlockingQueuedConnection);
			Sleep(3000);
			_progressManager.deletePopup();
		}
		catch (...)
		{
			//incase of custom unknown exception, gracefully exit
		}

		_progressManager.deletePopup();
		Hix::Application::ApplicationManager::getInstance().stateChanged();
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

TaskManager::~TaskManager()
{
	_end = true;
	EmptyTask lastTaskflow;
	enqueTask(&lastTaskflow);
	_taskThread.join();
}

