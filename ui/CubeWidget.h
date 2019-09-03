#pragma once

#include <QObject>
#include <Qt3DCore>
#include <Qt3DRender>
#include <Qt3DExtras>
#include <Qt3DInput>

#include "input/Draggable.h"
#include "input/Hoverable.h"
#include "Highlightable.h"
#include "input/raycastcontroller.h"

#include "glmodel.h"

namespace Hix
{
	namespace UI
	{
		class Widget3D;
		class CubeWidget :public Qt3DCore::QEntity, public Hix::Input::Draggable, public Hix::Input::Hoverable, public Hix::UI::Highlightable
		{
			Q_OBJECT

		public:
			CubeWidget(QEntity* parent, QVector3D overhangPoint, GLModel* model);
			virtual ~CubeWidget();


			void onClicked(Hix::Input::MouseEventData& e, const Qt3DRender::QRayCasterHit& hit);
			/* these are not used in now. */
            bool isDraggable(Hix::Input::MouseEventData& e, const Qt3DRender::QRayCasterHit& hit) override;
            void dragStarted(Hix::Input::MouseEventData& e, const Qt3DRender::QRayCasterHit& hit) override;
            void doDrag(Hix::Input::MouseEventData& e)override;
            void dragEnded(Hix::Input::MouseEventData& e) override;
            void onEntered()override;
            void onExited() override;
            void setHighlight(bool enable) override;

		private:
			GLModel* _model;
            Widget3D* _parent = nullptr;
			Qt3DExtras::QCuboidMesh _mesh;
			Qt3DCore::QTransform _transform;
			Qt3DExtras::QPhongMaterial _material;

			/* these are not used in now. */
			QPoint _mouseOrigin;
			QPoint _mouseCurrent;
			QPoint _mousePrev;
			double _pastProj;

			double calculateMove();
		};
	}
}