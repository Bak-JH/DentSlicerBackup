#pragma once

#include <QObject>
#include <Qt3DCore>
#include <Qt3DRender>
#include <Qt3DExtras>
#include <Qt3DInput>

#include "input/Draggable.h"
#include "input/Hoverable.h"
#include "input/Highlightable.h"

namespace Hix
{
	namespace UI
	{
		class Widget3D;
		class CubeWidget :public Qt3DCore::QEntity, public Hix::Input::Draggable, public Hix::Input::Hoverable, public Hix::Input::Highlightable
		{
			Q_OBJECT

		public:
			CubeWidget(QEntity* parent, QVector3D overhangPoint);
			virtual ~CubeWidget();

            bool isDraggable(Hix::Input::MouseEventData& e, const Qt3DRender::QRayCasterHit& hit) override;
            void dragStarted(Hix::Input::MouseEventData& e, const Qt3DRender::QRayCasterHit& hit) override;
            void doDrag(Hix::Input::MouseEventData& e)override;
            void dragEnded(Hix::Input::MouseEventData& e) override;
            void onEntered()override;
            void onExited() override;
            void setHighlight(bool enable) override;

		private:
            Widget3D* _parent = nullptr;
			Qt3DExtras::QCuboidMesh _mesh;
			Qt3DCore::QTransform _transform;
			Qt3DExtras::QPhongMaterial _material;

			QPoint _mouseOrigin;
			QPoint _mouseCurrent;
			QPoint _mousePrev;
			double _pastProj;

			double calculateMove();
		};
	}
}