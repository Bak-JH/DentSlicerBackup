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

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QCursor cursorTarget1 = QCursor(QPixmap(":/resource/cursor.png"));
    QCursor cursorTarget2 = QCursor(QPixmap(":/resource/pen.png"));
    app.setOverrideCursor(cursorTarget1);
    //app.changeOverrideCursor(cursorTarget2);
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
    //GLModel* ggglmodel =new GLModel(nullptr,3);

    QObject *item = FindItemByName(&engine, "item");
    QObject *curve = FindItemByName(&engine, "curve");
    QObject *flat = FindItemByName(&engine, "flat");
    QObject *slider = FindItemByName(&engine, "sslider");
    qDebug() << flat ;
    Lights* lights = new Lights(teethModel);

    QObject::connect(item,SIGNAL(qmlSignal()),gglmodel,SLOT(modelcut()));
    QObject::connect(curve,SIGNAL(curveSignal()),gglmodel,SLOT(lineAccept()));
    QObject::connect(flat,SIGNAL(flatSignal()),gglmodel,SLOT(pointAccept()));
    QObject::connect(slider,SIGNAL(govalue(double)),gglmodel,SLOT(getSignal(double)));
    QObject::connect(se.data(), SIGNAL(updateModelInfo(int,int,QString,float)), qm.data(), SLOT(sendUpdateModelInfo(int,int,QString,float)));


    foreach(QObject* obj, ((QObject*)teethModel)->children()){

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}}



