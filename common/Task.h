#pragma once
#include "taskflow/taskflow.hpp"


namespace Hix
{
	namespace Tasking
	{
		class Task
		{
		public:
			virtual tf::Taskflow& getFlow() = 0;
			virtual ~Task();
		};

		class GenericTask: public Task
		{
		public:
			tf::Taskflow& getFlow()override;
			virtual ~GenericTask();

		protected:
			tf::Taskflow _flow;
		};
		class UITask: public Task
		{
		public:
			UITask(std::function<void()> functor);
			tf::Taskflow& getFlow()override;
			virtual ~UITask();

		private:
			tf::Taskflow _flow;
		};
	}
}
