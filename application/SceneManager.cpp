#include "SceneManager.h"
#include "ApplicationManager.h"
#include "../Qml/util/QMLUtil.h"
#include "../glmodel.h"
#include "../render/lights.h"
#include <qcamera.h>
#include <qcameralens.h>
#include <qquickitem.h>
#include "../feature/interfaces/WidgetMode.h"
Hix::Application::SceneManager::SceneManager()
{
}

QVector2D Hix::Application::SceneManager::worldToScreen(QVector3D target)
{
	target = target * _systemTransform->scale3D();
	auto theta = -1 * _systemTransform->rotationX() / 180.0 * M_PI;
	auto alpha = -1 * _systemTransform->rotationZ() / 180.0 * M_PI;
	target = QVector3D(target.x() * std::cos(alpha) + target.y() * std::sin(alpha), target.x() * (-1) * std::sin(alpha) + target.y() * std::cos(alpha), target.z());
	QVector3D target3(target.x(), target.y() * std::cos(theta) + target.z() * std::sin(theta), (-1) * target.y() * std::sin(theta) + target.z() * std::cos(theta));

	QVector3D point2 (target3);
	QMatrix4x4 matrix = _lens->projectionMatrix() * _camera->viewMatrix();
	point2 = matrix * point2;
	point2.setX((point2.x() + 1) * _scene3d->width() / 2);
	point2.setY((-1 * point2.y() + 1) * _scene3d->height() / 2);
	return QVector2D(point2);
}

Qt3DCore::QEntity* Hix::Application::SceneManager::root()
{
	return _root;
}

Qt3DCore::QEntity* Hix::Application::SceneManager::total()
{
	return _total;
}

                                                                

QVector3D Hix::Application::SceneManager::cameraViewVector()const
{
	return _camera->position() - _systemTransform->translation();
}

const Qt3DCore::QTransform* Hix::Application::SceneManager::systemTransform() const
{
	return _systemTransform;
}

QQuickItem* Hix::Application::SceneManager::scene3d()
{
	return _scene3d;
}






void Hix::Application::SceneManagerLoader::init(SceneManager& manager, QObject* root)
{

	Hix::QML::getItemByID(root, manager._scene3d, "scene3d");
	Hix::QML::getItemByID(root, manager._root, "sceneRoot");
	Hix::QML::getItemByID(root, manager._total, "total");
	Hix::QML::getItemByID(root, manager._systemTransform, "systemTransform");
	Hix::QML::getItemByID(root, manager._camera, "camera");
	Hix::QML::getItemByID(root, manager._lens, "cameraLens");
	QMetaObject::invokeMethod(manager._root, "initCamera");

	//init lights
	Lights* lights = new Lights(manager._total);
	manager._bed.drawBed();

	//widget mode update when camera changed
	//QObject::connect(manager._root, SIGNAL(cameraViewChanged()), []() {
	//	Hix::Features::UpdateWidgetModePos();
	//});



}