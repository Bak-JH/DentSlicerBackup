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

    // model cut components
    cutPopup = FindItemByName(engine, "cutPopup");
    curveButton = FindItemByName(engine, "curveButton");
    flatButton = FindItemByName(engine, "flatButton");
    slider = FindItemByName(engine, "slider");

    // labelling components
    text3DInput = FindItemByName(engine, "text3DInput");
    labelPopup = FindItemByName(engine, "labelPopup");
    labelFontBox = FindItemByName(engine, "labelFontBox");

    // orientation components
    orientPopup = FindItemByName(engine, "orientPopup");
    progress_text = FindItemByName(engine, "progress_text");

    // repair components
    repairPopup = FindItemByName(engine, "repairPopup");


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

    //openModelFile("C:/Users/diridiri/Desktop/DLP/DLPslicer/partial2_flip.stl");
}

void QmlManager::openModelFile(QString fname){

    GLModel* glmodel = new GLModel(models, nullptr, fname, false);

    glmodels.push_back(glmodel);

    // set initial position
    float xmid = (glmodel->mesh->x_max + glmodel->mesh->x_min)/2;
    float ymid = (glmodel->mesh->y_max + glmodel->mesh->y_min)/2;
    float zmid = (glmodel->mesh->z_max + glmodel->mesh->z_min)/2;
    float zlength = (glmodel->mesh->z_max - glmodel->mesh->z_min);
    glmodel->moveModelMesh(QVector3D(
                           (-1)*xmid,
                           (-1)*ymid,
                           (-1)*zmid));
    glmodel->m_transform->setTranslation(QVector3D(0,0,zlength/2));

    // auto Repair

    // auto arrange components
    glmodels_arranged = false;
    QObject* arrangePopup = FindItemByName(engine, "arrangePopup");
    //QObject* progress_text = FindItemByName(engine, "progress_text"); //orientation와 공유
    QObject::connect(glmodel->arsignal, SIGNAL(runArrange()), this, SLOT(runArrange()));

    QObject::connect(glmodel->ft, SIGNAL(setProgress(QVariant)),progress_text, SLOT(update_loading(QVariant)));
    QObject::connect(glmodel->ft, SIGNAL(loadPopup(QVariant)),orientPopup, SLOT(show_popup(QVariant)));

    // need to connect for every popup

    // model cut popup codes
    QObject::connect(cutPopup,SIGNAL(generatePlane()),glmodel->shadowModel , SLOT(generatePlane()));
    //QObject::connect(cutPopup,SIGNAL(modelCut()),glmodel->shadowModel , SLOT(modelCut()));
    QObject::connect(cutPopup,SIGNAL(modelCutFinish()),glmodel->shadowModel , SLOT(modelCutFinished()));
    //QObject::connect(cutPopup,SIGNAL(runFeature(int)),glmodel->ft , SLOT(setTypeAndStart(int)));
    QObject::connect(cutPopup,SIGNAL(cutModeSelected(int)),glmodel->shadowModel,SLOT(cutModeSelected(int)));
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
    QObject::connect(repairPopup, SIGNAL(runFeature(int)), glmodel->ft, SLOT(setTypeAndStart(int)));

    // auto arrange popup codes
    QObject::connect(arrangePopup, SIGNAL(runFeature(int)), glmodel->ft, SLOT(setTypeAndStart(int)));
}


void QmlManager::sendUpdateModelInfo(int printing_time, int layer, QString xyz, float volume){
    updateModelInfo(printing_time, layer, xyz, volume);
}

void QmlManager::runArrange(){
    QFuture<void> future = QtConcurrent::run(this, &runArrange_internal);
}

void QmlManager::runArrange_internal(){
    if(!glmodels_arranged){
        glmodels_arranged = true;
        if (glmodels.size()>=2){
            vector<Mesh> meshes_to_arrange;
            vector<XYArrangement> arng_result_set;
            vector<Qt3DCore::QTransform*> m_transform_set;
            for (int i=0; i<glmodels.size(); i++){
                meshes_to_arrange.push_back(*(glmodels[i]->mesh));
                m_transform_set.push_back(glmodels[i]->m_transform);
            }
            autoarrange* ar;
            arng_result_set = ar->arngMeshes(&meshes_to_arrange);
            ar->arrangeQt3D(m_transform_set, arng_result_set);
            //ar->arrangeGlmodels(&glmodel);
        }
    }
    qDebug()<< "run arrange";
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

void QmlManager::showRotateSphere(){
    rotateSphere->setEnabled(1);
    //QFrameAction::QFrameAction qframeaction(rotateSphere);
}
void QmlManager::modelRotate(int Axis, int Angle){
    //QMatrix4x4 tmpMatrix = glmodel->m_transform->matrix();
    switch(Axis){
    case 1:{  //X
    //    float tmpx = glmodel->m_transform->rotationX();
    //    glmodel->m_transform->setRotationX(tmpx+Angle);
        break;
    }
    case 2:{  //Y
    //    float tmpy = glmodel->m_transform->rotationY();
    //    glmodel->m_transform->setRotationY(tmpy+Angle);
        break;
    }
    case 3:{  //Z
    //    float tmpz = glmodel->m_transform->rotationZ();
    //    glmodel->m_transform->setRotationZ(tmpz+Angle);
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
