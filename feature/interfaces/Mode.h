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
		};
		class InstantMode : public Mode
		{
		public:
			InstantMode();
			virtual ~InstantMode();
		};
	}
}