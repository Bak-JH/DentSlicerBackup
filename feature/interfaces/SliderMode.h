#pragma once
#include "Mode.h"
#include "Qml/components/SlideBar.h"

namespace Hix
{
	namespace Features
	{
		class SliderMode : virtual public Mode
		{
		public:
			SliderMode(double min, double max);
			void setSliderVisible(bool isVisible);
			virtual ~SliderMode();
		protected:
			std::unique_ptr<Hix::QML::SlideBarShell> _slideBar;
		};
	}
}