#pragma once
#include <memory>
namespace Qt3DCore
{
	class QEntity;
}
namespace Hix
{
	namespace UI
	{
		class GridBed
		{
		public:
			GridBed();
			virtual ~GridBed();
			void drawBed(Qt3DCore::QEntity* parent);
		protected:
			std::unique_ptr<Qt3DCore::QEntity> _bedShape;
			std::unique_ptr<Qt3DCore::QEntity> _grid;
		};
	}
}

