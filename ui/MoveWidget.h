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
		class MoveXYZWidget;
		class MoveWidget :public Qt3DCore::QEntity, public Hix::Input::Draggable, public Hix::Input::Hoverable
		{
			Q_OBJECT
		public:
			MoveWidget(const QVector3D& axis, Qt3DCore::QEntity* parent);
			virtual ~MoveWidget();
			bool isDraggable(Hix::Input::MouseEventData& e, const Qt3DRender::QRayCasterHit& hit) override;
			void dragStarted(Hix::Input::MouseEventData&e,const Qt3DRender::QRayCasterHit&hit) override;
			void doDrag(Hix::Input::MouseEventData& e)override;
			void dragEnded(Hix::Input::MouseEventData&e) override;
			void onEntered()override;
			void onExited() override;
			void setHighlight(bool enable);

		private:
			double calculateMove();
			MoveXYZWidget* _parent = nullptr;
			QPoint _mouseOrigin;
			QPoint _mouseCurrent;
			QPoint _mousePrev;

			double _pastProj;
			Qt3DRender::QMesh _mesh;
			Qt3DCore::QTransform _transform;
			Qt3DExtras::QPhongMaterial _material;
			//rotation axis
			QVector3D _axis;
			//static const float ROTATE_SPEED;
			static const QUrl ARROW_MESH_URL;

		};

	}
}

