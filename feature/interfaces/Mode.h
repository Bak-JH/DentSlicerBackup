#pragma once
#include "feature/interfaces/Feature.h"

namespace Hix
{
	namespace Features
	{
		class Mode
		{
		public:
			Mode();
			virtual ~Mode();
			virtual void apply() {}; //TODO remove this when alls compiled

		};
		class InstantMode : public Mode
		{
		public:
			InstantMode();
			virtual ~InstantMode();
		};
	}
}