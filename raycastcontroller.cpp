#include "raycastcontroller.h"
#include <QAbstractRayCaster>
#include <qscreenraycaster.h>
#include <qmousehandler.h>
#include <qcamera.h>
#include <qmouseevent.h>
#include <qmousedevice.h>
#include <qentity.h>
#include "qmlmanager.h"
using namespace Qt3DRender;
using namespace Qt3DInput;
using namespace Qt3DCore;
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


void RayCastController::mouseReleased(Qt3DInput::QMouseEvent* mouse)
{
	if (mouse->button() == Qt3DInput::QMouseEvent::Buttons::LeftButton)
	{
		_rayCaster->trigger(QPoint(mouse->x(), mouse->y()));
	}
}



void RayCastController::hitsChanged(const Qt3DRender::QAbstractRayCaster::Hits& hits)
{
	qDebug() << "ray cast hit detected: " << hits.size();
	bool hitGLModel = false;

	for (auto& hit : hits)
	{
		auto glModel = dynamic_cast<GLModel*>(hit.entity());
		if (glModel)
		{
			hitGLModel = true;
			break;
		}


	}

	if (!hitGLModel)
	{
		qmlManager->backgroundClicked();

	}
}
