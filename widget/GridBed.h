#pragma once
#include <memory>
#include <Qt3DCore>

namespace Hix
{
	namespace UI
	{
		class GridBed
		{
		public:
			GridBed(Qt3DCore::QEntity* parent);
			virtual ~GridBed();
			void drawBed();
		protected:
			std::unique_ptr<Qt3DCore::QEntity> _bedShape;
			std::unique_ptr<Qt3DCore::QEntity> _grid;
			std::unique_ptr<Qt3DCore::QEntity> _logo;
			Qt3DCore::QTransform _logoTransform;
			Qt3DCore::QEntity* _parent;
		};
	}
}

