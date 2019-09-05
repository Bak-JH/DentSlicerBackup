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
			virtual bool isHitTestable();
			Qt3DRender::QLayer* getLayer();

		protected:
			virtual void initHitTest() = 0;
			bool _hitEnabled = false;
			Qt3DRender::QLayer _layer;

		};

	}
}