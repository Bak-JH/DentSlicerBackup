#pragma once
#include "../Tasking/Task.h"

namespace Hix
{
	namespace Features
	{
		class Feature;
		class Undo : public Hix::Tasking::Task
		{
		public:
			Undo();
			void run()override;
			virtual ~Undo();
		};

		class Redo : public Hix::Tasking::Task
		{
		public:
			Redo();
			void run()override;
			virtual ~Redo();
		};

	}
}
