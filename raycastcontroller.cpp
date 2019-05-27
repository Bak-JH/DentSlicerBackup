#include "raycastcontroller.h"
#include <QAbstractRayCaster>
#include <qscreenraycaster.h>
#include <qmousehandler.h>
#include <qcamera.h>
#include <qmousedevice.h>
#include <qentity.h>
#include <vector>
#include "glmodel.h"
#include "qmlmanager.h"
using namespace Qt3DRender;
using namespace Qt3DInput;
using namespace Qt3DCore;
#if defined(_DEBUG) || defined(QT_DEBUG )
#define LOG_RAYCAST_TIME
#define RAYCAST_STRICT
#endif

#ifdef LOG_RAYCAST_TIME
std::chrono::time_point<std::chrono::system_clock> __startRayTraceTime;

#endif

const std::chrono::milliseconds RayCastController::MAX_CLICK_DURATION(500);
const size_t RayCastController::MIN_CLICK_MOVEMENT_SQRD = 40;

RayCastController::RayCastController()
{
}

void RayCastController::initialize(QEntity* camera)
{
	_rayCaster = new QScreenRayCaster();
	_mouseHandler = new QMouseHandler();

	camera->addComponent(_rayCaster);
	camera->addComponent(_mouseHandler);
	
	_mouseHandler->setSourceDevice(new QMouseDevice());
	_rayCaster->setFilterMode(QAbstractRayCaster::FilterMode::AcceptAnyMatchingLayers);
	QObject::connect(_mouseHandler, SIGNAL(released(Qt3DInput::QMouseEvent *)), this, SLOT(mouseReleased(Qt3DInput::QMouseEvent *)));
	QObject::connect(_mouseHandler, SIGNAL(pressed(Qt3DInput::QMouseEvent*)), this, SLOT(mousePressed(Qt3DInput::QMouseEvent*)));
	QObject::connect(_mouseHandler, SIGNAL(positionChanged(Qt3DInput::QMouseEvent*)), this, SLOT(mousePositionChanged(Qt3DInput::QMouseEvent*)));

	QObject::connect(_rayCaster, SIGNAL(hitsChanged(const Qt3DRender::QAbstractRayCaster::Hits&)), this, SLOT(hitsChanged(const Qt3DRender::QAbstractRayCaster::Hits&)));

}

void RayCastController::addLayer(Qt3DRender::QLayer* layer)
{
	_rayCaster->addLayer(layer);

}

void RayCastController::removeLayer(Qt3DRender::QLayer* layer)
{
	_rayCaster->removeLayer(layer);
}

//void RayCastController::initialize(Qt3DRender::QScreenRayCaster* rayCaster, Qt3DInput::QMouseHandler* mouseHandler)
//{
//	_rayCaster = rayCaster;
//	_mouseHandler = mouseHandler;
//	//camera->addComponent(_rayCaster);
//	//camera->addComponent(_mouseHandler);
//	//auto dbg = camera->components();
//	//_mouseHandler->setSourceDevice(new QMouseDevice());
//	QObject::connect(_mouseHandler, SIGNAL(released(Qt3DInput::QMouseEvent*)), this, SLOT(mouseReleased(Qt3DInput::QMouseEvent*)));
//	QObject::connect(_rayCaster, SIGNAL(hitsChanged(const Qt3DRender::QAbstractRayCaster::Hits&)), this, SLOT(hitsChanged(const Qt3DRender::QAbstractRayCaster::Hits&)));
//}



bool RayCastController::isClick(QPoint releasePt)
{

	bool isClickDist = ((releasePt.x() - _pressedPt.x()) ^ 2 + (releasePt.y() - _pressedPt.y()) ^ 2) < MIN_CLICK_MOVEMENT_SQRD;
	if (std::chrono::system_clock::now() - _pressedTime < MAX_CLICK_DURATION && isClickDist)
	{
		qDebug() << "clicked";
		return true;
	}
	return false;
}

void RayCastController::mousePressed(Qt3DInput::QMouseEvent* mouse)
{
	mouse->setAccepted(true);
//	_pressedTime = std::chrono::system_clock::now();
//	_pressedPt = { mouse->x(), mouse->y() };
//	//trying to tell if the click is a drag.
//	if (!qmlManager->selectedModels.empty())
//	{
//		_rayCastMode = RayCastMode::Other;
//		_rayCaster->trigger(QPoint(mouse->x(), mouse->y()));
//	}
	bool busy = false;
	if (!_rayCasterBusy.compare_exchange_strong(busy, true))
		return;
	_mouseEvent = MouseEventData(mouse);
	_rayCastMode = RayCastMode::Pressed;
	_rayCaster->trigger(QPoint(mouse->x(), mouse->y()));
}

void RayCastController::mouseReleased(Qt3DInput::QMouseEvent* mouse)
{
	mouse->setAccepted(true);
	//_releasePt = { mouse->x(), mouse->y() };
	//if (isClick(_releasePt))
	//{
	//	_rayCastMode = RayCastMode::Click;
	//	_rayCaster->trigger(QPoint(mouse->x(), mouse->y()));
	//}
	bool busy = false;
	if (!_rayCasterBusy.compare_exchange_strong(busy, true))
		return;
	_mouseEvent = MouseEventData(mouse);
	_rayCastMode = RayCastMode::Released;
	_rayCaster->trigger(QPoint(mouse->x(), mouse->y()));
}

void RayCastController::mousePositionChanged(Qt3DInput::QMouseEvent* mouse)
{
	if (_rayCasterBusy)
		return;
	//_rayCasterBusy = true;
	//disable move when in certain modes;
	//return;
	if (qmlManager->orientationActive ||
		qmlManager->rotateActive ||
		qmlManager->saveActive)
		return;

	_mouseEvent = MouseEventData(mouse);

	//only call mouseMoved to selected models that are NOT doing certain features
	for (auto selected : qmlManager->getSelectedModels())
	{
		if (selected->scaleActive ||
			selected->cutActive ||
			selected->shellOffsetActive ||
			selected->extensionActive ||
			selected->labellingActive ||
			selected->layflatActive ||
			selected->layerViewActive ||
			selected->manualSupportActive ||
			selected->supportViewActive)
			continue;

		selected->mouseMoved(_mouseEvent);
	}


}

void RayCastController::hitsChanged(const Qt3DRender::QAbstractRayCaster::Hits& hits)
{
	auto allHits = _rayCaster->hits();
	if (hits.size() > 0)
	{
		for (auto hit : hits)
		{

			auto glModel = dynamic_cast<GLModel*>(hit.entity());
			if (glModel)
			{
				qDebug() << "ray trace glmodel" << glModel;
				if (_rayCastMode == Pressed)
				{
					qDebug() << "glmodel pressed";

					glModel->mousePressed(_mouseEvent, hit);
				}
				else
				{
					qDebug() << "glmodel relllllleased";

					glModel->mouseReleased(_mouseEvent, hit);
				}
			}
		}

	}
	else if(_rayCastMode == Pressed)
	{
		qmlManager->backgroundClicked();
	}
	_rayCasterBusy = false;

}

MouseEventData::MouseEventData(Qt3DInput::QMouseEvent* v) : button(v->button()), modifiers(v->modifiers())
{
	position = {(float)v->x(), (float)v->y() };
}

MouseEventData::MouseEventData()
{
}
