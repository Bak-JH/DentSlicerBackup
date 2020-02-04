#pragma once
#include "feature/interfaces/Feature.h"

namespace Hix
{
	namespace QML
	{
		namespace Controls
		{
			class ToggleSwitch;
		}
	}
	namespace Features
	{
		class Mode
		{
		public:
			Mode();
			void addButton(Hix::QML::Controls::ToggleSwitch* button);
			virtual ~Mode();
			virtual void apply() {}; //TODO remove this when alls compiled
		protected:
			Hix::QML::Controls::ToggleSwitch* _button = nullptr;
		};
		class InstantMode : public Mode
		{
		public:
			InstantMode();
			virtual ~InstantMode();
		};
	}
}