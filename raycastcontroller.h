#pragma once

#include <QAbstractRayCaster>
#include <qmouseevent.h>
#include "common/Singleton.h"
namespace Qt3DCore
{
	class QEntity;
}
namespace Qt3DRender
{
	class QScreenRayCaster;
	class QAbstractRayCaster;
}
namespace Qt3DInput
{
	class QMouseHandler;
	class QMouseEvent;
}
class RayCastController: public QObject, private Common::Singleton<RayCastController>
{
	Q_OBJECT
public:
    RayCastController();
	void initialize(Qt3DCore::QEntity* camera);
	//void initialize(Qt3DRender::QScreenRayCaster* rayCaster, Qt3DInput::QMouseHandler* mouseHandler);
public slots:
	void mousePressed(Qt3DInput::QMouseEvent* mouse);
	void mouseReleased(Qt3DInput::QMouseEvent* mouse);
	void mousePositionChanged(Qt3DInput::QMouseEvent* mouse);

	void hitsChanged(const Qt3DRender::QAbstractRayCaster::Hits& hits);


private:

	bool isClick(QPoint releasePt);
	Qt3DRender::QScreenRayCaster* _rayCaster = nullptr;
	Qt3DInput::QMouseHandler* _mouseHandler = nullptr;

	//click detection
	std::chrono::time_point<std::chrono::system_clock> _pressedTime;
	QPoint _pressedPt;

	//I wonder when qt deletes the event
	Qt3DInput::QMouseEvent* _latestEvent;

	static const std::chrono::milliseconds MAX_CLICK_DURATION;
	static const size_t MIN_CLICK_MOVEMENT_SQRD;
signals:

};
