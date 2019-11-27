#pragma once

//#include "Widget3D.h"
#include <QObject>
#include <QApplication>
#include <Qt3DCore>
#include <Qt3DRender>
#include <Qt3DExtras>
#include <Qt3DInput>
#include <array>
#include <memory>
namespace Hix
{
	namespace UI
	{
		class Widget;
	}
	namespace UI
	{
		class Widget3D :public Qt3DCore::QEntity
		{
			Q_OBJECT
		public:
			Widget3D();
			virtual ~Widget3D();
			void addWidget(std::unique_ptr<Hix::UI::Widget> widget);
			void setVisible(bool show);
			bool visible();
			//need to place the widget between camera and the center
			void updatePosition();
			Qt3DCore::QTransform* transform();
			bool isManipulated();
			void setManipulated(bool isManipulated);
		private:
			std::vector<std::unique_ptr<Hix::UI::Widget>> _widgets;
			bool _visible = true;
			QVector3D _center;
			Qt3DCore::QTransform _transform;
			bool _isManipulated = false;

		};

	}
}

