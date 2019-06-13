#include "raycastcontroller.h"
#include <qquickitem.h>

#include "glmodel.h"
#include "qmlmanager.h"

#include "input/Draggable.h"
#include "input/Hoverable.h"
#include "utils/mathutils.h"

#include <chrono>
using namespace Qt3DRender;
using namespace Qt3DInput;
using namespace Qt3DCore;
using namespace Hix::Input;
using namespace std::chrono_literals;
using namespace Utils::Math;
#if defined(_DEBUG) || defined(QT_DEBUG )
#define LOG_RAYCAST_TIME
#define RAYCAST_STRICT
#endif
#ifdef LOG_RAYCAST_TIME
std::chrono::time_point<std::chrono::system_clock> __startRayTraceTime;

#endif

const std::chrono::milliseconds RayCastController::MAX_CLICK_DURATION = 200ms;
const float RayCastController::MAX_CLICK_MOVEMENT = 5;

RayCastController::RayCastController()
{
}

void RayCastController::initialize(QEntity* camera)
{


	camera->addComponent(&_rayCaster);
	camera->addComponent(&_hoverRayCaster);
	camera->addComponent(&_mouseHandler);
	_mouseHandler.setSourceDevice(&_mouseDevice);
	_rayCaster.setFilterMode(QAbstractRayCaster::FilterMode::AcceptAnyMatchingLayers);
	_hoverRayCaster.setFilterMode(QAbstractRayCaster::FilterMode::AcceptAnyMatchingLayers);
	_rayCaster.setRunMode(QAbstractRayCaster::RunMode::SingleShot);
	_hoverRayCaster.setRunMode(QAbstractRayCaster::RunMode::SingleShot);


	QObject::connect(&_mouseHandler, SIGNAL(released(Qt3DInput::QMouseEvent *)),
		this, SLOT(mouseReleased(Qt3DInput::QMouseEvent *)));
	QObject::connect(&_mouseHandler, SIGNAL(pressed(Qt3DInput::QMouseEvent*)), 
		this, SLOT(mousePressed(Qt3DInput::QMouseEvent*)));
	QObject::connect(&_mouseHandler, SIGNAL(positionChanged(Qt3DInput::QMouseEvent*)), 
		this, SLOT(mousePositionChanged(Qt3DInput::QMouseEvent*)));
	QObject::connect(&_rayCaster, SIGNAL(hitsChanged(const Qt3DRender::QAbstractRayCaster::Hits&)), 
		this, SLOT(hitsChanged(const Qt3DRender::QAbstractRayCaster::Hits&)));
	QObject::connect(&_hoverRayCaster, SIGNAL(hitsChanged(const Qt3DRender::QAbstractRayCaster::Hits&)), 
		this, SLOT(hoverHitsChanged(const Qt3DRender::QAbstractRayCaster::Hits&)));
}

void RayCastController::addLayer(Qt3DRender::QLayer* layer)
{
	_rayCaster.addLayer(layer);

}

void RayCastController::removeLayer(Qt3DRender::QLayer* layer)
{
	_rayCaster.removeLayer(layer);
}

void RayCastController::addHoverLayer(Qt3DRender::QLayer* layer)
{
	_hoverRayCaster.addLayer(layer);

}
void RayCastController::removeHoverLayer(Qt3DRender::QLayer* layer)
{
	_hoverRayCaster.removeLayer(layer);
}


void RayCastController::setHoverEnabled(bool isEnabled)
{
	if (_hoverEnabled != isEnabled)
	{
		_hoverEnabled = isEnabled;
        if (_hoverEnabled)
        {
            _hoverRayCaster.setEnabled(true);
        }
        else
        {
            _hoverRayCaster.setEnabled(false);
        }
	}
}

bool RayCastController::hoverEnabled()
{
	return _hoverEnabled;
}

