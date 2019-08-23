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
		class Clickable
		{
		public:
			virtual void clicked(Hix::Input::MouseEventData&, const Qt3DRender::QRayCasterHit&) = 0;

			
		};
	}
}
