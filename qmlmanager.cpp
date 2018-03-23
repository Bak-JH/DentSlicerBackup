/*
 * qmlmanager.cpp
 * : handles UI components (QML components) including Generation, Translation, Connections(sig&slot)
 *
 *
 *
 *
 *
*/



#include "qmlmanager.h"
#include "lights.h"

QmlManager::QmlManager(QObject *parent) : QObject(parent)
{

}

void QmlManager::initializeUI(QQmlApplicationEngine* e){
    engine = e;
    mainWindow = FindItemByName(engine, "mainWindow");
    models = (QEntity *)FindItemByName(engine, "Models");
    Lights* lights = new Lights(models);

    //openModelFile("C:/Users/user/Documents/diridiri/DLPslicer/partial2_flip.stl");
    openModelFile("D:/Dev/2018/DLPslicer/partial2_flip.stl");
}



void QmlManager::openModelFile(QString fname){

    GLModel* glmodel = new GLModel(models, nullptr, fname, false);

    glmodels.push_back(glmodel);

    // auto Repair

    // auto Arrange

    // Arrange **********************************
    if (glmodels.size()>=2){
        vector<Mesh> meshes_to_arrange;
        vector<XYArrangement> arng_result_set;
        vector<Qt3DCore::QTransform*> m_transform_set;
        for (int i=0; i<glmodels.size(); i++){
            meshes_to_arrange.push_back(*(glmodels[i]->mesh));
            m_transform_set.push_back(glmodels[i]->m_transform);
        }
        arng_result_set = arngMeshes(&meshes_to_arrange);
        arrangeQt3D(m_transform_set, arng_result_set);
    }

    // model cut components
    QObject *cutPopup = FindItemByName(engine, "cutPopup");
    QObject *curveButton = FindItemByName(engine, "curveButton");
    QObject *flatButton = FindItemByName(engine, "flatButton");
    QObject *slider = FindItemByName(engine, "slider");

    // labelling components
    QObject *text3DInput = FindItemByName(engine, "text3DInput");
    QObject *labelPopup = FindItemByName(engine, "labelPopup");
    QObject *labelFontBox = FindItemByName(engine, "labelFontBox");

    // orientation components
    QObject* orientPopup = FindItemByName(engine, "orientPopup");
    QObject* progress_text = FindItemByName(engine, "progress_text");

    featureThread* ft = new featureThread(glmodel, ftrOrient);

    QObject::connect(glmodel->ft, SIGNAL(setProgress(QVariant)),progress_text, SLOT(update_loading(QVariant)));
    QObject::connect(glmodel->ft, SIGNAL(loadPopup(QVariant)),orientPopup, SLOT(show_popup(QVariant)));

    // need to connect for every popup

    // model cut popup codes
    QObject::connect(cutPopup,SIGNAL(generatePlane()),glmodel->shadowModel , SLOT(generatePlane()));
    QObject::connect(cutPopup,SIGNAL(modelCut()),glmodel->shadowModel , SLOT(modelCut()));
    //QObject::connect(cutPopup,SIGNAL(runFeature(int)),glmodel->ft , SLOT(setTypeAndStart(int)));
    QObject::connect(cutPopup,SIGNAL(curveModeSelected()),glmodel->shadowModel,SLOT(lineAccept()));
    QObject::connect(cutPopup,SIGNAL(flatModeSelected()),glmodel->shadowModel,SLOT(pointAccept()));
    QObject::connect(slider, SIGNAL(govalue(double)), glmodel->shadowModel, SLOT(getSliderSignal(double)));
    //QObject::connect(slider,SIGNAL(govalue(double)),glmodel->ft->ct,SLOT(getSliderSignal(double)));

    // auto orientation popup codes
    QObject::connect(orientPopup, SIGNAL(runFeature(int)), glmodel->ft, SLOT(setTypeAndStart(int)));

    // label popup codes
    QObject::connect(text3DInput, SIGNAL(sendTextChanged(QString, int)),glmodel->shadowModel,SLOT(getTextChanged(QString, int)));
    QObject::connect(labelPopup, SIGNAL(openLabelling()),glmodel->shadowModel,SLOT(openLabelling()));
    QObject::connect(labelPopup, SIGNAL(closeLabelling()),glmodel->shadowModel,SLOT(closeLabelling()));
    //QObject::connect(labelPopup, SIGNAL(runFeature(int)),glmodel->ft, SLOT(setTypeAndStart(int)));
    QObject::connect(labelPopup, SIGNAL(generateText3DMesh()), glmodel->shadowModel, SLOT(generateText3DMesh()));
    QObject::connect(labelFontBox, SIGNAL(sendFontName(QString)),glmodel->shadowModel, SLOT(getFontNameChanged(QString)));

    // auto Repair popup codes

    // auto Arrange popup codes
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

