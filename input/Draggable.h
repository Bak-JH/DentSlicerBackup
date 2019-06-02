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
		class Draggable
		{
		public:
			virtual bool isDraggable(MouseEventData& e,const Qt3DRender::QRayCasterHit& hit) = 0;
			virtual void dragStarted(MouseEventData& e, const Qt3DRender::QRayCasterHit& hit) = 0;
			virtual void doDrag(MouseEventData& e) = 0;
			virtual void dragEnded(MouseEventData& e) = 0;
			
		};
	}
}
