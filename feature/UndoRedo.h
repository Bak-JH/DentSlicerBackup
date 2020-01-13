#pragma once
#include "../common/Task.h"

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
		private:
			Hix::Features::Feature& _feature;
		};


	}
}
