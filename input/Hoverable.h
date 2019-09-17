#pragma once

//draggable interface

namespace Qt3DRender
{
	class QRayCasterHit;
}

namespace Hix
{
	namespace Input
	{
		struct MouseEventData;
		class Hoverable
		{
		public:
			virtual void onEntered()=0;
			virtual void onExited() =0;
			virtual ~Hoverable();
		};
	}
}
