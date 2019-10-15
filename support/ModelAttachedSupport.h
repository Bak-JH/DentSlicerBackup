#pragma once
#include "../feature/overhangDetect.h"
class GLModel;
namespace Hix
{
	namespace Support
	{
		class ModelAttachedSupport
		{
		public:
			virtual const Hix::OverhangDetect::Overhang& getOverhang()const = 0;
			const GLModel& getAttachedModel();
		};
	}
}