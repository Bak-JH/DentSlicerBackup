#pragma once

//#include "Widget3D.h"
#include <QObject>
#include <QApplication>
#include <Qt3DCore>
#include <Qt3DRender>
#include <Qt3DExtras>
#include <Qt3DInput>
#include "RotateWidget.h"
#include <array>
namespace Hix
{
	namespace UI
	{

		class RotateXYZWidget :public Qt3DCore::QEntity
		{
			Q_OBJECT
		public:
			RotateXYZWidget();
			virtual ~RotateXYZWidget();
			void setVisible(bool show);
			bool visible();
			Qt3DRender::QLayer layer;
			//need to place the widget between camera and the center
			void updatePosition();
			Qt3DCore::QTransform* transform();
			bool isManipulated();
			void setManipulated(bool isManipulated);
		private:
			//fucking qt....
			std::array<RotateWidget, 3> _widgets;
			bool _visible = true;
			QVector3D _center;
			Qt3DCore::QTransform _transform;
			bool _isManipulated = false;

		};

	}
}

