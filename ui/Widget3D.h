#pragma once

#include <QObject>
#include <QApplication>
#include <Qt3DCore>
#include <Qt3DRender>
#include <Qt3DExtras>
#include <Qt3DInput>
#include "input/Draggable.h"
#include "glmodel.h"

using namespace Qt3DCore;
using namespace Qt3DRender;
using namespace Qt3DExtras;

namespace Hix
{
	namespace UI
	{
		//class Widget3D: public QEntity, public 
		//{
		//	Q_OBJECT
		//public:
		//	virtual ~Widget3D();
		//	virtual void onPressed	(Hix::Input::MouseEventData& e, Qt3DRender::QRayCasterHit& hit) = 0;
		//	virtual void onReleased	(Hix::Input::MouseEventData& e, Qt3DRender::QRayCasterHit& hit) = 0;
		//	//hover
		//	//virtual void onEntered() = 0;
		//	//virtual void onExited() = 0;
		//};


	}
}
