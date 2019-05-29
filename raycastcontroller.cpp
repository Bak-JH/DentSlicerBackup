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
const size_t RayCastController::MAX_CLICK_MOVEMENT = 20;

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

bool RayCastController::isDrag()const
{
	return _isDrag;
}





bool RayCastController::isClick(QVector2D releasePt)
{

	bool isClickDist = releasePt.distanceToPoint(_pressedPt) < MAX_CLICK_MOVEMENT;
	if (std::chrono::system_clock::now() - _pressedTime < MAX_CLICK_DURATION && isClickDist)
	{
		return true;
	}
	return false;
}

bool RayCastController::shouldRaycastPressed(Qt3DInput::QMouseEvent*me)
{
	if (me->button() == Qt3DInput::QMouseEvent::Buttons::LeftButton)
		return true;
	return false;
}

bool RayCastController::shouldRaycastReleased(Qt3DInput::QMouseEvent* me)
{
	if (me->button() == Qt3DInput::QMouseEvent::Buttons::LeftButton || me->button() == Qt3DInput::QMouseEvent::Buttons::RightButton)
		return true;
	return false;
}

void RayCastController::mousePressed(Qt3DInput::QMouseEvent* mouse)
{
	mouse->setAccepted(true);
	_mouseEvent = MouseEventData(mouse);
	_pressedTime = std::chrono::system_clock::now();
	_pressedPt = _mouseEvent.position;
	_isDrag = true;
	if (shouldRaycastPressed(mouse))
	{
		bool busy = false;
		if (!_rayCasterBusy.compare_exchange_strong(busy, true))
			return;
		_rayCastMode = RayCastMode::Pressed;
		_rayCaster->trigger(QPoint(mouse->x(), mouse->y()));
	}
}

void RayCastController::mouseReleased(Qt3DInput::QMouseEvent* mouse)
{
	mouse->setAccepted(true);
	_mouseEvent = MouseEventData(mouse);
	_isDrag = false;
	qmlManager->resetCursor();
	//common loginc that does not need ray casting
	if (qmlManager->getViewMode() == VIEW_MODE_SUPPORT) {
		qmlManager->openYesNoPopUp(false, "", "Support will disappear.", "", 18, "", ftrSupportDisappear, 1);
		return;
	}
	bool isMoved = false;
	for (auto selected : qmlManager->getSelectedModels())
	{
		if (selected->isMoved)
		{
			selected->isMoved = false;
			isMoved = true;
		}
	}
	if (isMoved) {
		qmlManager->modelMoveDone();
	}
	if (qmlManager->yesno_popup->property("isFlawOpen").toBool())
		return;


	if (shouldRaycastReleased(mouse))
	{
		//only do ray cast on click for release event
		if (isClick(_mouseEvent.position))
		{
			bool busy = false;
			if (!_rayCasterBusy.compare_exchange_strong(busy, true))
				return;
			_rayCastMode = RayCastMode::Released;
			_rayCaster->trigger(QPoint(mouse->x(), mouse->y()));
		}

	}
}

void RayCastController::mousePositionChanged(Qt3DInput::QMouseEvent* mouse)
{
	if (_isDrag)
	{
		if (_rayCasterBusy)
			return;
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
				auto test = hit.primitiveIndex();
				qDebug() << "ray trace glmodel" << glModel;
				if (_rayCastMode == Pressed)
				{
					qDebug() << "glmodel pressed";

					glModel->mousePressedRayCasted(_mouseEvent, hit);
				}
				else
				{
					qDebug() << "glmodel relllllleased";

					glModel->mouseReleasedRayCasted(_mouseEvent, hit);




				}
				//ignore other hits
				break;
			}
			else
			{
				return;
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
