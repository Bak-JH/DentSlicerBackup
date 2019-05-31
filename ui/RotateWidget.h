#pragma once

#include <QObject>
#include <QApplication>
#include <Qt3DCore>
#include <Qt3DRender>
#include <Qt3DExtras>
#include <Qt3DInput>
#include <qtorusmesh.h>
#include "../input/Draggable.h"
#include "../input/Hoverable.h"

namespace Hix
{
	namespace UI
	{
		class RotateXYZWidget;
		class RotateWidget :public Qt3DCore::QEntity, public Hix::Input::Draggable, public Hix::Input::Hoverable
		{
			Q_OBJECT
		public:
			RotateWidget(const QVector3D& axis, Qt3DCore::QEntity* parent);
			virtual ~RotateWidget();
			bool isDraggable(Hix::Input::MouseEventData& e, Qt3DRender::QRayCasterHit& hit) override;
			void dragStarted(Hix::Input::MouseEventData&e, Qt3DRender::QRayCasterHit&hit) override;
			void doDrag(Hix::Input::MouseEventData& e)override;
			void dragEnded(Hix::Input::MouseEventData&e) override;
			void onEntered()override;
			void onExited() override;
		private:
			double calculateRot();
			RotateXYZWidget* _parent = nullptr;
			QPoint _mouseOrigin;
			QPoint _mouseCurrent;
			QPoint _mousePrev;

			double _pastAngle;
			Qt3DExtras::QTorusMesh _torus;
			Qt3DCore::QTransform _transform;
			Qt3DExtras::QPhongMaterial _material;
			//rotation axis
			QVector3D _axis;
			static const float ROTATE_SPEED;

		};

	}
}

