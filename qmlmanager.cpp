#include "qmlmanager.h"
#include "lights.h"
#include "glmodel.h"

QmlManager::QmlManager(QObject *parent) : QObject(parent)
{
}

void QmlManager::initializeUI(QQmlApplicationEngine* e){
    engine = e;
    QObject* mainView = FindItemByName(engine, "MainView");
    QEntity* teethModel = (QEntity *)FindItemByName(engine, "model");
    GLModel* gglmodel = new GLModel(teethModel, "/Users/Diridiri/Desktop/DLP/DLPslicer/partial2_flip.stl",false);//"C:/Users/jaine/workspace/DLPslicerResource/partial2_flip.stl", false);

    QObject *item = FindItemByName(engine, "item");
    QObject *curve = FindItemByName(engine, "curve");
    QObject *flat = FindItemByName(engine, "flat");
    QObject *slider = FindItemByName(engine, "sslider");
    QObject *text3DInput = FindItemByName(engine, "text3DInput");
    QObject *labellingPopup = FindItemByName(engine, "labellingPopup");
    QObject *labelFontBox = FindItemByName(engine, "labelFontBox");
    Lights* lights = new Lights(teethModel);
    QObject* autoorientButton = FindItemByName(engine, "autoorientButton");
    QObject* progress_text = FindItemByName(engine, "progress_text");
    orientThread* ot=new orientThread(gglmodel);

    QObject::connect(gglmodel->m_autoorientation, SIGNAL(setProgress(QVariant)),progress_text, SLOT(update_loading(QVariant)));
    QObject::connect(ot, SIGNAL(loadPopup(QVariant)),autoorientButton, SLOT(show_popup(QVariant)));
    QObject::connect(autoorientButton, SIGNAL(autoOrientSignal()),ot, SLOT(start()));
    QObject::connect(item,SIGNAL(qmlSignal()),gglmodel,SLOT(modelcut()));
    QObject::connect(curve,SIGNAL(curveSignal()),gglmodel,SLOT(lineAccept()));
    QObject::connect(flat,SIGNAL(flatSignal()),gglmodel,SLOT(pointAccept()));
    QObject::connect(slider,SIGNAL(govalue(double)),gglmodel,SLOT(getSignal(double)));
    QObject::connect(text3DInput, SIGNAL(sendTextChanged(QString)),gglmodel->shadowModel,SLOT(getTextChanged(QString)));
    QObject::connect(labellingPopup, SIGNAL(openLabelling()),gglmodel->shadowModel,SLOT(openLabelling()));
    QObject::connect(labellingPopup, SIGNAL(closeLabelling()),gglmodel->shadowModel,SLOT(closeLabelling()));
    QObject::connect(labellingPopup, SIGNAL(generateText3DMesh()),gglmodel->shadowModel, SLOT(generateText3DMesh()));
    QObject::connect(labelFontBox, SIGNAL(sendFontName(QString)),gglmodel->shadowModel, SLOT(getFontNameChanged(QString)));
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

