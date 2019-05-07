#pragma once

#include <QAbstractRayCaster>
#include <Qt3DRender>
#include <qmouseevent.h>
#include <vector>
#include "common/Singleton.h"

class GLModel;
namespace Qt3DCore
{
	class QEntity;
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


	//for ray casting optimization using the bounding box
	Qt3DRender::QLayer _boundingBoxLayer;
	Qt3DRender::QLayer _modelLayer;
public slots:
	void mousePressed(Qt3DInput::QMouseEvent* mouse);
	void mouseReleased(Qt3DInput::QMouseEvent* mouse);
	void mousePositionChanged(Qt3DInput::QMouseEvent* mouse);

	void hitsChanged(const Qt3DRender::QAbstractRayCaster::Hits& hits);

private:
	void clearLayers();
	bool isClick(QPoint releasePt);
	Qt3DRender::QScreenRayCaster* _rayCaster = nullptr;
	Qt3DInput::QMouseHandler* _mouseHandler = nullptr;

	//click detection
	std::chrono::time_point<std::chrono::system_clock> _pressedTime;
	QPoint _pressedPt;
	QPoint _releasePt;


	//I wonder when qt deletes the event
	//Qt3DInput::QMouseEvent* _latestEvent;

	//for removing layer element
	std::vector<GLModel*> _boundBoxHitModels;
	static const std::chrono::milliseconds MAX_CLICK_DURATION;
	static const size_t MIN_CLICK_MOVEMENT_SQRD;
signals:

};
