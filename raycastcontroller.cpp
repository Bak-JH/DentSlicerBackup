#include "raycastcontroller.h"
#include <QAbstractRayCaster>
#include <qscreenraycaster.h>
#include <qmousehandler.h>
#include <qcamera.h>
#include <qmousedevice.h>
#include <qentity.h>
#include "qmlmanager.h"
using namespace Qt3DRender;
using namespace Qt3DInput;
using namespace Qt3DCore;

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
	auto dbg = camera->components();
	_mouseHandler->setSourceDevice(new QMouseDevice());
	QObject::connect(_mouseHandler, SIGNAL(released(Qt3DInput::QMouseEvent *)), this, SLOT(mouseReleased(Qt3DInput::QMouseEvent *)));
	QObject::connect(_mouseHandler, SIGNAL(pressed(Qt3DInput::QMouseEvent*)), this, SLOT(mousePressed(Qt3DInput::QMouseEvent*)));
	QObject::connect(_mouseHandler, SIGNAL(positionChanged(Qt3DInput::QMouseEvent*)), this, SLOT(mousePositionChanged(Qt3DInput::QMouseEvent*)));

	QObject::connect(_rayCaster, SIGNAL(hitsChanged(const Qt3DRender::QAbstractRayCaster::Hits&)), this, SLOT(hitsChanged(const Qt3DRender::QAbstractRayCaster::Hits&)));

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
	_latestEvent = mouse;
	_pressedTime = std::chrono::system_clock::now();
	_pressedPt = { mouse->x(), mouse->y() };
}

void RayCastController::mouseReleased(Qt3DInput::QMouseEvent* mouse)
{
	_latestEvent = mouse;
	if (isClick({ mouse->x(), mouse->y() }) && mouse->button() == Qt3DInput::QMouseEvent::Buttons::LeftButton)
	{
		_rayCaster->trigger(QPoint(mouse->x(), mouse->y()));
	}
}

void RayCastController::mousePositionChanged(Qt3DInput::QMouseEvent* mouse)
{

}


void RayCastController::hitsChanged(const Qt3DRender::QAbstractRayCaster::Hits& hits)
{
	qDebug() << "ray cast hit detected: " << hits.size();
	bool hitGLModel = false;

	for (auto& hit : hits)
	{
		auto glModel = dynamic_cast<GLModel*>(hit.entity());
		size_t primitiveIdx = hit.primitiveIndex();
		if (glModel && glModel->isHitTestable())
		{
			hitGLModel = true;
			qmlManager->modelSelected(glModel->ID);
		}


	}

	if (!hitGLModel)
	{
		qmlManager->backgroundClicked();

	}
}
