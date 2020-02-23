#pragma once
#include <memory>
#include <Qt3DCore>
#include "../Settings/AppSetting.h"

namespace Hix
{
	namespace UI
	{
		class GridBed
		{
		public:
			GridBed();
			virtual ~GridBed();
			void drawBed();
		protected:
			std::unique_ptr<Qt3DCore::QEntity> _bedShape;
			std::unique_ptr<Qt3DCore::QEntity> _grid;
			std::unique_ptr<Qt3DCore::QEntity> _logo;
			Qt3DCore::QTransform _logoTransform;
		};
	}
}