bool Hix::Input::RayCastController::mousePosInBound(const Qt3DInput::QMouseEvent* mv)
{
	auto sceneScreen = qmlManager->scene3d;
	if (mv->x() > 0 && mv->x() <= sceneScreen->width() && mv->y() > 0 && mv->y() <= sceneScreen->height())
	{
		return true;
	}
	return false;
}





//bool RayCastController::isClick(QVector2D releasePt)
//{
//
//	bool isClickDist = releasePt.distanceToPoint(_pressedPt) < MAX_CLICK_MOVEMENT;
//	if (std::chrono::system_clock::now() - _pressedTime < MAX_CLICK_DURATION && isClickDist)
//	{
//		return true;
//	}
//	return false;
//}
//
//bool RayCastController::shouldRaycastPressed(Qt3DInput::QMouseEvent*me)
//{
//	if (me->button() == Qt3DInput::QMouseEvent::Buttons::LeftButton)
//		return true;
//	return false;
//}

//bool RayCastController::shouldRaycastReleased(Qt3DInput::QMouseEvent* me)
//{
//	if (me->button() == Qt3DInput::QMouseEvent::Buttons::LeftButton || me->button() == Qt3DInput::QMouseEvent::Buttons::RightButton)
//		return true;
//	return false;
//}
bool RayCastController::verifyClick()
{
	qDebug() << "verifyClick";

	//lock that is to be relinquished
	std::unique_lock<std::mutex> lk(_clickVerifyMtx);
	//TODO: spurious wake ups are causing to to failed to timeout often....
	if (_isClickVerified.wait_for(lk, MAX_CLICK_DURATION) == std::cv_status::timeout )
	{
		//if the click was not verified yet ie) not yet released, it's a drag
		//start drag
		if (!_pressedPt.isNull())
		{
			_rayCastMode = RayCastMode::Drag;
			_rayCaster.trigger(_pressedPt);
			return true;
		}
	}
	return false;
}

void RayCastController::mousePressed(Qt3DInput::QMouseEvent* mouse)
{

	if (mouse->button() == Qt3DInput::QMouseEvent::Buttons::LeftButton || mouse->button() == Qt3DInput::QMouseEvent::Buttons::RightButton)
	{
		if (!_mouseBusy)
		{

			//no need for mutex, mouse events occur in main thread
			_mouseBusy = true;
			mouse->setAccepted(true);
			_mouseEvent = MouseEventData(mouse);
			_pressedPt = _mouseEvent.position;
			_verifyClickTask = std::async(std::launch::async, &RayCastController::verifyClick, this);
		}



	}
}

void RayCastController::mouseReleased(Qt3DInput::QMouseEvent* mouse)
{
	if (mouse->button() == Qt3DInput::QMouseEvent::Buttons::LeftButton || mouse->button() == Qt3DInput::QMouseEvent::Buttons::RightButton)
	{
		_mouseEvent = MouseEventData(mouse);
		mouse->setAccepted(true);
		qmlManager->resetCursor();
		_mouseBusy = false;

		//if drag, stop dragging
		if (_dragged)
		{
			_dragged->dragEnded(_mouseEvent);
			_dragged = nullptr;
		}
		else
		{
			if (_verifyClickTask.valid())
			{
				//is a click, so stop verifier from working to figure out if it's a click
				_isClickVerified.notify_all();
				//if successfully cancelled
				auto test = _verifyClickTask.get();
				if (!test)
				{
					//common loginc that does not need ray casting
					if (qmlManager->getViewMode() == VIEW_MODE_SUPPORT) {
						qmlManager->openYesNoPopUp(false, "", "Support will disappear.", "", 18, "", ftrSupportDisappear, 1);
						return;
					}
					if (qmlManager->yesno_popup->property("isFlawOpen").toBool())
						return;

					bool busy = false;
					if (!_mouseEvent.position.isNull())
					{
						_rayCastMode = RayCastMode::Click;
						_rayCaster.trigger(_mouseEvent.position);
					}
				}
			}

		}
	}
	//so that we can know if a verification is still going on by checking validity of verifier


	
}

