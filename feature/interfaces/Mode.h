#pragma once
#include "feature/interfaces/Feature.h"

namespace Hix
{
	namespace Features
	{
		class ModeDialogQMLParsed;
		class Mode
		{
		public:
			Mode();
			virtual ~Mode();
		protected:
			std::unique_ptr< ModeDialogQMLParsed> _dialog;
		};
		class InstantMode : public Mode
		{
		public:
			InstantMode();
			virtual ~InstantMode();
		};
	}
}