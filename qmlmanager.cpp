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
    loginWindow = FindItemByName(engine, "loginWindow");
    loginButton = FindItemByName(engine, "loginButton");

    models = (QEntity *)FindItemByName(engine, "Models");
    Lights* lights = new Lights(models);

    mv = FindItemByName(engine, "MainView");
    mttab = (QEntity *)FindItemByName(engine, "mttab");
    QMetaObject::invokeMethod(mv, "initCamera");
    // model move componetns
    movePopup = FindItemByName(engine, "movePopup");
    QObject::connect(movePopup, SIGNAL(runFeature(int,int,int)),this, SLOT(modelMoveByNumber(int,int,int)));

    boundedBox = (QEntity *)FindItemByName(engine, "boundedBox");

    // model rotate components
    rotatePopup = FindItemByName(engine, "rotatePopup");
    rotateSphere = (QEntity *)FindItemByName(engine, "rotateSphereEntity");
    rotateSphereX = (QEntity *)FindItemByName(engine, "rotateSphereTorusX");
    rotateSphereY = (QEntity *)FindItemByName(engine, "rotateSphereTorusY");
    rotateSphereZ = (QEntity *)FindItemByName(engine, "rotateSphereTorusZ");
    rotateSphereobj = FindItemByName(engine, "rotateSphere");
    QObject::connect(rotateSphereobj, SIGNAL(rotateSignal(int,int)),this, SLOT(modelRotate(int,int)));
    QObject::connect(rotateSphereobj, SIGNAL(rotateDone(int)),this, SLOT(modelRotateDone(int)));
    hideRotateSphere();
    // model rotate popup codes
    QObject::connect(rotatePopup, SIGNAL(runFeature(int,int,int,int)),this, SLOT(modelRotateByNumber(int,int,int,int)));
    //rotateSphere->setEnabled(0);
    QObject *rotateButton = FindItemByName(engine, "rotateButton");

    // create hollowShellSphere and make it invisible
    hollowShellSphereEntity = new Qt3DCore::QEntity(models);
    hollowShellSphereMesh = new Qt3DExtras::QSphereMesh;
    hollowShellSphereTransform = new Qt3DCore::QTransform;
    hollowShellSphereMaterial = new Qt3DExtras::QPhongMaterial();
    hollowShellSphereEntity->addComponent(hollowShellSphereMesh);
    hollowShellSphereEntity->addComponent(hollowShellSphereTransform);
    hollowShellSphereEntity->addComponent(hollowShellSphereMaterial);
    hollowShellSphereMesh->setRadius(0);
    hollowShellSphereEntity->setProperty("visible", false);

    partList = FindItemByName(engine, "partList");

    // model layflat components
    layflatPopup = FindItemByName(engine,"layflatPopup");
    // model cut components
    cutPopup = FindItemByName(engine, "cutPopup");
    curveButton = FindItemByName(engine, "curveButton");
    flatButton = FindItemByName(engine, "flatButton");
    cutSlider = FindItemByName(engine, "cutSlider");

    // hollow shell components
    hollowShellPopup = FindItemByName(engine, "hollowShellPopup");
    hollowShellSlider = FindItemByName(engine, "cutSlider");

    // labelling components
    text3DInput = FindItemByName(engine, "text3DInput");
    labelPopup = FindItemByName(engine, "labelPopup");
    labelFontBox = FindItemByName(engine, "labelFontBox");

    // orientation components
    orientPopup = FindItemByName(engine, "orientPopup");
    progress_popup = FindItemByName(engine, "progress_popup");
    result_popup = FindItemByName(engine, "result_popup");

    // extension components
    extensionButton = FindItemByName(engine,"extendButton");
    extensionPopup = FindItemByName(engine, "extensionPopup");

    // shell offset components
    shelloffsetPopup = FindItemByName(engine, "shelloffsetPopup");

    // repair components
    repairPopup = FindItemByName(engine, "repairPopup");

    // arrange components
    arrangePopup = FindItemByName(engine, "arrangePopup");
    progress_popup = FindItemByName(engine, "progress_popup");
    //QObject::connect(arrangePopup, SIGNAL(runFeature()), this, SLOT(runArrange()));

    // save component
    saveButton = FindItemByName(engine, "saveBtn");

    // export component
    //exportButton = FindItemByName(engine, "exportBtn");
    exportOKButton = FindItemByName(engine, "exportOKBtn");

    moveArrow = (QEntity *)FindItemByName(engine, "moveArrowEntity");
    moveArrowX = (QEntity *)FindItemByName(engine, "moveArrowX");
    moveArrowY = (QEntity *)FindItemByName(engine, "moveArrowY");
    moveArrowobj = FindItemByName(engine, "moveArrow");
    QObject::connect(moveArrowobj, SIGNAL(moveSignal(int,int)),this, SLOT(modelMove(int,int)));
    QObject::connect(moveArrowobj, SIGNAL(moveDone(int)),this, SLOT(modelMoveDone(int)));
    hideMoveArrow();
    //moveArrow->setEnabled(0);
    QObject *moveButton = FindItemByName(engine, "moveButton");
    QObject::connect(mttab,SIGNAL(runGroupFeature(int,QString)),this,SLOT(runGroupFeature(int,QString)));
    QObject::connect(moveButton,SIGNAL(runGroupFeature(int,QString)),this,SLOT(runGroupFeature(int,QString)));
    QObject::connect(rotateButton,SIGNAL(runGroupFeature(int,QString)),this,SLOT(runGroupFeature(int,QString)));
    orientButton = FindItemByName(engine, "orientButton");
    QObject::connect(orientButton,SIGNAL(runGroupFeature(int,QString)),this,SLOT(runGroupFeature(int,QString)));
    repairButton = FindItemByName(engine,"repairButton");
    QObject::connect(repairButton,SIGNAL(runGroupFeature(int,QString)),this,SLOT(runGroupFeature(int,QString)));

    QObject::connect(extensionButton,SIGNAL(runGroupFeature(int,QString)),this,SLOT(runGroupFeature(int,QString)));


    layflatButton = FindItemByName(engine,"layflatButton");
    QObject::connect(layflatButton,SIGNAL(runGroupFeature(int,QString)),this,SLOT(runGroupFeature(int,QString)));

    boxUpperTab = FindItemByName(engine, "boxUpperTab");
    QObject::connect(boxUpperTab,SIGNAL(runGroupFeature(int,QString)),this,SLOT(runGroupFeature(int,QString)));

    QObject::connect(this, SIGNAL(arrangeDone(vector<QVector3D>, vector<float>)), this, SLOT(applyArrangeResult(vector<QVector3D>, vector<float>)));

    httpreq* hr = new httpreq();
    QObject::connect(loginButton, SIGNAL(loginTrial(QString)), hr, SLOT(get_iv(QString)));
    //QObject::connect(loginButton, SIGNAL(loginTrial(QString, QString)), hr, SLOT(login(QString,QString)));
    //openModelFile(QDir::currentPath()+"/Models/partial1.stl");
    //openModelFile("c:/Users/user/Desktop/asdfasf.stl");
}

