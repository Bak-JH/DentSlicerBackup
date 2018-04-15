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

    QObject* mv = FindItemByName(engine, "MainView");
    QMetaObject::invokeMethod(mv, "initCamera");

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
    progress_popup = FindItemByName(engine, "progress_popup");

    // extension components
    extensionPopup = FindItemByName(engine, "extensionPopup");

    // shell offset components
    shelloffsetPopup = FindItemByName(engine, "shelloffsetPopup");

    // repair components
    repairPopup = FindItemByName(engine, "repairPopup");

    // arrange components
    arrangePopup = FindItemByName(engine, "arrangePopup");

    // save component
    saveButton = FindItemByName(engine, "saveBtn");

    // export component
    //exportButton = FindItemByName(engine, "exportBtn");
    exportOKButton = FindItemByName(engine, "exportOKBtn");

    //rotation Sphere
    moveArrow = (QEntity *)FindItemByName(engine, "moveArrowEntity");
    moveArrowobj = FindItemByName(engine, "moveArrow");
    QObject::connect(moveArrowobj, SIGNAL(moveSignal(int,int)),this, SLOT(modelMove(int,int)));
    QObject::connect(moveArrowobj, SIGNAL(moveDone(int)),this, SLOT(modelMoveDone(int)));
    hideMoveArrow();
    //moveArrow->setEnabled(0);
    rotateSphere = (QEntity *)FindItemByName(engine, "rotateSphereEntity");
    rotateSphereX = (QEntity *)FindItemByName(engine, "rotateSphereTorusX");
    rotateSphereY = (QEntity *)FindItemByName(engine, "rotateSphereTorusY");
    rotateSphereZ = (QEntity *)FindItemByName(engine, "rotateSphereTorusZ");
    rotateSphereobj = FindItemByName(engine, "rotateSphere");
    QObject::connect(rotateSphereobj, SIGNAL(rotateSignal(int,int)),this, SLOT(modelRotate(int,int)));
    QObject::connect(rotateSphereobj, SIGNAL(rotateDone(int)),this, SLOT(modelRotateDone(int)));
    //rotateSphere->setEnabled(0);
    hideRotateSphere();
    QObject *rotateButton = FindItemByName(engine, "rotateButton");
    QObject *moveButton = FindItemByName(engine, "moveButton");
    QObject::connect(moveButton,SIGNAL(runGroupFeature(int,QString)),this,SLOT(runGroupFeature(int,QString)));
    QObject::connect(rotateButton,SIGNAL(runGroupFeature(int,QString)),this,SLOT(runGroupFeature(int,QString)));

    QObject *boxUpperTab = FindItemByName(engine, "boxUpperTab");
    QObject::connect(boxUpperTab,SIGNAL(runGroupFeature(int,QString)),this,SLOT(runGroupFeature(int,QString)));

    QObject::connect(this, SIGNAL(arrangeDone(vector<QVector3D>, vector<float>)), this, SLOT(applyArrangeResult(vector<QVector3D>, vector<float>)));

    //openModelFile(QDir::currentPath()+"/Models/partial1.stl");
    openModelFile("c:/Users/user/Desktop/asdfasf.stl");
}

void QmlManager::createModelFile(Mesh* target_mesh, QString fname) {
    GLModel* glmodel = new GLModel(mainWindow, models, target_mesh, fname, false);
    // 승환 GLModel constructor 안쪽

    qDebug() << "created new model file";
    glmodels.push_back(glmodel);

    // set initial position
    float xmid = (glmodel->mesh->x_max + glmodel->mesh->x_min)/2;
    float ymid = (glmodel->mesh->y_max + glmodel->mesh->y_min)/2;
    float zmid = (glmodel->mesh->z_max + glmodel->mesh->z_min)/2;

    glmodel->moveModelMesh(QVector3D(
                           (-1)*xmid,
                           (-1)*ymid,
                           (-1)*zmid));
    qDebug() << "moved model to right place";
    //QObject* progress_text = FindItemByName(engine, "progress_text"); //orientation와 공유
    // 승환 100%

    // model selection codes, connect handlers later when model selected
    QObject::connect(glmodel->shadowModel, SIGNAL(modelSelected(int)), this, SLOT(modelSelected(int)));

    qDebug() << "connected model selected signal";

    glmodels_arranged = false;

    // do auto arrange
    if (glmodels.size() >=2){
        //runArrange();
    }

    //shellOffset(glmodel, -0.5);
}

void QmlManager::openModelFile(QString fname){
    createModelFile(nullptr, fname);
}

void QmlManager::deleteModelFile(int ID){
    if (selectedModel != nullptr && selectedModel->ID == ID){
        selectedModel->shadowModel->deleteLater();
        selectedModel->deleteLater();
        selectedModel = nullptr;
    } else {
        for(int i=0; i<glmodels.size();i++){
            if(glmodels.at(i)->ID == ID){
                glmodels.at(i)->shadowModel->deleteLater();
                glmodels.at(i)->deleteLater();
                break;
            }
        }
    }
}