void RayCastController::mousePositionChanged(Qt3DInput::QMouseEvent* mouse)
{
	if (mousePosInBound(mouse))
	{
		if (_hoverEnabled && !_hoverRaycastBusy)
		{
			auto hoverEvent = MouseEventData(mouse);
			if (!hoverEvent.position.isNull())
			{
				_hoverRaycastBusy = true;
				_hoverRayCaster.trigger(hoverEvent.position);
			}
		}
		if (_mouseBusy)
		{
			if (_dragged)
			{
				_mouseEvent = MouseEventData(mouse);

				_dragged->doDrag(_mouseEvent);
			}
			else
			{
				//we still need to use mouse event from pressed for initiating drag.
				auto tmpMouse = MouseEventData(mouse);
				//probably still trying to figure out if it's a click, so help out
				if (intPointDistance(tmpMouse.position, _pressedPt) > MAX_CLICK_MOVEMENT && !tmpMouse.position.isNull())
				{
					//mutex against verifier, check verifier state
					if (_verifyClickTask.valid())
					{
						//cancel verifier
						_isClickVerified.notify_all();
						//if successfully cancelled
						if (!_verifyClickTask.get())
						{
							//do drag
							_rayCastMode = RayCastMode::Drag;
							_rayCaster.trigger(_pressedPt);
						}

					}
				}

			}
		}
	}
}

void RayCastController::hitsChanged(const Qt3DRender::QAbstractRayCaster::Hits& hits)
{
	if (hits.size() > 0)
	{
		//get the closest..WTF qt, you are supposed to return the nearest one first!
		auto minLengthSqrd = std::numeric_limits<float>::max();
		const Qt3DRender::QRayCasterHit* nearestHit = nullptr;
		for (auto& hit : hits)
		{

			if (hit.distance() < minLengthSqrd)
			{
				minLengthSqrd = hit.distance();
				nearestHit = &hit;
			}

		}
		if(nearestHit)
		{
			if (_rayCastMode == Drag)
			{
				auto draggable = dynamic_cast<Draggable*>(nearestHit->entity());
				if (draggable && draggable->isDraggable(_mouseEvent, *nearestHit))
				{
					_dragged = draggable;
					_dragged->dragStarted(_mouseEvent, *nearestHit);
				}
			}
			else
			{
				auto glmodel = dynamic_cast<GLModel*>(nearestHit->entity());
				if (glmodel)
				{
					glmodel->clicked(_mouseEvent, *nearestHit);
				}
                else
                {
                    auto parent = dynamic_cast<GLModel*>(nearestHit->entity()->parentEntity());
                    if(parent)
                    {
                        parent->clicked(_mouseEvent, *nearestHit);
                    }
                }
			}
			//ignore other entities... there shouldn't be any
		}
	}
	else if(_rayCastMode == Click)
	{
		qmlManager->backgroundClicked();
	}
}


void RayCastController::hoverHitsChanged(const Qt3DRender::QAbstractRayCaster::Hits& hits)
{
	if (hits.size() > 0)
	{
		for (auto hit : hits)
		{
			auto hovered = dynamic_cast<Hoverable*>(hit.entity());
			if (hovered)
			{
				if (_hovered != hovered)
				{
					if (_hovered)
					{
						_hovered->onExited();
					}
					_hovered = hovered;
					_hovered->onEntered();
				}
				break;
			}
		}
	}
	else
	{
		if (_hovered)
		{
			_hovered->onExited();
			_hovered = nullptr;
		}
	}
	_hoverRaycastBusy = false;
}


MouseEventData::MouseEventData(Qt3DInput::QMouseEvent* v) : button(v->button()), modifiers(v->modifiers())
{
	position = {v->x(), v->y() };
}

MouseEventData::MouseEventData()
{
}
