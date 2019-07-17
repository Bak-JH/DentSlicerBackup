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
#include "../input/Highlightable.h"

namespace Hix
{
	namespace UI
	{
		class Widget3D;
		class RotateWidget :public Qt3DCore::QEntity, public Hix::Input::Draggable, public Hix::Input::Hoverable, public Hix::Input::Highlightable
		{
			Q_OBJECT
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
			Widget3D* _parent = nullptr;
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

