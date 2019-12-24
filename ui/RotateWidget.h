#pragma once
#include "Widget.h"
#include <qtorusmesh.h>


namespace Hix
{
	namespace UI
	{
		class Widget3D;
		class RotateWidget :public Hix::UI::Widget
		{
		public:
			RotateWidget(const QVector3D& axis, Qt3DCore::QEntity* parent);
			virtual ~RotateWidget();
			bool isDraggable(Hix::Input::MouseEventData& e, const Qt3DRender::QRayCasterHit& hit) override;
			void dragStarted(Hix::Input::MouseEventData&e,const Qt3DRender::QRayCasterHit&hit) override;
			void doDrag(Hix::Input::MouseEventData& e)override;
			void dragEnded(Hix::Input::MouseEventData&e) override;
			void onEntered()override;
			void onExited() override;
			void setHighlight(bool enable) override;

		private:
			double calculateRot();
			QPoint _mouseOrigin;
			QPoint _mouseCurrent;
			QPoint _mousePrev;
			double _pastAngle;
			Qt3DExtras::QTorusMesh _torus;
			Qt3DCore::QTransform _transform;
			Qt3DExtras::QPhongMaterial _material;
			//rotation axis


			static const float ROTATE_SPEED;

		};
	}
}

