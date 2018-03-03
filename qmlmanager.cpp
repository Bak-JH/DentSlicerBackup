#include "qmlmanager.h"
#include "lights.h"
#include "glmodel.h"
#include <QQmlApplicationEngine>
#include <QQmlEngine>


QmlManager::QmlManager(QObject *parent) : QObject(parent)
{
}

void QmlManager::initializeUI(QQmlApplicationEngine* e){
    engine = e;
    QObject* mainView = FindItemByName(engine, "MainView");
    QEntity* teethModel = (QEntity *)FindItemByName(engine, "model");
    GLModel* gglmodel = new GLModel(teethModel, "C:/Users/jaine/workspace/DLPslicerResource/partial2_flip.stl", false);

    QObject *item = FindItemByName(engine, "item");
    QObject *curve = FindItemByName(engine, "curve");
    QObject *flat = FindItemByName(engine, "flat");
    QObject *slider = FindItemByName(engine, "sslider");
    Lights* lights = new Lights(teethModel);

    QObject::connect(item,SIGNAL(qmlSignal()),gglmodel,SLOT(modelcut()));
    QObject::connect(curve,SIGNAL(curveSignal()),gglmodel,SLOT(lineAccept()));
    QObject::connect(flat,SIGNAL(flatSignal()),gglmodel,SLOT(pointAccept()));
    QObject::connect(slider,SIGNAL(govalue(double)),gglmodel,SLOT(getSignal(double)));
}


void QmlManager::sendUpdateModelInfo(int printing_time, int layer, QString xyz, float volume){
    updateModelInfo(printing_time, layer, xyz, volume);
}


QObject* FindItemByName(QList<QObject*> nodes, const QString& name)
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

QObject* FindItemByName(QQmlApplicationEngine* engine, const QString& name)
{
    return FindItemByName(engine->rootObjects(), name);
}