void QmlManager::createModelFile(Mesh* target_mesh, QString fname) {
    openProgressPopUp();

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

    // model selection codes, connect handlers later when model selected
    QObject::connect(glmodel->shadowModel, SIGNAL(modelSelected(int)), this, SLOT(modelSelected(int)));
    qDebug() << "connected model selected signal";

    // 승환 100%
    qmlManager->setProgress(1);
}

void QmlManager::openModelFile(QString fname){
    createModelFile(nullptr, fname);

    // do auto arrange
    runArrange();
}

void QmlManager::deleteModelFile(int ID){
    //for(int i=0; i<glmodels.size();i++){
    qDebug() << "deletemodelfile" << glmodels.size();
    for (vector<GLModel*>::iterator gl_it = glmodels.begin(); gl_it != glmodels.end();){
        if((*gl_it)->ID == ID){
            disconnectHandlers((*gl_it));
            (*gl_it)->shadowModel->deleteLater();
            (*gl_it)->deleteLater();
            if (selectedModel != nullptr && selectedModel->ID == ID)
                selectedModel = nullptr;
            gl_it = glmodels.erase(gl_it);
            break;
        } else
            gl_it ++;
    }
    qDebug() << "deleteModelFile" << glmodels.size();
    QMetaObject::invokeMethod(qmlManager->boundedBox, "hideBox");
    deletePart(ID);

    // UI
    hideMoveArrow();
    hideRotateSphere();
    QMetaObject::invokeMethod(boxUpperTab, "all_off");
}