void QmlManager::disconnectHandlers(GLModel* glmodel){
    QObject::disconnect(glmodel->arsignal, SIGNAL(runArrange()), this, SLOT(runArrange()));

    QObject::disconnect(glmodel->ft, SIGNAL(setProgress(QVariant)),progress_popup, SLOT(updateNumber(QVariant)));
    //QObject::disconnect(glmodel->ft, SIGNAL(loadPopup(QVariant)),orientPopup, SLOT(show_popup(QVariant)));


    // need to connect for every popup

    // model cut popup codes
    QObject::disconnect(cutPopup,SIGNAL(modelCut()),glmodel->shadowModel , SLOT(modelCut()));
    QObject::disconnect(cutPopup,SIGNAL(cutModeSelected(int)),glmodel->shadowModel,SLOT(cutModeSelected(int)));
    QObject::disconnect(cutPopup, SIGNAL(openCut()), glmodel->shadowModel, SLOT(openCut()));
    QObject::disconnect(cutPopup, SIGNAL(closeCut()), glmodel->shadowModel, SLOT(closeCut()));
    QObject::disconnect(slider, SIGNAL(govalue(double)), glmodel->shadowModel, SLOT(getSliderSignal(double)));

    // auto orientation popup codes
    QObject::disconnect(orientPopup, SIGNAL(runFeature(int)), glmodel->ft, SLOT(setTypeAndStart(int)));

    // label popup codes
    QObject::disconnect(text3DInput, SIGNAL(sendTextChanged(QString, int)),glmodel->shadowModel,SLOT(getTextChanged(QString, int)));
    QObject::disconnect(labelPopup, SIGNAL(openLabelling()),glmodel->shadowModel,SLOT(openLabelling()));
    QObject::disconnect(labelPopup, SIGNAL(closeLabelling()),glmodel->shadowModel,SLOT(closeLabelling()));
    //QObject::connect(labelPopup, SIGNAL(runFeature(int)),glmodel->ft, SLOT(setTypeAndStart(int)));
    QObject::disconnect(labelPopup, SIGNAL(generateText3DMesh()), glmodel->shadowModel, SLOT(generateText3DMesh()));
    QObject::disconnect(labelFontBox, SIGNAL(sendFontName(QString)),glmodel->shadowModel, SLOT(getFontNameChanged(QString)));

    // extension popup codes
    QObject::disconnect(extensionPopup, SIGNAL(openExtension()), glmodel->shadowModel, SLOT(openExtension()));
    QObject::disconnect(extensionPopup, SIGNAL(closeExtension()), glmodel->shadowModel, SLOT(closeExtension()));
    QObject::disconnect(extensionPopup, SIGNAL(runFeature(int)), glmodel->ft, SLOT(setTypeAndStart(int)));

    // shelloffset popup codes
    QObject::disconnect(shelloffsetPopup, SIGNAL(shellOffset(float)), glmodel, SLOT(generateShellOffset(float)));


    // auto Repair popup codes
    QObject::disconnect(repairPopup, SIGNAL(runFeature(int)), glmodel->ft, SLOT(setTypeAndStart(int)));

    // auto arrange popup codes
    QObject::disconnect(arrangePopup, SIGNAL(runFeature(int)), glmodel->ft, SLOT(setTypeAndStart(int)));

    // save button codes
    QObject::disconnect(saveButton, SIGNAL(runFeature(int)), glmodel->ft, SLOT(setTypeAndRun(int)));

    // export button codes
    QObject::disconnect(exportOKButton, SIGNAL(runFeature(int, QVariant)), glmodel->ft, SLOT(setTypeAndRun(int, QVariant)));
    //QObject::disconnect(exportButton, SIGNAL(runFeature(int, QVariant)), glmodel->ft, SLOT(setTypeAndRun(int, QVariant)));
}

