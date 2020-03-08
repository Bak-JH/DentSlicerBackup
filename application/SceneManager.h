#pragma once
#include <unordered_set>
#include <unordered_map>
#include <memory>
#include <qvector3d.h>
#include "../DentEngine/src/Bounds3D.h"
#include "../widget/GridBed.h"
class QQuickItem;
class GLModel;
namespace Qt3DCore
{
	class QEntity;
	class QTransform;
}
namespace Qt3DRender
{
	class QCamera;
	class QCameraLens;
}
namespace Hix
{
	namespace QML
	{
		class PartList;
		class PrintInfo;
	}
	namespace Features
	{
		class ListModel;
	}
	namespace Application
	{
		class ApplicationManager;
		//temp qobject to get qml side signal
		class SceneManager: QObject
		{
			Q_OBJECT
		public:
			SceneManager();

			SceneManager(const SceneManager& other) = delete;
			SceneManager(SceneManager&& other) = delete;
			SceneManager& operator=(SceneManager other) = delete;
			SceneManager& operator=(SceneManager&& other) = delete;
			QVector2D worldToScreen(QVector3D target);
			Qt3DCore::QEntity* root();
			Qt3DCore::QEntity* total();
			QVector3D cameraViewVector()const;
			const Qt3DCore::QTransform* systemTransform()const;
			QQuickItem* scene3d();
			void drawBed();
		private:
			QQuickItem* _scene3d;
			Qt3DCore::QEntity* _root;
			Qt3DCore::QEntity* _mainView;
			Qt3DCore::QEntity* _total;
			Qt3DCore::QTransform* _systemTransform;
			Qt3DRender::QCamera* _camera;
			Qt3DRender::QCameraLens* _lens;
			Hix::UI::GridBed _bed;

		private slots:
			void onCameraChanged();


			friend class SceneManagerLoader;
		};

		class SceneManagerLoader
		{
		private:
			static void init(SceneManager& manager, QObject* root);
			friend class Hix::Application::ApplicationManager;
		};





	}
}
