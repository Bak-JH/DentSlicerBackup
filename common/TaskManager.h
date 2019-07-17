#pragma once
#include <list>
#include <array>
#include <memory>
#include <thread>
#include "taskflow/taskflow.hpp"
#include "moodycamel/readerwriterqueue.h"


class TaskManager
{
public:
	TaskManager();
	void run();
	void enqueTask(tf::Taskflow* task);
	~TaskManager();
private:
	tf::Executor _executor;
	moodycamel::BlockingReaderWriterQueue<tf::Taskflow*> _queue;
	bool _end = false;
	std::thread _taskThread;
};