void QmlManager::connectHandlers(GLModel* glmodel){
    QObject::connect(glmodel->arsignal, SIGNAL(runArrange()), this, SLOT(runArrange()));

    //QObject::connect(glmodel->ft, SIGNAL(setProgress(QVariant)),progress_text, SLOT(update_loading(QVariant)));
    QObject::connect(glmodel->ft, SIGNAL(setProgress(QVariant)),progress_popup, SLOT(updateNumber(QVariant)));
    //QObject::connect(glmodel->ft, SIGNAL(loadPopup(QVariant)),orientPopup, SLOT(show_popup(QVariant)));

    // need to connect for every popup

    // model cut popup codes
    QObject::connect(cutPopup,SIGNAL(modelCut()),glmodel->shadowModel , SLOT(modelCut()));
    QObject::connect(cutPopup,SIGNAL(cutModeSelected(int)),glmodel->shadowModel,SLOT(cutModeSelected(int)));
    QObject::connect(cutPopup, SIGNAL(openCut()), glmodel->shadowModel, SLOT(openCut()));
    QObject::connect(cutPopup, SIGNAL(closeCut()), glmodel->shadowModel, SLOT(closeCut()));
    QObject::connect(slider, SIGNAL(govalue(double)), glmodel->shadowModel, SLOT(getSliderSignal(double)));

    // auto orientation popup codes
    QObject::connect(orientPopup, SIGNAL(runFeature(int)), glmodel->ft, SLOT(setTypeAndStart(int)));

    // label popup codes
    QObject::connect(text3DInput, SIGNAL(sendTextChanged(QString, int)),glmodel->shadowModel,SLOT(getTextChanged(QString, int)));
    QObject::connect(labelPopup, SIGNAL(openLabelling()),glmodel->shadowModel,SLOT(openLabelling()));
    QObject::connect(labelPopup, SIGNAL(closeLabelling()),glmodel->shadowModel,SLOT(closeLabelling()));
    //QObject::connect(labelPopup, SIGNAL(runFeature(int)),glmodel->ft, SLOT(setTypeAndStart(int)));
    QObject::connect(labelPopup, SIGNAL(generateText3DMesh()), glmodel->shadowModel, SLOT(generateText3DMesh()));
    QObject::connect(labelFontBox, SIGNAL(sendFontName(QString)),glmodel->shadowModel, SLOT(getFontNameChanged(QString)));

    // extension popup codes
    QObject::connect(extensionPopup, SIGNAL(openExtension()), glmodel->shadowModel, SLOT(openExtension()));
    QObject::connect(extensionPopup, SIGNAL(closeExtension()), glmodel->shadowModel, SLOT(closeExtension()));
    QObject::connect(extensionPopup, SIGNAL(generateExtensionFaces(double)), glmodel, SLOT(generateExtensionFaces(double)));

    // shelloffset popup codes
    QObject::connect(shelloffsetPopup, SIGNAL(shellOffset(double)), glmodel, SLOT(generateShellOffset(double)));


    // auto Repair popup codes
    QObject::connect(repairPopup, SIGNAL(runFeature(int)), glmodel->ft, SLOT(setTypeAndStart(int)));

    // auto arrange popup codes
    QObject::connect(arrangePopup, SIGNAL(runFeature(int)), glmodel->ft, SLOT(setTypeAndStart(int)));

    // save button codes
    QObject::connect(saveButton, SIGNAL(runFeature(int)), glmodel->ft, SLOT(setTypeAndRun(int)));

    // export button codes
    QObject::connect(exportOKButton, SIGNAL(runFeature(int, QVariant)), glmodel->ft, SLOT(setTypeAndRun(int, QVariant)));
    //QObject::connect(exportButton, SIGNAL(runFeature(int, QVariant)), glmodel->ft, SLOT(setTypeAndRun(int, QVariant)));

}

// slicing information
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
            vector<QVector3D> translations;
            vector<float> rotations;
            for (int i=0; i<arng_result_set.size(); i++){
                XYArrangement arng_result = arng_result_set[i];
                QVector3D trans_vec = QVector3D(arng_result.first.X/scfg->resolution, arng_result.first.Y/scfg->resolution, 0);
                translations.push_back(trans_vec);
                rotations.push_back(arng_result.second);
            }
            emit arrangeDone(translations, rotations);

            //ar->arrangeQt3D(m_transform_set, arng_result_set);
            //ar->arrangeGlmodels(&glmodel);
        }
    }
    qDebug()<< "run arrange";
}

void QmlManager::applyArrangeResult(vector<QVector3D> translations, vector<float> rotations){
    qDebug() << "apply arrange result ";
    for (int i=0; i<glmodels.size(); i++){
        glmodels[i]->m_transform->setTranslation(translations[i]);
        glmodels[i]->rotateModelMesh(3, rotations[i]);
    }
}
void QmlManager::modelSelected(int ID){
    qDebug() << "model id :" << ID ;
    GLModel* target;
    for(int i=0; i<glmodels.size();i++){
        if(glmodels.at(i)->ID == ID){
            target = glmodels.at(i);
            break;
        }
    }

    // reset previous model texture
    if (selectedModel != nullptr){
        qDebug() << "resetting model" << selectedModel->ID;
        selectedModel->m_meshMaterial->setDiffuse(QColor(173,215,218));
        disconnectHandlers(selectedModel);
        if (groupFunctionState == "active"){
            switch (groupFunctionIndex){
            case 5:
                hideRotateSphere();
                break;
            case 4:
                hideMoveArrow();
                break;
            }
        }
    }
    if (selectedModel != target){
        // change selectedModel
        selectedModel = target;
        selectedModel->m_meshMaterial->setDiffuse(QColor(100,255,100));
        qDebug() << "changing model" << selectedModel->ID;
        connectHandlers(selectedModel);
        if (groupFunctionState == "active"){
            switch (groupFunctionIndex){
            case 5:
                showRotateSphere();
                break;
            case 4:
                showMoveArrow();
                break;
            }
        }
    } else {
        selectedModel = nullptr;
    }
}

