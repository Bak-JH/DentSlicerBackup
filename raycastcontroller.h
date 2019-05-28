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

//cause holding 
struct MouseEventData
{
	QVector2D position;
	Qt3DInput::QMouseEvent::Buttons button;
	Qt3DInput::QMouseEvent::Modifiers modifiers;
public:
	MouseEventData(Qt3DInput::QMouseEvent*);
	MouseEventData();
};
class RayCastController: public QObject
{
	Q_OBJECT
public:
    RayCastController();
	void initialize(Qt3DCore::QEntity* camera);


	//for ray casting optimization using the bounding box
	//Qt3DRender::QLayer _modelLayer;
	void addLayer(Qt3DRender::QLayer* layer);
	void removeLayer(Qt3DRender::QLayer* layer);

public slots:

	void mousePressed(Qt3DInput::QMouseEvent* mouse);
	void mouseReleased(Qt3DInput::QMouseEvent* mouse);
	void mousePositionChanged(Qt3DInput::QMouseEvent* mouse);

	void hitsChanged(const Qt3DRender::QAbstractRayCaster::Hits& hits);

private:
	Qt3DCore::QEntity* _camera = nullptr;
	enum RayCastMode
	{
		//Click = 0,
		//Other
		Pressed = 0,
		Released
	};
	bool isClick(QVector2D releasePt);
	bool shouldRaycastReleased(Qt3DInput::QMouseEvent*);
	bool shouldRaycastPressed(Qt3DInput::QMouseEvent*);

	Qt3DRender::QScreenRayCaster* _rayCaster = nullptr;
	Qt3DInput::QMouseHandler* _mouseHandler = nullptr;
	RayCastMode _rayCastMode;
	//click detection
	std::chrono::time_point<std::chrono::system_clock> _pressedTime;
	QVector2D _pressedPt;
	QPoint _releasePt;
	//mouse event
	MouseEventData _mouseEvent;
	//change this to atomic if raytracing is done on seperate thread
	std::atomic<bool> _rayCasterBusy = false;
	//for removing layer element
	std::vector<GLModel*> _boundBoxHitModels;
	static const std::chrono::milliseconds MAX_CLICK_DURATION;
	static const size_t MAX_CLICK_MOVEMENT;
signals:

};
