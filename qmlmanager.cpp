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

    GLModel* glmodel = new GLModel(teethModel, nullptr, "C:/Users/user/Documents/diridiri/DLPslicer/partial2_flip.stl", false);
    //GLModel* glmodel = new GLModel(teethModel, nullptr, "C:/Users/user/Documents/diridiri/DLPslicer/partial2_flip.stl", false);//"/Users/Diridiri/Desktop/DLP/DLPslicer/partial2_flip.stl",false);//"C:/Users/jaine/workspace/DLPslicerResource/partial2_flip.stl", false);
    //GLModel* gglmodel = new GLModel(teethModel, "C:/Users/hsy61/Desktop/3D_models/teeth_models/lowerjaw.stl", false);


    GLModel* glmodel2 = new GLModel(teethModel, nullptr, "C:/Users/user/Documents/diridiri/DLPslicer/partial2_flip.stl", false);

    /*//* Arrange **********************************
    vector<Mesh> meshes_to_arrange;
    vector<XYArrangement> arng_result_set;
    meshes_to_arrange.push_back(* glmodel->mesh);
    meshes_to_arrange.push_back(* glmodel2->mesh);
    arng_result_set = arngMeshes(&meshes_to_arrange);
    vector<Qt3DCore::QTransform*> m_transform_set;
    m_transform_set.push_back(glmodel->m_transform);
    m_transform_set.push_back(glmodel2->m_transform);
    arrangeQt3D(m_transform_set, arng_result_set);*/

    // model cut components
    QObject *cutPopup = FindItemByName(engine, "cutPopup");
    QObject *curveButton = FindItemByName(engine, "curveButton");
    QObject *flatButton = FindItemByName(engine, "flatButton");
    QObject *slider = FindItemByName(engine, "slider");
    Lights* lights = new Lights(teethModel);

    // labelling components
    QObject *text3DInput = FindItemByName(engine, "text3DInput");
    QObject *labelPopup = FindItemByName(engine, "labelPopup");
    QObject *labelFontBox = FindItemByName(engine, "labelFontBox");

    // orientation components
    QObject* orientPopup = FindItemByName(engine, "orientPopup");
    QObject* progress_text = FindItemByName(engine, "progress_text");

    featureThread* ft = new featureThread(glmodel, ftrOrient);
    featureThread* ft2 = new featureThread(glmodel2, ftrOrient);

    QObject::connect(glmodel->ft, SIGNAL(setProgress(QVariant)),progress_text, SLOT(update_loading(QVariant)));
    QObject::connect(glmodel->ft, SIGNAL(loadPopup(QVariant)),orientPopup, SLOT(show_popup(QVariant)));

    // need to connect for every popup

    // model cut popup codes
    //QObject::connect(cutPopup,SIGNAL(modelCut()),glmodel->shadowModel , SLOT(modelCut()));
    QObject::connect(cutPopup,SIGNAL(rrunFeature(int)),glmodel->ft , SLOT(setTypeAndStart(int)));
    QObject::connect(cutPopup,SIGNAL(curveModeSelected()),glmodel->shadowModel,SLOT(lineAccept()));
    QObject::connect(cutPopup,SIGNAL(flatModeSelected()),glmodel->shadowModel,SLOT(pointAccept()));
    QObject::connect(slider,SIGNAL(govalue(double)),glmodel->ft->ct,SLOT(getSliderSignal(double)));

    // auto orientation popup codes
    QObject::connect(orientPopup, SIGNAL(runFeature(int)), glmodel->ft, SLOT(setTypeAndStart(int)));

    // label popup codes
    QObject::connect(text3DInput, SIGNAL(sendTextChanged(QString, int)),glmodel->shadowModel,SLOT(getTextChanged(QString, int)));
    QObject::connect(labelPopup, SIGNAL(openLabelling()),glmodel->shadowModel,SLOT(openLabelling()));
    QObject::connect(labelPopup, SIGNAL(closeLabelling()),glmodel->shadowModel,SLOT(closeLabelling()));
    //QObject::connect(labelPopup, SIGNAL(runFeature(int)),glmodel->ft, SLOT(setTypeAndStart(int)));
    QObject::connect(labelPopup, SIGNAL(generateText3DMesh()), glmodel->shadowModel, SLOT(generateText3DMesh()));
    QObject::connect(labelFontBox, SIGNAL(sendFontName(QString)),glmodel->shadowModel, SLOT(getFontNameChanged(QString)));

    // auto Repair

    // auto Arrange

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

