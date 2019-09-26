#pragma once
#include <Qt3DRender>


namespace Hix
{
	namespace Input
	{
		class HitTestAble
		{
		public:
			virtual void setHitTestable(bool isEnable);
			virtual void setHoverable(bool isEnable);
			virtual bool isHitTestable();
			virtual bool isHoverable();
			Qt3DRender::QLayer* getLayer();

		protected:
			virtual void initHitTest() = 0;
			bool _hitEnabled = false;
			bool _hoverEnabled = false;

			Qt3DRender::QLayer _layer;

		};

	}
}