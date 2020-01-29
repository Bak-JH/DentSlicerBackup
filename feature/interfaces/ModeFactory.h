#pragma once
#include "Mode.h"
#include <memory>
namespace Hix
{
	namespace Features
	{
		class Mode;
		class ModeFactory
		{
		public:
			virtual ~ModeFactory();
			virtual std::unique_ptr<Mode> createMode() = 0;
		protected:
			virtual void redoImpl() = 0;
		};
	}
}