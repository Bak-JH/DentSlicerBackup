#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlEngine>
#include <QQmlContext>
#include "quaternionhelper.h"
#include "slicingengine.h"
#include "configuration.h"
#include "glmodel.h"
#include "qmlmanager.h"
#include "lights.h"
#include <QQuickView>

using namespace Qt3DCore;

//using namespace Qt3DRender;
//void makePlane(QVector3D& v1, QVector3D& v2, QVector3D& v3, QEntity* motherEntity);


int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);




    QScopedPointer<QuaternionHelper> qq(new QuaternionHelper);
    QScopedPointer<SlicingEngine> se(new SlicingEngine);
    QScopedPointer<QmlManager> qm(new QmlManager);

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    engine.rootContext()->setContextProperty("qm", qm.data());
    engine.rootContext()->setContextProperty("qq",qq.data());
    engine.rootContext()->setContextProperty("se",se.data());

    QObject* mainView = FindItemByName(&engine, "MainView");
    QEntity* teethModel = (QEntity *)FindItemByName(&engine, "normalModel");
    GLModel* gglmodel = new GLModel(teethModel);


    QObject *item = FindItemByName(&engine, "item");
    Lights* mylights = new Lights(teethModel);
//    QObject::connect(item,SIGNAL(qmlSignal()),cut,SLOT(makePlane()));

   QObject::connect(item,SIGNAL(qmlSignal()),gglmodel,SLOT(modelcut()));
    QObject::connect(se.data(), SIGNAL(updateModelInfo(int,int,QString,float)), qm.data(), SLOT(sendUpdateModelInfo(int,int,QString,float)));


    foreach(QObject* obj, ((QObject*)teethModel)->children()){

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}}