void QmlManager::modelVisible(int ID, bool isVisible){
    GLModel* target;
    for(int i=0; i<glmodels.size();i++){
        if(glmodels.at(i)->ID == ID){
            target = glmodels.at(i);
            break;
        }
    }
    target->setEnabled(isVisible);
}

void QmlManager::doDelete(){
    /*set ID for delete*/
    int ID = 1;
    GLModel* target;
    for(int i=0; i<glmodels.size();i++){
        if(glmodels.at(i)->ID == ID){
            target = glmodels.at(i);
            break;
        }
    }
    if(target != NULL){
        target->removeModelPartList();
        target->removeModel();
    }
}

void QmlManager::showMoveArrow(){
    if (selectedModel == nullptr)
        return;
    moveArrow->setEnabled(1);
    QQmlProperty::write(moveArrowobj,"center",selectedModel->m_transform->translation());
}
void QmlManager::hideMoveArrow(){
    moveArrow->setEnabled(0);
}
void QmlManager::hideRotateSphere(){
    rotateSphere->setEnabled(0);
}
void QmlManager::showRotateSphere(){
    if (selectedModel == nullptr)
        return;
    rotateSphere->setEnabled(1);
    QQmlProperty::write(rotateSphereobj,"center",selectedModel->m_transform->translation());
}
void QmlManager::modelMoveDone(int Axis){
    if (selectedModel == nullptr)
        return;
    QQmlProperty::write(moveArrowobj,"center",selectedModel->m_transform->translation());
}
void QmlManager::modelRotateDone(int Axis){
    if (selectedModel == nullptr)
        return;

    float angle;
    switch(Axis){
    case 1:{
        angle = selectedModel->m_transform->rotationX();
        selectedModel->m_transform->setRotationX(0);
        break;
    }
    case 2:{
        angle = selectedModel->m_transform->rotationY();
        selectedModel->m_transform->setRotationY(0);
        break;
    }
    case 3:{
        angle = selectedModel->m_transform->rotationZ();
        selectedModel->m_transform->setRotationZ(0);
        break;
    }
    }
    selectedModel->rotateModelMesh(Axis,-angle);
    //float zlength = (glmodel->mesh->z_max - glmodel->mesh->z_min);
    //glmodel->m_transform->setTranslation(QVector3D(0,0,zlength/2));
    hideRotateSphere();
    showRotateSphere();
}
void QmlManager::modelMove(int Axis, int Distance){
    if (selectedModel == nullptr)
        return;
    switch(Axis){
    case 1:{  //X
        QVector3D tmp = selectedModel->m_transform->translation();
        selectedModel->m_transform->setTranslation(QVector3D(tmp.x()+Distance,tmp.y(),tmp.z()));
        break;
    }
    case 2:{  //Y
        QVector3D tmp = selectedModel->m_transform->translation();
        selectedModel->m_transform->setTranslation(QVector3D(tmp.x(),tmp.y()+Distance,tmp.z()));
        break;
    }
    }
}
void QmlManager::modelRotate(int Axis, int Angle){
    if (selectedModel == nullptr)
        return;

    switch(Axis){
    case 1:{  //X
        float tmpx = selectedModel->m_transform->rotationX();
        selectedModel->m_transform->setRotationX(tmpx+Angle);
        break;
    }
    case 2:{  //Y
        float tmpy = selectedModel->m_transform->rotationY();
        selectedModel->m_transform->setRotationY(tmpy+Angle);
        break;
    }
    case 3:{  //Z
        float tmpz = selectedModel->m_transform->rotationZ();
        selectedModel->m_transform->setRotationZ(tmpz+Angle);
        break;
    }
    }
}

void QmlManager::runGroupFeature(int ftrType, QString state){
    groupFunctionIndex = ftrType;
    groupFunctionState = state;
    switch(ftrType){
    case 5: //rotate
    {
        qDebug()<<state;
        if (state == "inactive"){
            hideRotateSphere();

        }else if(state == "active"){
            showRotateSphere();
        }
        break;
    }
    case 4:  //move
    {
        qDebug()<<state;
        if (state == "inactive"){
            moveArrow->setEnabled(0);
        }else if(state == "active"){
            showMoveArrow();
        }
        break;
    }
    }

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
