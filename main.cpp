#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlEngine>
#include <QQmlContext>
#include "quaternionhelper.h"
#include "slicingengine.h"
#include "configuration.h"
#include "glmodel.h"
#include "qmlmanager.h"



using namespace Qt3DCore;
//using namespace Qt3DRender;

static QObject* FindItemByName(QList<QObject*> nodes, const QString& name);
static QObject* FindItemByName(QQmlApplicationEngine* engine, const QString& name);

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    qmlRegisterType<GLModel>("GLQML", 1, 0, "GLModel");
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


    QObject::connect(se.data(), SIGNAL(updateModelInfo(int,int,QString,float)), qm.data(), SLOT(sendUpdateModelInfo(int,int,QString,float)));


    foreach(QObject* obj, ((QObject*)teethModel)->children()){
        //qDebug() << (*((QEntity*)obj)).components()[0];
        /*foreach (QComponent* component, (*((QEntity*)obj)).components()){
            qDebug() << component->objectName();
        }*/
    }

    /*
    foreach(QComponent* component, teethModel->components()){
        qDebug() << (*component).objectName();
    }*/

    /*QComponentVector::iterator i;
    QComponentVector teethModelComponents = teethModel->components();
    for (i=teethModelComponents.begin(); i!=teethModelComponents.end(); ++i ){
        qDebug() << (*i);
    }*/

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}

static QObject* FindItemByName(QList<QObject*> nodes, const QString& name)
{
    for(int i = 0; i < nodes.size(); i++){
        // search for node
        if (nodes.at(i) && nodes.at(i)->objectName() == name){
            return dynamic_cast<QObject*>(nodes.at(i));
        }
        // search in children
        else if (nodes.at(i) && nodes.at(i)->children().size() > 0){
            QObject* item = FindItemByName(nodes.at(i)->children(), name);
            if (item)
                return item;
        }
    }
    // not found
    return NULL;
}

static QObject* FindItemByName(QQmlApplicationEngine* engine, const QString& name)
{
    return FindItemByName(engine->rootObjects(), name);
}
