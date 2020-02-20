#pragma once

#include <QAbstractRayCaster>
#include <Qt3DRender>
#include <qmouseevent.h>
#include <vector>
#include <qscreenraycaster.h>
#include <qmousehandler.h>
#include <qcamera.h>
#include <qmousedevice.h>
#include <qentity.h>
#include <future>
#include <condition_variable>



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
namespace Hix
{
	namespace Input
	{
		class Draggable;
		class Hoverable;

		//cause holding 
		struct MouseEventData
		{
			QPoint position;
			Qt3DInput::QMouseEvent::Buttons button;
			Qt3DInput::QMouseEvent::Modifiers modifiers;
		public:
			MouseEventData(Qt3DInput::QMouseEvent*);
			MouseEventData();
		};

		//if the code size increases, use inheritance with virtual interface
		class RayCastController : public QObject
		{
			Q_OBJECT
		public:
			RayCastController();
			//for ray casting optimization using the bounding box
			//Qt3DRender::QLayer _modelLayer;
			void addInputLayer(Qt3DRender::QLayer* layer);
			void removeInputLayer(Qt3DRender::QLayer* layer);
			void addHoverLayer(Qt3DRender::QLayer* layer);
			void removeHoverLayer(Qt3DRender::QLayer* layer);

			void setHoverEnabled(bool isEnabled);
			bool hoverEnabled()const;
			bool mousePosInBound(const Qt3DInput::QMouseEvent* mv);
			//returns true if successfully dragged, false otherwise
			//to remove delete object reference
			void hoverObjectDeleted(Hix::Input::Hoverable* justDeleted);
			void draggableObjectDeleted(Hix::Input::Draggable* justDeleted);


		public slots:
			void mousePressed(Qt3DInput::QMouseEvent* mouse);
			void mouseReleased(Qt3DInput::QMouseEvent* mouse);
			void mousePositionChanged(Qt3DInput::QMouseEvent* mouse);

			void hitsChanged(const Qt3DRender::QAbstractRayCaster::Hits& hits);
			void hoverHitsChanged(const Qt3DRender::QAbstractRayCaster::Hits& hits);

		private:
			bool verifyClick();
			Qt3DCore::QEntity* _camera = nullptr;
			enum RayCastMode
			{
				Click = 0,
				Drag
			};
			//click detection
			QPoint _pressedPt;
			std::condition_variable _isClickVerified;
			std::mutex _clickVerifyMtx;
			//bool _isPressed = false;
			bool _mouseBusy = false;
			std::future<bool> _verifyClickTask;
			Qt3DInput::QMouseDevice _mouseDevice;
			Qt3DRender::QScreenRayCaster _rayCaster;
			//for objects that responds to hover, only for widget...for now.
			Qt3DRender::QScreenRayCaster _hoverRayCaster;
			Qt3DInput::QMouseHandler _mouseHandler;
			RayCastMode _rayCastMode;
			MouseEventData _mouseEvent;
			bool _hoverEnabled = false;
			//no need for atomic or lock, raycast callback already in main thread, same for mouse events
			bool _hoverRaycastBusy = false;
			Hix::Input::Draggable*  _dragged = nullptr;
			Hix::Input::Hoverable* _hovered = nullptr;

			static const std::chrono::milliseconds MAX_CLICK_DURATION;
			static const float MAX_CLICK_MOVEMENT;
			friend class RayCastControllerLoader;
		};

		class RayCastControllerLoader
		{
		private:
			static void init(RayCastController& manager, Qt3DCore::QEntity* parentEntity);
			friend class Hix::Application::ApplicationManager;
		};




	}
}
