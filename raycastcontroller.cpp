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
	//_rayCaster->addLayer(layer);

}

void RayCastController::removeLayer(Qt3DRender::QLayer* layer)
{
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
//	_pressedTime = std::chrono::system_clock::now();
//	_pressedPt = { mouse->x(), mouse->y() };
//	//trying to tell if the click is a drag.
//	if (!qmlManager->selectedModels.empty())
//	{
//		_rayCastMode = RayCastMode::Other;
//		_rayCaster->trigger(QPoint(mouse->x(), mouse->y()));
//	}
	if (_rayCasterBusy)
		return;
	_rayCasterBusy = true;
	_mouseEvent = MouseEventData(mouse);
	_rayCastMode = RayCastMode::Pressed;
	_rayCaster->trigger(QPoint(mouse->x(), mouse->y()));
}

void RayCastController::mouseReleased(Qt3DInput::QMouseEvent* mouse)
{
	//_releasePt = { mouse->x(), mouse->y() };
	//if (isClick(_releasePt))
	//{
	//	_rayCastMode = RayCastMode::Click;
	//	_rayCaster->trigger(QPoint(mouse->x(), mouse->y()));
	//}
	if (_rayCasterBusy)
		return;
	_rayCasterBusy = true;
	_mouseEvent = MouseEventData(mouse);
	_rayCastMode = RayCastMode::Released;
	_rayCaster->trigger(QPoint(mouse->x(), mouse->y()));
}

void RayCastController::mousePositionChanged(Qt3DInput::QMouseEvent* mouse)
{
	//if (_rayCasterBusy)
	//	return;
	//_rayCasterBusy = true;
	//disable move when in certain modes;
	if (qmlManager->orientationActive ||
		qmlManager->rotateActive ||
		qmlManager->saveActive)
		return;

	_mouseEvent = MouseEventData(mouse);

	//only call mouseMoved to selected models that are NOT doing certain features
	for (auto selected : qmlManager->selectedModels)
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
bool RayCastController::isSelected(const GLModel* model)
{
	auto findItr = std::find(qmlManager->selectedModels.cbegin(), qmlManager->selectedModels.cend(), model);
	if(findItr != qmlManager->selectedModels.cend())
	{
		return true;
	}
	return false;
}


void RayCastController::hitsChanged(const Qt3DRender::QAbstractRayCaster::Hits& hits)
{
	auto allHits = _rayCaster->hits();
	if (hits.size() > 0)
	{
		for (auto& hit : hits)
		{
			auto hitType = hit.type();
			auto etc = hit.entity();
			auto id = etc->id();
			auto name = etc->objectName();
			qDebug() << "ray cast hit something: -------!";
			qDebug() << hitType << etc << id << name;


			auto mesh = dynamic_cast<QMesh*>(etc->components()[0]);
			auto renderer = dynamic_cast<QGeometryRenderer*>(mesh);
			if (renderer)
			{
				auto indBufferOffset = renderer->indexBufferByteOffset();
				auto indOffset = renderer->indexOffset();
				auto restart = renderer->restartIndexValue();

				auto geo = renderer->geometry();
				auto attrbts = geo->attributes();

				QAttribute* pos = nullptr;
				QAttribute* norm = nullptr;
				QAttribute* colr = nullptr;
				QAttribute* idx = nullptr;

				for (auto& each : attrbts)
				{
					if (each->name() == QAttribute::defaultPositionAttributeName())
					{
						pos = each;
					}
					else if (each->name() == QAttribute::defaultNormalAttributeName())
					{
						norm = each;
					}
					else if (each->name() == QAttribute::defaultColorAttributeName())
					{
						colr = each;
					}
					else if (each->attributeType() == QAttribute::AttributeType::IndexAttribute)
					{
						idx = each;
					}

				}
				auto count = idx->count();
				auto normBuffer = norm->buffer();
				auto normArr = normBuffer->data();
				float* reNormArr = reinterpret_cast<float*>(normArr.data());


				auto type = idx->vertexBaseType();
				auto idxBuffer = idx->buffer();
				auto bufferType = idxBuffer->type();
				auto byteOffset = idx->byteOffset();
				auto usage = idxBuffer->usage();

				auto indxArray = idxBuffer->data();
				auto stride = idx->byteStride();
				size_t primitiveIdx = hit.primitiveIndex();

			}
		}
		for (auto hit : hits)
		{

			auto glModel = dynamic_cast<GLModel*>(hit.entity());
			if (glModel)
			{
				qDebug() << "ray trace glmodel" << glModel;
				if (_rayCastMode == Pressed)
				{
					glModel->mousePressed(_mouseEvent, hit);
				}
				else
				{
					glModel->mouseReleased(_mouseEvent, hit);
				}
			}
			//auto isAlreadySelected = isSelected(glModel);
			//if (_rayCastMode == RayCastMode::Other)
			//{
			//	if (isAlreadySelected)
			//	{
			//		//do other stuff like drag-move
			//	}
			//}
			//else
			//{
			//	if (isAlreadySelected)
			//	{
			//		//unselect
			//	}
			//	else
			//	{
			//		//select
			//	}
			//}
		}

	}
	else
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
