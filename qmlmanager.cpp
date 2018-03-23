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

    //rotation Sphere
    rotateSphere = (QEntity *)FindItemByName(engine, "rotateSphereEntity");
    rotateSphereX = (QEntity *)FindItemByName(engine, "rotateSphereTorusX");
    rotateSphereY = (QEntity *)FindItemByName(engine, "rotateSphereTorusY");
    rotateSphereZ = (QEntity *)FindItemByName(engine, "rotateSphereTorusZ");
    QObject* rotateSphereobj = FindItemByName(engine, "rotateSphere");
    QObject::connect(rotateSphereobj, SIGNAL(rotateSignal(int,int)),this, SLOT(modelRotate(int,int)));
    rotateSphere->setEnabled(0);
    QObject *rotateButton = FindItemByName(engine, "rotateButton");
    QObject::connect(rotateButton,SIGNAL(runGroupFeature(int,QString)),this,SLOT(runGroupFeature(int,QString)));
}

void QmlManager::openModelFile(QString fname){

    GLModel* glmodel = new GLModel(models, nullptr, fname, false);

    glmodels.push_back(glmodel);
    glmodel->moveModelMesh(QVector3D(
                           (-1)*glmodel->mesh->x_min,
                           (-1)*glmodel->mesh->y_min,
                           (-1)*glmodel->mesh->z_min));
    // set initial position

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


void QmlManager::ModelVisible(int ID, bool isVisible){
    GLModel* target;
    for(int i=0; i<glmodels.size();i++){
        if(glmodels.at(i)->ID == ID){
            target = glmodels.at(i);
            break;
        }
    }
    target->setEnabled(isVisible);

void QmlManager::showRotateSphere(){
    rotateSphere->setEnabled(1);
    //QFrameAction::QFrameAction qframeaction(rotateSphere);
}
void QmlManager::modelRotate(int Axis, int Angle){
    //QMatrix4x4 tmpMatrix = glmodel->m_transform->matrix();
    switch(Axis){
    case 1:{  //X
        //float tmpx = glmodel->m_transform->rotationX();
        //glmodel->m_transform->setRotationX(tmpx+Angle);
        break;
    }
    case 2:{  //Y
        //float tmpy = glmodel->m_transform->rotationY();
        //glmodel->m_transform->setRotationY(tmpy+Angle);
        break;
    }
    case 3:{  //Z
        //float tmpz = glmodel->m_transform->rotationZ();
        //glmodel->m_transform->setRotationZ(tmpz+Angle);
        break;
    }
    }
    //qDebug() << glmodel->m_transform->rotationX() << Angle;
    //glmodel->m_transform->setMatrix(tmpMatrix);
    qDebug() << "Angle Monitor" << Angle;
}
void QmlManager::runGroupFeature(int ftrType, QString state){
    showRotateSphere();
    switch(ftrType){
    case 5: //rotate
    {
        qDebug()<<state;
        if (state == "active"){
            rotateSphere->setEnabled(0);
        }else if(state == "inactive"){
            showRotateSphere();
        }
        break;
    }
    case 4:  //move
    {
        break;
    }
    }

}
