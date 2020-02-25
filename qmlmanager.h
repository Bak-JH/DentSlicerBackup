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
#include <QQmlProperty>
#include "feature/shelloffset.h"
#include "glmodel.h"
#include "QFuture"
#include "feature/overhangDetect.h"
#include "slice/SlicingOptBackend.h"
#include "feature/FeatureHistoryManager.h"
#include "feature/interfaces/Feature.h"

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



   explicit QmlManager(QObject *parent = nullptr);

	//Q_INVOKABLE void settingFileChanged(QString path);


	void addSupport(std::unique_ptr<Hix::Features::FeatureContainer> container);


	//void modelMoveWithAxis(QVector3D axis, double distance);
	//void modelMove(QVector3D displacement);

	//tmp
private:
	//cursors




public slots:

	void regenerateRaft();

};





extern QmlManager *qmlManager;

#endif //QMLMANAGER_H
