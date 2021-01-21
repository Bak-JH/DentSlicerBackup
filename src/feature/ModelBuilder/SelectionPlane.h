#pragma once
#include "../cut/DrawingPlane.h"
#include "../../input/Hoverable.h"





namespace Hix
{
	namespace Features
	{
		
		class SelectionPlane: public Hix::Features::Cut::DrawingPlane
		{
		public:
			using Hix::Features::Cut::DrawingPlane::DrawingPlane;
			void clicked(Hix::Input::MouseEventData&, const Qt3DRender::QRayCasterHit&)override;
			void removePt(Hix::Features::Cut::FreeCutPtWidget* pt)override;
			void clearPt()override;

		private:
			Hix::Features::Cut::FreeCutPtWidget* _firstPt = nullptr;

		};

	}
}
