#ifndef QMLMANAGER_H
#define QMLMANAGER_H


#include <QObject>
#include <QDebug>
#include <QString>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlEngine>
#include <QMouseEvent>
#include <QCoreApplication>
#include <QEvent>
#include <QPointF>
#include <QCursor>
#include <QQmlProperty>
#include "feature/shelloffset.h"
#include "glmodel.h"
#include "QtConcurrent/QtConcurrentRun"
#include "QFuture"
#include "input/raycastcontroller.h"
#include "feature/overhangDetect.h"
#include "Tasking/TaskManager.h"
#include "slice/SlicingOptBackend.h"
#include "support/SupportRaftManager.h"
#include "feature/FeatureHistoryManager.h"
#include "feature/interfaces/Feature.h"
#include "widget/GridBed.h"
#include "Settings/AppSetting.h"

namespace Hix
{
	namespace Features
	{
		class Feature;
	}
    namespace QML
    {
        class FeatureMenu;
    }
}
class QQuickItem;
class QmlManager : public QObject
{
    Q_OBJECT
public:

	template <typename F>
	static void postToObject(F&& fun, QObject* obj = qApp) {
		QMetaObject::invokeMethod(obj, std::forward<F>(fun));
	}

	template<typename FeatureType>
	bool isActive()
	{
		if (_currentMode.get() != nullptr)
		{
			return dynamic_cast<const FeatureType*>(_currentMode.get()) != nullptr;
		}
		return false;
	}
	bool isFeatureActive();
	template <typename F>
	static void postToThread(F&& fun, QThread* thread = qApp->thread()) {
		auto* obj = QAbstractEventDispatcher::instance(thread);
		Q_ASSERT(obj);
		QMetaObject::invokeMethod(obj, std::forward<F>(fun));
	}


    explicit QmlManager(QObject *parent = nullptr);

    // UI components
	Qt3DRender::QCamera* _camera;
    QQuickItem* featureArea;
    QQuickItem* popupArea;
    QObject* mainWindow;
	Qt3DCore::QEntity* total;

    Qt3DCore::QTransform* systemTransform;
	Qt3DCore::QEntity* mv;



    std::unordered_map<GLModel*, std::unique_ptr<GLModel>> glmodels;

    std::vector<size_t> copyMeshes;


	Hix::Input::RayCastController& getRayCaster();
	void cameraViewChanged();
	Hix::Features::Mode* currentMode()const;
	//TODO: temp
	void setMode(Hix::Features::Mode*);


	QVector2D world2Screen(QVector3D target);

    Q_INVOKABLE QString getVersion();

    Q_INVOKABLE void setHandCursor();
	Q_INVOKABLE void setEraserCursor();
    Q_INVOKABLE void setClosedHandCursor();
    Q_INVOKABLE void resetCursor();
	Q_INVOKABLE void settingFileChanged(QString path);


    void showCubeWidgets(GLModel* model);
	void addSupport(std::unique_ptr<Hix::Features::FeatureContainer> container);


	//void modelMoveWithAxis(QVector3D axis, double distance);
	//void modelMove(QVector3D displacement);
	QVector3D cameraViewVector();
	Hix::Tasking::TaskManager& taskManager();
	Hix::Support::SupportRaftManager& supportRaftManager();
	Hix::Features::FeatureHisroyManager& featureHistoryManager();
	Hix::Features::Mode* getCurrentMode();
	const Hix::Settings::AppSetting& settings()const;
	//tmp
	Hix::UI::GridBed _bed;
private:
	Hix::Tasking::TaskManager _taskManager;
	SlicingOptBackend _optBackend;
	Hix::Input::RayCastController _rayCastController;
	//cursors
	QCursor _cursorEraser;
	Hix::Support::SupportRaftManager _supportRaftManager;
	std::unique_ptr<Hix::Features::Mode> _currentMode;
	Hix::Features::FeatureHisroyManager _featureHistoryManager;
	Hix::Settings::AppSetting _setting;


public slots:
	void openSupport();
	void closeSupport();
	void clearSupports();
	void supportEditEnabled(bool enabled);
	void supportCancelEdit();
	void supportApplyEdit();
	void generateAutoSupport();
	void regenerateRaft();

};





extern QmlManager *qmlManager;

#endif //QMLMANAGER_H
