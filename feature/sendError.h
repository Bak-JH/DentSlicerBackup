#pragma once
#include "interfaces/Mode.h"
namespace Hix
{
	namespace Render
	{
		class SceneEntityWithMaterial;
	}
	namespace Features
	{
		class SendError : public Hix::Tasking::Task
		{
		public:
			SendError(const std::string& title, const std::string& details);
			virtual ~SendError();
			void run()override;
		private:
			std::string _title;
			std::string _details;
		};
	}
}