void QmlManager::disconnectHandlers(GLModel* glmodel){

    //QObject::disconnect(glmodel->ft, SIGNAL(setProgress(QVariant)),progress_popup, SLOT(updateNumber(QVariant)));
    //QObject::disconnect(glmodel->ft, SIGNAL(loadPopup(QVariant)),orientPopup, SLOT(show_popup(QVariant)));


    // need to connect for every popup
    // model rotate popup codes
    // model layflat popup codes
    QObject::disconnect(layflatPopup, SIGNAL(openLayflat()), glmodel, SLOT(openLayflat()));

    // model cut popup codes
    QObject::disconnect(cutPopup,SIGNAL(modelCut()),glmodel->shadowModel , SLOT(modelCut()));
    QObject::disconnect(cutPopup,SIGNAL(cutModeSelected(int)),glmodel->shadowModel,SLOT(cutModeSelected(int)));
    QObject::disconnect(cutPopup,SIGNAL(cutFillModeSelected(int)),glmodel->shadowModel,SLOT(cutFillModeSelected(int)));
    QObject::disconnect(cutPopup, SIGNAL(openCut()), glmodel->shadowModel, SLOT(openCut()));
    QObject::disconnect(cutPopup, SIGNAL(closeCut()), glmodel->shadowModel, SLOT(closeCut()));
    QObject::disconnect(cutPopup, SIGNAL(resultSliderValueChanged(double)), glmodel->shadowModel, SLOT(getSliderSignal(double)));

    /*// hollow shell popup codes
    QObject::disconnect(hollowShellPopup, SIGNAL(hollowShell(double)), glmodel->shadowModel, SLOT(hollowShell(double)));
    QObject::disconnect(hollowShellPopup, SIGNAL(openHollowShell()), glmodel->shadowModel, SLOT(openHollowShell()));
    QObject::disconnect(hollowShellPopup, SIGNAL(hollowShell(double)), glmodel, SLOT(indentHollowShell(double)));
    QObject::disconnect(hollowShellPopup, SIGNAL(resultSliderValueChanged(double)), glmodel->shadowModel, SLOT(getSliderSignal(double)));
    */

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
    QObject::disconnect(extensionPopup, SIGNAL(generateExtensionFaces(double)), glmodel, SLOT(generateExtensionFaces(double)));
    QObject::disconnect(glmodel->shadowModel,SIGNAL(extensionSelect()),this,SLOT(extensionSelect()));
    QObject::disconnect(glmodel->shadowModel,SIGNAL(extensionUnSelect()),this,SLOT(extensionUnSelect()));

    // shelloffset popup codes
    QObject::disconnect(shelloffsetPopup, SIGNAL(openShellOffset()), glmodel->shadowModel, SLOT(openShellOffset()));
    QObject::disconnect(shelloffsetPopup, SIGNAL(closeShellOffset()), glmodel->shadowModel, SLOT(closeShellOffset()));
    QObject::disconnect(shelloffsetPopup, SIGNAL(shellOffset(double)), glmodel, SLOT(generateShellOffset(double)));
    QObject::disconnect(shelloffsetPopup, SIGNAL(resultSliderValueChanged(double)), glmodel->shadowModel, SLOT(getSliderSignal(double)));


    // auto Repair popup codes
    QObject::disconnect(repairPopup, SIGNAL(runFeature(int)), glmodel->ft, SLOT(setTypeAndStart(int)));

    // auto arrange popup codes
    //unused, signal from qml goes right into QmlManager.runArrange
    //QObject::disconnect(arrangePopup, SIGNAL(runFeature(int)), glmodel->ft, SLOT(setTypeAndStart(int)));
    //QObject::disconnect(glmodel->arsignal, SIGNAL(runArrange()), this, SLOT(runArrange()));

    // save button codes
    QObject::disconnect(saveButton, SIGNAL(runFeature(int)), glmodel->ft, SLOT(setTypeAndRun(int)));

    // export button codes
    QObject::disconnect(exportOKButton, SIGNAL(runFeature(int, QVariant)), glmodel->ft, SLOT(setTypeAndRun(int, QVariant)));
    //QObject::disconnect(exportButton, SIGNAL(runFeature(int, QVariant)), glmodel->ft, SLOT(setTypeAndRun(int, QVariant)));
}

