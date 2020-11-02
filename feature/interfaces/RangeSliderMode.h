#pragma once
#include "Mode.h"
#include <memory>
#include "Qml/components/SlideBar.h"

namespace Hix
{
	namespace Features
	{
		class RangeSliderMode : virtual public Mode
		{
		public:
			RangeSliderMode(double min, double max);
			virtual ~RangeSliderMode();
		protected:
			Hix::QML::RangeSlideBarShell& slider();
		private:
			std::unique_ptr<Hix::QML::RangeSlideBarShell> _slideBar;
			std::unique_ptr<QQmlComponent> _component;
		};
	}
}