#pragma once
#include <unordered_set>
#include <unordered_map>
#include <memory>
#include <qvector3d.h>
#include "../render/Bounds3D.h"
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
			enum class ViewPreset
			{
				Up,
				Down,
				Front,
				Back,
				Left,
				Right,
				Center
			};


			SceneManager();

			SceneManager(const SceneManager& other) = delete;
			SceneManager(SceneManager&& other) = delete;
			SceneManager& operator=(SceneManager other) = delete;
			SceneManager& operator=(SceneManager&& other) = delete;
			QVector2D worldToScreen(QVector3D target);
			Qt3DCore::QEntity* root();
			Qt3DCore::QEntity* total();
			QVector3D cameraViewVector()const;
			Qt3DCore::QTransform* systemTransform();
			QQuickItem* scene3d();
			void drawBed();
			void setViewPreset(ViewPreset preset);
			Qt3DRender::QCamera* camera();

		private:
			QObject* _dropArea;
			QQuickItem* _scene3d;
			Qt3DCore::QEntity* _root;
			Qt3DCore::QEntity* _mainView;
			Qt3DCore::QEntity* _total;
			Qt3DCore::QTransform* _systemTransform;
			Qt3DRender::QCamera* _camera;
			Qt3DRender::QCameraLens* _lens;
			Hix::UI::GridBed _bed;

		private slots:
			void fileDropped(QUrl fileUrl);
		public slots:
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

namespace magic_enum
{
	template<>
	struct name_formatter<Hix::Application::SceneManager::ViewPreset>
	{
		static constexpr name_option name_option = name_option::unchanged;
	};
}