void QmlManager::connectHandlers(GLModel* glmodel){

    //QObject::connect(glmodel->ft, SIGNAL(setProgress(QVariant)),progress_text, SLOT(update_loading(QVariant)));
    //QObject::connect(glmodel->ft, SIGNAL(setProgress(QVariant)),progress_popup, SLOT(updateNumber(QVariant)));
    //QObject::connect(glmodel->ft, SIGNAL(loadPopup(QVariant)),orientPopup, SLOT(show_popup(QVariant)));

    // need to connect for every popup
    // model layflat popup codes
    QObject::connect(layflatPopup, SIGNAL(openLayflat()), glmodel, SLOT(openLayflat()));
    QObject::connect(glmodel, SIGNAL(resetLayflat()), this, SLOT(resetLayflat()));

    // model cut popup codes
    QObject::connect(cutPopup,SIGNAL(modelCut()),glmodel->shadowModel , SLOT(modelCut()));
    QObject::connect(cutPopup,SIGNAL(cutModeSelected(int)),glmodel->shadowModel,SLOT(cutModeSelected(int)));
    QObject::connect(cutPopup,SIGNAL(cutFillModeSelected(int)),glmodel->shadowModel,SLOT(cutFillModeSelected(int)));
    QObject::connect(cutPopup, SIGNAL(openCut()), glmodel->shadowModel, SLOT(openCut()));
    QObject::connect(cutPopup, SIGNAL(closeCut()), glmodel->shadowModel, SLOT(closeCut()));
    QObject::connect(cutPopup, SIGNAL(resultSliderValueChanged(double)), glmodel->shadowModel, SLOT(getSliderSignal(double)));

    /*// hollow shell popup codes
    QObject::connect(hollowShellPopup, SIGNAL(openHollowShell()), glmodel->shadowModel, SLOT(openHollowShell()));
    QObject::connect(hollowShellPopup, SIGNAL(closeHollowShell()), glmodel->shadowModel, SLOT(closeHollowShell()));
    QObject::connect(hollowShellPopup, SIGNAL(hollowShell(double)), glmodel, SLOT(indentHollowShell(double)));
    QObject::connect(hollowShellPopup, SIGNAL(resultSliderValueChanged(double)), glmodel->shadowModel, SLOT(getSliderSignal(double)));
    */

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
    QObject::connect(glmodel->shadowModel,SIGNAL(extensionSelect()),this,SLOT(extensionSelect()));
    QObject::connect(glmodel->shadowModel,SIGNAL(extensionUnSelect()),this,SLOT(extensionUnSelect()));

    // shelloffset popup codes
    QObject::connect(shelloffsetPopup, SIGNAL(openShellOffset()), glmodel->shadowModel, SLOT(openShellOffset()));
    QObject::connect(shelloffsetPopup, SIGNAL(closeShellOffset()), glmodel->shadowModel, SLOT(closeShellOffset()));
    QObject::connect(shelloffsetPopup, SIGNAL(shellOffset(double)), glmodel, SLOT(generateShellOffset(double)));
    QObject::connect(shelloffsetPopup, SIGNAL(resultSliderValueChanged(double)), glmodel->shadowModel, SLOT(getSliderSignal(double)));


    // auto Repair popup codes
    QObject::connect(repairPopup, SIGNAL(runFeature(int)), glmodel->ft, SLOT(setTypeAndStart(int)));

    // auto arrange popup codes
    //unused, signal from qml goes right into QmlManager.runArrange
    //QObject::connect(arrangePopup, SIGNAL(runFeature(int)), glmodel->ft, SLOT(setTypeAndStart(int)));
    //QObject::connect(glmodel->arsignal, SIGNAL(runArrange()), this, SLOT(runArrange()));

    // save button codes
    QObject::connect(saveButton, SIGNAL(runFeature(int)), glmodel->ft, SLOT(setTypeAndRun(int)));

    // export button codes
    QObject::connect(exportOKButton, SIGNAL(runFeature(int, QVariant)), glmodel->ft, SLOT(setTypeAndRun(int, QVariant)));
    //QObject::connect(exportButton, SIGNAL(runFeature(int, QVariant)), glmodel->ft, SLOT(setTypeAndRun(int, QVariant)));
}
void QmlManager::cleanSelectedModel(int type){
    //selectedModel = nullptr;
}

QVector3D QmlManager::getSelectedCenter(){
    QVector3D result = QVector3D(0,0,0);

    if(selectedModel != nullptr)
        result = selectedModel->m_transform->translation();

    return result;
}
QVector3D QmlManager::getSelectedSize(){
    QVector3D result = QVector3D(0,0,0);

    if(selectedModel != nullptr){
        result = selectedModel->m_transform->translation();
        result.setX(selectedModel->mesh->x_max - selectedModel->mesh->x_min);
        result.setY(selectedModel->mesh->y_max - selectedModel->mesh->y_min);
        result.setZ(selectedModel->mesh->z_max - selectedModel->mesh->z_min);
    }

    return result;
}

int QmlManager::getSelectedModelID(){
    int result = -1;
    result = selectedModel->ID;

    return result;
}

void QmlManager::setHandCursor(){
    QApplication::setOverrideCursor(QCursor(Qt::PointingHandCursor));
}
void QmlManager::resetCursor(){
    QApplication::restoreOverrideCursor();
}


// slicing information
void QmlManager::sendUpdateModelInfo(int printing_time, int layer, QString xyz, float volume){
    updateModelInfo(printing_time, layer, xyz, volume);
}


void QmlManager::runArrange(){
    QFuture<void> future = QtConcurrent::run(this, &runArrange_internal);
}

void QmlManager::runArrange_internal(){
    qDebug() << "run arrange glmodels size : " <<glmodels.size();
    if (glmodels.size()>=2){
        vector<Mesh> meshes_to_arrange;
        vector<XYArrangement> arng_result_set;
        vector<Qt3DCore::QTransform*> m_transform_set;
        for (int i=0; i<glmodels.size(); i++){
            meshes_to_arrange.push_back(*(glmodels[i]->mesh));
            m_transform_set.push_back(glmodels[i]->m_transform);
        }
        autoarrange* ar;
        arng_result_set = ar->arngMeshes(meshes_to_arrange);
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

void QmlManager::applyArrangeResult(vector<QVector3D> translations, vector<float> rotations){
    qDebug() << "apply arrange result ";
    for (int i=0; i<glmodels.size(); i++){
        glmodels[i]->m_transform->setTranslation(translations[i]);
        glmodels[i]->rotateModelMesh(3, rotations[i]);
    }
    qmlManager->setProgressText("Done");
    qmlManager->openResultPopUp("","Arrangement done","");
    if(selectedModel != nullptr){
        QMetaObject::invokeMethod(boundedBox, "setPosition", Q_ARG(QVariant, QVector3D(selectedModel->m_transform->translation())));
    }
}

GLModel* QmlManager::findGLModelByName(QString filename){
    for (int i=0; i<glmodels.size(); i++){
        qDebug() << "finding " << filename << glmodels.at(i)->filename;
        if (glmodels.at(i)->filename == filename){
            return glmodels.at(i);
        }
    }
    return NULL;
}

void QmlManager::modelSelected(int ID){
    QMetaObject::invokeMethod(boxUpperTab, "all_off");
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
        selectedModel->changecolor(0);
        selectedModel->checkPrintingArea();
        QMetaObject::invokeMethod(partList, "unselectPartByModel", Q_ARG(QVariant, selectedModel->ID));
        disconnectHandlers(selectedModel);  //check
        QMetaObject::invokeMethod(qmlManager->mttab, "hideTab"); // off MeshTransformer Tab

        QMetaObject::invokeMethod(boundedBox, "hideBox"); // Bounded Box
        if (groupFunctionState == "active"){
            switch (groupFunctionIndex){
            case 5:
                hideRotateSphere();
                QMetaObject::invokeMethod(rotatePopup,"offApplyFinishButton");
                break;
            case 4:
                hideMoveArrow();
                QMetaObject::invokeMethod(movePopup,"offApplyFinishButton");
                break;
            case 6:
                QMetaObject::invokeMethod(layflatPopup,"offApplyFinishButton");
                break;
            case 8:
                QMetaObject::invokeMethod(orientPopup,"offApplyFinishButton");
                break;
            case 10:
                QMetaObject::invokeMethod(repairPopup,"offApplyFinishButton");
                break;
            }
        }
    }
    if (selectedModel != target){
        // change selectedModel
        selectedModel = target;
        selectedModel->changecolor(3);
        selectedModel->changecolor(1);
        QMetaObject::invokeMethod(partList, "selectPartByModel", Q_ARG(QVariant, selectedModel->ID));
        qDebug() << "changing model" << selectedModel->ID;
        connectHandlers(selectedModel);

        // Set BoundedBox
        float xmid = (selectedModel->mesh->x_max + selectedModel->mesh->x_min)/2;
        float ymid = (selectedModel->mesh->y_max + selectedModel->mesh->y_min)/2;
        float zmid = (selectedModel->mesh->z_max + selectedModel->mesh->z_min)/2;
        //QVector3D center = (xmid, ymid, zmid);

        qDebug() << "b box center" << xmid << " " << ymid << " " << zmid ;
        QMetaObject::invokeMethod(boundedBox, "showBox");
        QMetaObject::invokeMethod(boundedBox, "setPosition", Q_ARG(QVariant, QVector3D(selectedModel->m_transform->translation())));
        QMetaObject::invokeMethod(boundedBox, "setSize", Q_ARG(QVariant, selectedModel->mesh->x_max - selectedModel->mesh->x_min),
                                                         Q_ARG(QVariant, selectedModel->mesh->y_max - selectedModel->mesh->y_min),
                                                         Q_ARG(QVariant, selectedModel->mesh->z_max - selectedModel->mesh->z_min));

        qDebug() << "scale value   " << selectedModel->mesh->x_max - selectedModel->mesh->x_min;



        if (groupFunctionState == "active"){
            switch (groupFunctionIndex){
            case 5:
                QMetaObject::invokeMethod(rotatePopup,"onApplyFinishButton");
                showRotateSphere();
                break;
            case 4:
                QMetaObject::invokeMethod(movePopup,"onApplyFinishButton");
                showMoveArrow();
                break;
            case 6:
                QMetaObject::invokeMethod(layflatPopup,"onApplyFinishButton");
                break;
            case 8:
                QMetaObject::invokeMethod(orientPopup,"onApplyFinishButton");
                break;
            case 10:
                QMetaObject::invokeMethod(repairPopup,"onApplyFinishButton");
                break;
            }
        }
    } else {
        selectedModel = nullptr;
    }
}
void QmlManager::extensionSelect(){
    QMetaObject::invokeMethod(extensionPopup,"onApplyFinishButton");
}
void QmlManager::extensionUnSelect(){
    QMetaObject::invokeMethod(extensionPopup,"offApplyFinishButton");
}
void QmlManager::selectPart(int ID){
    emit modelSelected(ID);
}
void QmlManager::unselectPart(int ID){
    GLModel* target;
    for(int i=0; i<glmodels.size();i++){
        if(glmodels.at(i)->ID == ID){
            target = glmodels.at(i);
            break;
        }
    }

    qDebug() << "resetting model" << ID;
    target->m_meshMaterial->setDiffuse(QColor(173,215,218));
    disconnectHandlers(target);
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
    selectedModel = nullptr;
    QMetaObject::invokeMethod(boundedBox, "hideBox"); // Bounded Box
}

bool QmlManager::isSelected(){
    if(selectedModel == nullptr)
        return false;
    else
        return true;
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
    if(selectedModel == nullptr)
        return;

    deleteModelFile(selectedModel->ID);
}

void QmlManager::doDeletebyID(int ID){
    deleteModelFile(ID);
}

void QmlManager::showMoveArrow(){
    if (selectedModel == nullptr)
        return;
    moveArrow->setEnabled(1);
    moveArrowX->setEnabled(1);
    moveArrowY->setEnabled(1);
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
    rotateSphereX->setEnabled(1);
    rotateSphereY->setEnabled(1);
    rotateSphereZ->setEnabled(1);
    QQmlProperty::write(rotateSphereobj,"center",selectedModel->m_transform->translation());
}
void QmlManager::mouseHack(){
    const QPointF tmp_cor(265,105);
    QMouseEvent* evt = new QMouseEvent(QEvent::MouseButtonPress,tmp_cor,Qt::LeftButton, Qt::LeftButton,0);
    //QCoreApplication::postEvent(this->parent(),evt);
    QCoreApplication::postEvent(mainWindow,evt);
    QMouseEvent* evt2 = new QMouseEvent(QEvent::MouseButtonRelease,tmp_cor,Qt::LeftButton, Qt::LeftButton,0);
    //QCoreApplication::postEvent(this->parent(),evt2);
    QCoreApplication::postEvent(mainWindow,evt2);
}
void QmlManager::modelMoveDone(int Axis){
    if (selectedModel == nullptr)
        return;
    QQmlProperty::write(moveArrowobj,"center",selectedModel->m_transform->translation());
    mouseHack();

    selectedModel->checkPrintingArea();

    if(selectedModel != nullptr)
        QMetaObject::invokeMethod(boundedBox, "setPosition", Q_ARG(QVariant, QVector3D(selectedModel->m_transform->translation())));

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
    showRotateSphere();
    mouseHack();
    rotateSnapAngle = 0;
    if(selectedModel != nullptr){
        QMetaObject::invokeMethod(boundedBox, "setPosition", Q_ARG(QVariant, QVector3D(selectedModel->m_transform->translation())));
        QMetaObject::invokeMethod(boundedBox, "setSize", Q_ARG(QVariant, selectedModel->mesh->x_max - selectedModel->mesh->x_min),
                                                         Q_ARG(QVariant, selectedModel->mesh->y_max - selectedModel->mesh->y_min),
                                                         Q_ARG(QVariant, selectedModel->mesh->z_max - selectedModel->mesh->z_min));
    }
}
void QmlManager::modelMove(int Axis, int Distance){
    if (selectedModel == nullptr)
        return;
    switch(Axis){
    case 1:{  //X
        QVector3D tmp = selectedModel->m_transform->translation();
        selectedModel->m_transform->setTranslation(QVector3D(tmp.x()+Distance,tmp.y(),tmp.z()));

        if(tmp.x() + selectedModel->mesh->x_max +1 > 100/2 )
            selectedModel->m_transform->setTranslation(QVector3D(tmp.x() - (tmp.x() + selectedModel->mesh->x_max - 100/2 + 1),tmp.y(),tmp.z()));
            //qDebug() << "Case X 1 " << tmp.x() << " " << selectedModel->mesh->x_max << ;

        if(tmp.x() + selectedModel->mesh->x_min -1 < - 100/2 )
            selectedModel->m_transform->setTranslation(QVector3D(tmp.x() - (tmp.x() + selectedModel->mesh->x_min + 100/2 - 1),tmp.y(),tmp.z()));
            //qDebug() << "Case X 2 " << tmp.x() << " " << selectedModel->mesh->x_min;

        break;
    }
    case 2:{  //Y
        QVector3D tmp = selectedModel->m_transform->translation();
        selectedModel->m_transform->setTranslation(QVector3D(tmp.x(),tmp.y()+Distance,tmp.z()));


        if(tmp.y() + selectedModel->mesh->y_max +1 > 80/2 )
            selectedModel->m_transform->setTranslation(QVector3D(tmp.x(), tmp.y() - (tmp.y() + selectedModel->mesh->y_max - 80/2 + 1), tmp.z()));
            //qDebug() << "Case Y 3 " << tmp.y() << " " << selectedModel->mesh->y_max;
        if(tmp.y() + selectedModel->mesh->y_min -1 < - 80/2 )
            selectedModel->m_transform->setTranslation(QVector3D(tmp.x(), tmp.y() - (tmp.y() + selectedModel->mesh->y_min + 80/2 - 1), tmp.z()));
            //qDebug() << "Case Y 4 " << tmp.y() << " " << selectedModel->mesh->y_min;
        break;
    }
    }
    selectedModel->checkPrintingArea();
}
void QmlManager::modelRotate(int Axis, int Angle){
    if (selectedModel == nullptr)
        return;
    rotateSnapAngle = (rotateSnapAngle + Angle +360) % 360;
    switch(Axis){
    case 1:{  //X
        float tmpx = selectedModel->m_transform->rotationX();

        if (QApplication::queryKeyboardModifiers() & Qt::ShiftModifier){// snap mode
            if(rotateSnapAngle>0 && rotateSnapAngle<90){
                selectedModel->m_transform->setRotationX(rotateSnapStartAngle + 0);
            }
            else if(rotateSnapAngle>90 && rotateSnapAngle<180){
                selectedModel->m_transform->setRotationX(rotateSnapStartAngle + 90);
            }

            else if(rotateSnapAngle>180 && rotateSnapAngle<270){
                selectedModel->m_transform->setRotationX(rotateSnapStartAngle + 180);
            }
            else if(rotateSnapAngle>270 && rotateSnapAngle<360){
                selectedModel->m_transform->setRotationX(rotateSnapStartAngle + 270);
            }
        }
        else
            selectedModel->m_transform->setRotationX(tmpx+Angle);

        break;
    }
    case 2:{  //Y
        float tmpy = selectedModel->m_transform->rotationY();

        if (QApplication::queryKeyboardModifiers() & Qt::ShiftModifier){// snap mode
            if(rotateSnapAngle>0 && rotateSnapAngle<90){
                selectedModel->m_transform->setRotationY(rotateSnapStartAngle + 0);
            }
            else if(rotateSnapAngle>90 && rotateSnapAngle<180){
                selectedModel->m_transform->setRotationY(rotateSnapStartAngle + 90);
            }
            else if(rotateSnapAngle>180 && rotateSnapAngle<270){
                selectedModel->m_transform->setRotationY(rotateSnapStartAngle + 180);
            }
            else if(rotateSnapAngle>270 && rotateSnapAngle<360){
                selectedModel->m_transform->setRotationY(rotateSnapStartAngle + 270);
            }
        }
        else
            selectedModel->m_transform->setRotationY(tmpy+Angle);
        break;
    }
    case 3:{  //Z
        float tmpz = selectedModel->m_transform->rotationZ();
        if (QApplication::queryKeyboardModifiers() & Qt::ShiftModifier){// snap mode
            if(rotateSnapAngle>0 && rotateSnapAngle<90){
                selectedModel->m_transform->setRotationZ(rotateSnapStartAngle + 0);
            }
            else if(rotateSnapAngle>90 && rotateSnapAngle<180){
                selectedModel->m_transform->setRotationZ(rotateSnapStartAngle + 90);
            }
            else if(rotateSnapAngle>180 && rotateSnapAngle<270){
                selectedModel->m_transform->setRotationZ(rotateSnapStartAngle + 180);
            }
            else if(rotateSnapAngle>270 && rotateSnapAngle<360){
                selectedModel->m_transform->setRotationZ(rotateSnapStartAngle + 270);
            }
        }
        else
            selectedModel->m_transform->setRotationZ(tmpz+Angle);
        break;
    }
    }
}
void QmlManager::modelMoveByNumber(int axis, int X, int Y){
    if (selectedModel == nullptr)
        return;
    QVector3D tmp = selectedModel->m_transform->translation();
    qDebug() << X << Y;
    qDebug() << tmp;
    selectedModel->m_transform->setTranslation(QVector3D(tmp.x()+X,tmp.y()+Y,tmp.z()));
}
void QmlManager::modelRotateByNumber(int axis,  int X, int Y, int Z){
    if (selectedModel == nullptr)
        return;
    Qt3DCore::QTransform* tmp = new Qt3DCore::QTransform();
    tmp->setRotationX(X);
    tmp->setRotationY(Y);
    tmp->setRotationZ(Z);
    selectedModel->rotateModelMesh(tmp->matrix());
    showRotateSphere();
    mouseHack();
}
void QmlManager::resetLayflat(){
    QMetaObject::invokeMethod(layflatPopup,"onApplyFinishButton");
}
void QmlManager::runGroupFeature(int ftrType, QString state){
    groupFunctionIndex = ftrType;
    groupFunctionState = state;
    switch(ftrType){
    case ftrRotate: //rotate
    {
        qDebug()<<state;
        if (state == "inactive"){
            hideRotateSphere();

        }else if(state == "active"){
            showRotateSphere();
            if (selectedModel == nullptr){
                QMetaObject::invokeMethod(rotatePopup,"offApplyFinishButton");
            }else{
                QMetaObject::invokeMethod(rotatePopup,"onApplyFinishButton");
            }

        }
        break;
    }
    case ftrMove:  //move
    {
        qDebug()<<state;
        if (state == "inactive"){
            moveArrow->setEnabled(0);
        }else if(state == "active"){
            showMoveArrow();
            if (selectedModel == nullptr){
                QMetaObject::invokeMethod(movePopup,"offApplyFinishButton");
            }else{
                QMetaObject::invokeMethod(movePopup,"onApplyFinishButton");
            }
        }
        break;
    }
    case ftrLayFlat:
    {
        if (state == "active"){
            if (selectedModel == nullptr){
                QMetaObject::invokeMethod(layflatPopup,"offApplyFinishButton");
            }else{
                QMetaObject::invokeMethod(layflatPopup,"onApplyFinishButton");
            }
        }else if (state == "inactive"){
            QApplication::restoreOverrideCursor();
        }
        break;
    }
    case ftrOrient:  //orient
    {
        qDebug()<<state;
        if (state == "active"){
            if (selectedModel == nullptr){
                QMetaObject::invokeMethod(orientPopup,"offApplyFinishButton");
            }else{
                QMetaObject::invokeMethod(orientPopup,"onApplyFinishButton");
            }
        }
        break;
    }
    case ftrRepair:  //repair
    {
        qDebug()<<state;
        if (state == "active"){
            if (selectedModel == nullptr){
                QMetaObject::invokeMethod(repairPopup,"offApplyFinishButton");
            }else{
                QMetaObject::invokeMethod(repairPopup,"onApplyFinishButton");
            }
        }
        break;
    }
    case ftrExtend:
        qDebug() << "run groupfeature extend";
        qDebug()<<state;
        if (state == "active"){
            if (selectedModel != nullptr){
                selectedModel->uncolorExtensionFaces();
                selectedModel->colorExtensionFaces();
            }
        }else if (state == "inactive"){
            if (selectedModel != nullptr){
                selectedModel->uncolorExtensionFaces();
            }
        }
        break;
    }
}
void QmlManager::addPart(QString fileName, int ID){
    QMetaObject::invokeMethod(partList, "addPart",
        Q_ARG(QVariant, fileName),
        Q_ARG(QVariant, ID));
}

void QmlManager::deletePart(int ID){
    QMetaObject::invokeMethod(partList, "deletePart", Q_ARG(QVariant, ID));
}

void QmlManager::openProgressPopUp(){
    progress = 0;
    QMetaObject::invokeMethod(progress_popup, "openPopUp");
}

void QmlManager::openResultPopUp(string inputText_h, string inputText_m, string inputText_l){
    QMetaObject::invokeMethod(result_popup, "openResultPopUp",
                              Q_ARG(QVariant, QString::fromStdString(inputText_h)),
                              Q_ARG(QVariant, QString::fromStdString(inputText_m)),
                              Q_ARG(QVariant, QString::fromStdString(inputText_l)));
}
void QmlManager::setProgress(float value){
    if (value == 0 || value >= progress){
        QMetaObject::invokeMethod(progress_popup, "updateNumber",
                                      Q_ARG(QVariant, value));
        progress = value;
    }

}

void QmlManager::setProgressText(string inputText){
    QMetaObject::invokeMethod(progress_popup, "updateText",
                              Q_ARG(QVariant, QString::fromStdString(inputText)));
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
