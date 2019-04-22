/*
 * qmlmanager.cpp
 * : handles UI components (QML components) including Generation, Translation, Connections(sig&slot)
 *
 *
 *
 *
 *
*/

//for absolute correctness
#ifdef _DEBUG
//#define _STRICT_DEBUG

#endif

#include "qmlmanager.h"
#include "lights.h"
#include <iostream>
#include <ctype.h>
#include <QCoreApplication>
#include <QTextStream>
#include <QFileDialog>
#include <feature/generatesupport.h>
#include <feature/generateraft.h>
#include <exception>

QmlManager::QmlManager(QObject *parent) : QObject(parent)
  ,layerViewFlags(LAYER_INFILL | LAYER_SUPPORTERS | LAYER_RAFT), modelIDCounter(0)
{
}

void QmlManager::initializeUI(QQmlApplicationEngine* e){
    engine = e;
    mainWindow = FindItemByName(engine, "mainWindow");
    loginWindow = FindItemByName(engine, "loginWindow");
    loginButton = FindItemByName(engine, "loginButton");
    keyboardHandler = (Qt3DInput::QKeyboardHandler*)FindItemByName(engine, "keyboardHandler");
    models = (QEntity *)FindItemByName(engine, "Models");
    selectedModels.push_back(nullptr);
    Lights* lights = new Lights(models);

    mv = FindItemByName(engine, "MainView");
    systemTransform = (Qt3DCore::QTransform *) FindItemByName(engine, "systemTransform");
    mttab = (QEntity *)FindItemByName(engine, "mttab");
    QMetaObject::invokeMethod(mv, "initCamera");

    // model move componetns
    moveButton = FindItemByName(engine, "moveButton");
    movePopup = FindItemByName(engine, "movePopup");
    QObject::connect(movePopup, SIGNAL(runFeature(int,int,int)),this, SLOT(modelMoveByNumber(int,int,int)));
    QObject::connect(movePopup, SIGNAL(closeMove()), this, SLOT(closeMove()));
    QObject::connect(movePopup, SIGNAL(openMove()), this, SLOT(openMove()));

    boundedBox = (QEntity *)FindItemByName(engine, "boundedBox");

    // model rotate components
    rotatePopup = FindItemByName(engine, "rotatePopup");
    rotateSphere = (QEntity *)FindItemByName(engine, "rotateSphereEntity");
    rotateSphereX = (QEntity *)FindItemByName(engine, "rotateSphereTorusX");
    rotateSphereY = (QEntity *)FindItemByName(engine, "rotateSphereTorusY");
    rotateSphereZ = (QEntity *)FindItemByName(engine, "rotateSphereTorusZ");
    rotateSphereobj = FindItemByName(engine, "rotateSphere");
    QObject::connect(rotateSphereobj, SIGNAL(rotateInit()),this, SLOT(modelRotateInit()));
    QObject::connect(rotateSphereobj, SIGNAL(rotateSignal(int,int)),this, SLOT(modelRotate(int,int)));
    QObject::connect(rotateSphereobj, SIGNAL(rotateDone(int)),this, SLOT(modelRotateDone(int)));
    hideRotateSphere();
    // model rotate popup codes
    QObject::connect(rotatePopup, SIGNAL(runFeature(int,int,int,int)),this, SLOT(modelRotateByNumber(int,int,int,int)));
    QObject::connect(rotatePopup, SIGNAL(openRotate()), this, SLOT(openRotate()));
    QObject::connect(rotatePopup, SIGNAL(closeRotate()), this, SLOT(closeRotate()));
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

    undoRedoButton = FindItemByName(engine, "undoRedoButton");
    slicingData = FindItemByName(engine, "slicingData");


    // selection popup
    yesno_popup = FindItemByName(engine, "yesno_popup");
    result_popup = FindItemByName(engine, "result_popup");

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
    labelFontBoldBox = FindItemByName(engine, "labelFontBoldBox");
    labelFontSizeBox = FindItemByName(engine, "labelFontSizeBox");

    // orientation components
    orientPopup = FindItemByName(engine, "orientPopup");
    progress_popup = FindItemByName(engine, "progress_popup");
    QObject::connect(orientPopup, SIGNAL(openOrientation()), this, SLOT(openOrientation()));
    QObject::connect(orientPopup, SIGNAL(closeOrientation()), this, SLOT(closeOrientation()));

    // scale components
    scalePopup = FindItemByName(engine, "scalePopup");

    // extension components
    extensionButton = FindItemByName(engine,"extendButton");
    extensionPopup = FindItemByName(engine, "extensionPopup");

    // shell offset components
    shelloffsetPopup = FindItemByName(engine, "shelloffsetPopup");

    // manual support components
    manualSupportPopup = FindItemByName(engine, "manualSupportPopup");

    // repair components
    repairPopup = FindItemByName(engine, "repairPopup");

    // arrange components
    progress_popup = FindItemByName(engine, "progress_popup");

    // save component
    saveButton = FindItemByName(engine, "saveBtn");

    QObject::connect(saveButton, SIGNAL(runGroupFeature(int,QString, double,double,double,QVariant)) , this, SLOT(runGroupFeature(int,QString,double,double,double,QVariant)));

    // export component
    //exportButton = FindItemByName(engine, "exportBtn");
    exportOKButton = FindItemByName(engine, "exportOKBtn");

    moveArrow = (QEntity *)FindItemByName(engine, "moveArrowEntity");
    moveArrowX = (QEntity *)FindItemByName(engine, "moveArrowX");
    moveArrowY = (QEntity *)FindItemByName(engine, "moveArrowY");
    moveArrowobj = (QEntity *)FindItemByName(engine, "moveArrow");
    QObject::connect(moveArrowobj, SIGNAL(moveInit()),this, SLOT(modelMoveInit()));
    QObject::connect(moveArrowobj, SIGNAL(moveSignal(int,int)),this, SLOT(modelMove(int,int)));
    QObject::connect(moveArrowobj, SIGNAL(moveDone()),this, SLOT(modelMoveDone()));
    hideMoveArrow();
    //moveArrow->setEnabled(0);
    QObject *moveButton = FindItemByName(engine, "moveButton");
    QObject::connect(mttab,SIGNAL(runGroupFeature(int,QString, double, double, double,QVariant)),this,SLOT(runGroupFeature(int,QString, double, double, double, QVariant)));
    QObject::connect(moveButton,SIGNAL(runGroupFeature(int,QString, double, double, double,QVariant)),this,SLOT(runGroupFeature(int,QString, double, double, double, QVariant)));
    QObject::connect(rotateButton,SIGNAL(runGroupFeature(int,QString, double, double, double, QVariant)),this,SLOT(runGroupFeature(int,QString, double, double, double, QVariant)));
    orientButton = FindItemByName(engine, "orientButton");
    QObject::connect(orientButton,SIGNAL(runGroupFeature(int,QString, double, double, double, QVariant)),this,SLOT(runGroupFeature(int,QString, double, double, double, QVariant)));
    repairButton = FindItemByName(engine,"repairButton");
    QObject::connect(repairButton,SIGNAL(runGroupFeature(int,QString, double, double, double, QVariant)),this,SLOT(runGroupFeature(int,QString, double, double, double, QVariant)));

    QObject::connect(extensionButton,SIGNAL(runGroupFeature(int,QString, double, double, double, QVariant)),this,SLOT(runGroupFeature(int,QString, double, double, double, QVariant)));

    QObject::connect(scalePopup, SIGNAL(runGroupFeature(int,QString,double, double, double, QVariant)), this, SLOT(runGroupFeature(int,QString, double, double, double, QVariant)));

    layflatButton = FindItemByName(engine,"layflatButton");
    QObject::connect(layflatButton,SIGNAL(runGroupFeature(int,QString, double, double, double, QVariant)),this,SLOT(runGroupFeature(int,QString, double, double, double, QVariant)));

    boxUpperTab = FindItemByName(engine, "boxUpperTab");
    boxLeftTab = FindItemByName(engine, "boxLeftTab");
    QObject::connect(boxUpperTab,SIGNAL(runGroupFeature(int,QString, double, double, double, QVariant)),this,SLOT(runGroupFeature(int,QString, double, double, double, QVariant)));

    QObject::connect(this, SIGNAL(arrangeDone(std::vector<QVector3D>, std::vector<float>)), this, SLOT(applyArrangeResult(std::vector<QVector3D>, std::vector<float>)));

    leftTabViewMode = FindItemByName(engine, "ltvm");
    layerViewPopup = FindItemByName(engine, "layerViewPopup");
    layerViewSlider = FindItemByName(engine, "layerViewSlider");
    viewObjectButton = FindItemByName(engine, "viewObjectButton");
    QObject::connect(viewObjectButton, SIGNAL(onChanged(bool)), this, SLOT(viewObjectChanged(bool)));
    viewSupportButton = FindItemByName(engine, "viewSupportButton");
    QObject::connect(viewSupportButton, SIGNAL(onChanged(bool)), this, SLOT(viewSupportChanged(bool)));
    viewLayerButton = FindItemByName(engine, "viewLayerButton");
    QObject::connect(viewLayerButton, SIGNAL(onChanged(bool)), this, SLOT(viewLayerChanged(bool)));
    setViewMode(VIEW_MODE_OBJECT);

    layerInfillButton = FindItemByName(engine, "layerInfillButton");
    QObject::connect(layerInfillButton, SIGNAL(onChanged(bool)), this, SLOT(layerInfillButtonChanged(bool)));
    layerSupportersButton = FindItemByName(engine, "layerSupportersButton");
    QObject::connect(layerSupportersButton, SIGNAL(onChanged(bool)), this, SLOT(layerSupportersButtonChanged(bool)));
    layerRaftButton = FindItemByName(engine, "layerRaftButton");
    QObject::connect(layerRaftButton, SIGNAL(onChanged(bool)), this, SLOT(layerRaftButtonChanged(bool)));

    QObject::connect(undoRedoButton, SIGNAL(unDo()), this, SLOT(unDo()));
    QObject::connect(undoRedoButton, SIGNAL(reDo()), this, SLOT(reDo()));
    QObject::connect(mv, SIGNAL(unDo()), this, SLOT(unDo()));
    QObject::connect(mv, SIGNAL(reDo()), this, SLOT(reDo()));

    httpreq* hr = new httpreq();
    QObject::connect(loginButton, SIGNAL(loginTrial(QString)), hr, SLOT(get_iv(QString)));
    //QObject::connect(loginButton, SIGNAL(loginTrial(QString, QString)), hr, SLOT(login(QString,QString)));
    //openModelFile(QDir::currentPath()+"/Models/partial1.stl");
    //openModelFile("c:/Users/user/Desktop/asdfasf.stl");

    QObject::connect(mv, SIGNAL(copy()), this, SLOT(copyModel()));
    QObject::connect(mv, SIGNAL(paste()), this, SLOT(pasteModel()));
    QObject::connect(mv, SIGNAL(groupSelectionActivate(bool)), this, SLOT(groupSelectionActivate(bool)));

    QObject::connect(yesno_popup, SIGNAL(runGroupFeature(int, QString, double, double, double, QVariant)),this,SLOT(runGroupFeature(int,QString, double, double, double, QVariant)));
}

void QmlManager::createModelFile(Mesh* target_mesh, QString fname) {
    openProgressPopUp();
    auto res = glmodels.try_emplace(modelIDCounter, mainWindow, models, target_mesh, fname, false, modelIDCounter);
    ++modelIDCounter;
    _latest = &(res.first->second);
    qDebug() << "created new model file";
    // model selection codes, connect handlers later when model selected
    QObject::connect(_latest->shadowModel, SIGNAL(modelSelected(int)), this, SLOT(modelSelected(int)));
    qDebug() << "connected model selected signal";

    // set initial position
    float xmid = (_latest->getMesh()->x_max() + _latest->getMesh()->x_min())/2;
    float ymid = (_latest->getMesh()->y_max() + _latest->getMesh()->y_min())/2;
    float zmid = (_latest->getMesh()->z_max() + _latest->getMesh()->z_min())/2;

    _latest->moveModelMesh(QVector3D(
                           (-1)*xmid,
                           (-1)*ymid,
                           (-1)*zmid));

    //glmodel->m_translation = glmodel->m_transform->translation();
    qDebug() << "moved model to right place";

    // 승환 100%
    setProgress(1);
}
void QmlManager::openModelFile(QString fname){
    createModelFile(nullptr, fname);

    // check for defects
    checkModelFile(_latest);

    // do auto arrange
    if (glmodels.size() >= 2)
        openArrange();
    //runArrange();
    QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));
}

void QmlManager::checkModelFile(GLModel* target){
    ;

    size_t holesCount = MeshRepair::identifyHoles(target->getMesh()).size();
    if (holesCount!= 0){
        selectPart(target->ID);
        qmlManager->openYesNoPopUp(false, "Model has flaws.", "", "Do you want to fix the model?", 16, "", ftrRepair, 0);
    }
}

GLModel* QmlManager::getModelByID(int ID)
{
	auto modelItr = glmodels.find(ID);
	if (modelItr == glmodels.end())
	{
#ifdef _STRICT_DEBUG
		throw std::exception("getModelByID failed");
#endif
		qDebug() << "glmodels.find failed" <<ID;
		return nullptr;
	}
	return &modelItr->second;
}


void QmlManager::deleteOneModelFile(int ID) {

	auto target = getModelByID(ID);
	if (target)
	{
		//TODO: move these into glmodel destructor
		target->removeCuttingPoints();
		target->removeCuttingContour();
		target->shadowModel->removePlane();
		disconnectHandlers(target);
		//    target->shadowModel->deleteLater();
		//    target->deleteLater();
		glmodels.erase(ID);
		deletePart(ID);
	}

}

void QmlManager::deleteModelFileDone() {
    selectedModels.clear();
    selectedModels.push_back(nullptr);
    QMetaObject::invokeMethod(leftTabViewMode, "setEnable", Q_ARG(QVariant, false));

    QMetaObject::invokeMethod(qmlManager->boundedBox, "hideBox");

    updateModelInfo(0, 0, "0.0 X 0.0 X 0.0 mm", 0);

    // UI
    hideMoveArrow();
    hideRotateSphere();
    QMetaObject::invokeMethod(qmlManager->mttab, "hideTab");
    QMetaObject::invokeMethod(boxUpperTab, "all_off");
    QMetaObject::invokeMethod(leftTabViewMode, "setObjectView");
}

void QmlManager::deleteModelFile(int ID){
    qDebug() << "deletemodelfile" << glmodels.size();
    deleteOneModelFile(ID);
    deleteModelFileDone();
}

void QmlManager::deleteSelectedModels() {
    qDebug() << "deleteSelectedModels()";

    if (selectedModels.size() == 0 || selectedModels[0] == nullptr) {
        deleteModelFileDone();
        return;
    }

    int ID;
    for (size_t i = 0; i < selectedModels.size(); i++) {
        ID = selectedModels[i]->ID;
        deleteOneModelFile(ID);
    }

    deleteModelFileDone();

    return;
}

void QmlManager::disconnectHandlers(GLModel* glmodel){
    qDebug() << "disconnectHandlers(GLModel* glmodel)";

    //QObject::disconnect(glmodel->ft, SIGNAL(setProgress(QVariant)),progress_popup, SLOT(updateNumber(QVariant)));
    //QObject::disconnect(glmodel->ft, SIGNAL(loadPopup(QVariant)),orientPopup, SLOT(show_popup(QVariant)));

    //QObject::disconnect(yesno_popup, SIGNAL(runGroupFeature(int, QString, double, double, double, QVariant)),this,SLOT(runGroupFeature(int,QString, double, double, double, QVariant))); //glmodel->ft, SLOT(setTypeAndRun(int, QVariant)));
    //QObject::disconnect(yesno_popup, SIGNAL(runFeature(int, QVariant)), glmodel->ft, SLOT(setTypeAndRun(int, QVariant)));

    /*QObject::disconnect(undoRedoButton, SIGNAL(unDo()), glmodel, SLOT(loadUndoState()));
    QObject::disconnect(undoRedoButton, SIGNAL(reDo()), glmodel, SLOT(loadRedoState()));
    QObject::disconnect(mv, SIGNAL(unDo()), glmodel, SLOT(loadUndoState()));
    QObject::disconnect(mv, SIGNAL(reDo()), glmodel, SLOT(loadRedoState()));*/

    // need to connect for every popup
    // model rotate popup codes



    // model layflat popup codes
    QObject::disconnect(layflatPopup, SIGNAL(openLayflat()), glmodel->shadowModel, SLOT(openLayflat()));
    QObject::disconnect(layflatPopup, SIGNAL(closeLayflat()), glmodel->shadowModel, SLOT(closeLayflat()));
    QObject::disconnect(layflatPopup, SIGNAL(generateLayFlat()), glmodel, SLOT(generateLayFlat()));
    QObject::disconnect(glmodel->shadowModel,SIGNAL(layFlatSelect()),this,SLOT(layFlatSelect()));
    QObject::disconnect(glmodel->shadowModel,SIGNAL(layFlatUnSelect()),this,SLOT(layFlatUnSelect()));
    //QObject::disconnect(layflatPopup, SIGNAL(openLayflat()), glmodel, SLOT(openLayflat()));

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

    // scale popup codes
    //QObject::disconnect(scalePopup, SIGNAL(runFeature(int,double,double,double)), glmodel->ft, SLOT(setTypeAndRun(int, double, double, double)));
    QObject::disconnect(scalePopup, SIGNAL(openScale()), glmodel->shadowModel, SLOT(openScale()));
    QObject::disconnect(scalePopup, SIGNAL(closeScale()), glmodel->shadowModel, SLOT(closeScale()));

    // label popup codes
    QObject::disconnect(labelPopup, SIGNAL(sendTextChanged(QString, int)),glmodel->shadowModel,SLOT(getTextChanged(QString, int)));
    QObject::disconnect(labelPopup, SIGNAL(openLabelling()),glmodel->shadowModel,SLOT(openLabelling()));
    QObject::disconnect(labelPopup, SIGNAL(closeLabelling()),glmodel->shadowModel,SLOT(closeLabelling()));
    QObject::disconnect(labelPopup, SIGNAL(stateChangeLabelling()), glmodel, SLOT(stateChangeLabelling()));
    QObject::disconnect(labelPopup, SIGNAL(sendLabelUpdate(QString, int, QString, bool, int)), glmodel, SLOT(applyLabelInfo(QString, int, QString, bool, int)));
    //QObject::connect(labelPopup, SIGNAL(runFeature(int)),glmodel->ft, SLOT(setTypeAndStart(int)));
    QObject::disconnect(labelPopup, SIGNAL(generateText3DMesh()), glmodel->shadowModel, SLOT(generateText3DMesh()));
    QObject::disconnect(labelFontBox, SIGNAL(sendFontName(QString)),glmodel->shadowModel, SLOT(getFontNameChanged(QString)));
    QObject::disconnect(labelFontBoldBox, SIGNAL(sendFontBold(bool)),glmodel->shadowModel, SLOT(getFontBoldChanged(bool)));
    QObject::disconnect(labelFontSizeBox, SIGNAL(sendFontSize(int)),glmodel->shadowModel, SLOT(getFontSizeChanged(int)));

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

    // manual support popup codes
    QObject::disconnect(manualSupportPopup, SIGNAL(openManualSupport()), glmodel->shadowModel, SLOT(openManualSupport()));
    QObject::disconnect(manualSupportPopup, SIGNAL(closeManualSupport()), glmodel->shadowModel, SLOT(closeManualSupport()));
    QObject::disconnect(manualSupportPopup, SIGNAL(generateManualSupport()), glmodel, SLOT(generateManualSupport()));


    // auto Repair popup codes
    QObject::disconnect(repairPopup, SIGNAL(runFeature(int)), glmodel->ft, SLOT(setTypeAndStart(int)));

    // auto arrange popup codes
    //unused, signal from qml goes right into QmlManager.runArrange
    //QObject::disconnect(arrangePopup, SIGNAL(runFeature(int)), glmodel->ft, SLOT(setTypeAndStart(int)));
    //QObject::disconnect(glmodel->arsignal, SIGNAL(runArrange()), this, SLOT(runArrange()));

    // save button codes
    //QObject::disconnect(saveButton, SIGNAL(runFeature(int)), glmodel->ft, SLOT(setTypeAndRun(int)));

    // export button codes
    //QObject::disconnect(exportOKButton, SIGNAL(runGroupFeature(int, QString, double, double, double, QVariant)),this,SLOT(runGroupFeature(int,QString, double, double, double, QVariant))); //glmodel->ft, SLOT(setTypeAndRun(int, QVariant)));
    //QObject::disconnect(exportButton, SIGNAL(runFeature(int, QVariant)), glmodel->ft, SLOT(setTypeAndRun(int, QVariant)));



    QObject::disconnect(layerViewSlider, SIGNAL(sliderValueChanged(double)), glmodel, SLOT(getLayerViewSliderSignal(double)));
}

void QmlManager::connectHandlers(GLModel* glmodel){

    //QObject::connect(glmodel->ft, SIGNAL(setProgress(QVariant)),progress_text, SLOT(update_loading(QVariant)));
    //QObject::connect(glmodel->ft, SIGNAL(setProgress(QVariant)),progress_popup, SLOT(updateNumber(QVariant)));
    //QObject::connect(glmodel->ft, SIGNAL(loadPopup(QVariant)),orientPopup, SLOT(show_popup(QVariant)));

    // need to connect for every popup
    // model layflat popup codes
    /*
    QObject::connect(layflatPopup, SIGNAL(openLayflat()), glmodel, SLOT(openLayflat()));
    QObject::connect(glmodel, SIGNAL(resetLayflat()), this, SLOT(resetLayflat()));
    */

    //QObject::connect(yesno_popup, SIGNAL(runFeature(int, QVariant)), glmodel->ft, SLOT(setTypeAndRun(int, QVariant)));

    /*QObject::connect(undoRedoButton, SIGNAL(unDo()), glmodel, SLOT(loadUndoState()));
    QObject::connect(undoRedoButton, SIGNAL(reDo()), glmodel, SLOT(loadRedoState()));
    QObject::connect(mv, SIGNAL(unDo()), glmodel, SLOT(loadUndoState()));
    QObject::connect(mv, SIGNAL(reDo()), glmodel, SLOT(loadRedoState()));*/

    QObject::connect(layflatPopup, SIGNAL(openLayflat()), glmodel->shadowModel, SLOT(openLayflat()));
    QObject::connect(layflatPopup, SIGNAL(closeLayflat()), glmodel->shadowModel, SLOT(closeLayflat()));
    QObject::connect(layflatPopup, SIGNAL(generateLayFlat()), glmodel, SLOT(generateLayFlat()));
    QObject::connect(glmodel->shadowModel,SIGNAL(layFlatSelect()),this,SLOT(layFlatSelect()));
    QObject::connect(glmodel->shadowModel,SIGNAL(layFlatUnSelect()),this,SLOT(layFlatUnSelect()));

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

    // scale popup codes
    //QObject::connect(scalePopup, SIGNAL(runFeature(int,double,double,double)), glmodel->ft, SLOT(setTypeAndRun(int, double, double, double)));
    QObject::connect(scalePopup, SIGNAL(openScale()), glmodel->shadowModel, SLOT(openScale()));
    QObject::connect(scalePopup, SIGNAL(closeScale()), glmodel->shadowModel, SLOT(closeScale()));

    // label popup codes
    QObject::connect(labelPopup, SIGNAL(sendTextChanged(QString, int)),glmodel->shadowModel,SLOT(getTextChanged(QString, int)));
    QObject::connect(labelPopup, SIGNAL(openLabelling()),glmodel->shadowModel,SLOT(openLabelling()));
    QObject::connect(labelPopup, SIGNAL(closeLabelling()),glmodel->shadowModel,SLOT(closeLabelling()));
    QObject::connect(labelPopup, SIGNAL(stateChangeLabelling()), glmodel->shadowModel, SLOT(stateChangeLabelling()));
    QObject::connect(labelPopup, SIGNAL(sendLabelUpdate(QString, int, QString, bool, int)), glmodel->shadowModel, SLOT(applyLabelInfo(QString, int, QString, bool, int)));
    //QObject::connect(labelPopup, SIGNAL(runFeature(int)),glmodel->ft, SLOT(setTypeAndStart(int)));
    QObject::connect(labelPopup, SIGNAL(generateText3DMesh()), glmodel->shadowModel, SLOT(generateText3DMesh()));
    QObject::connect(labelFontBox, SIGNAL(sendFontName(QString)),glmodel->shadowModel, SLOT(getFontNameChanged(QString)));
    QObject::connect(labelFontBoldBox, SIGNAL(sendFontBold(bool)),glmodel->shadowModel, SLOT(getFontBoldChanged(bool)));
    QObject::connect(labelFontSizeBox, SIGNAL(sendFontSize(int)),glmodel->shadowModel, SLOT(getFontSizeChanged(int)));

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

    // manual support popup codes
    QObject::connect(manualSupportPopup, SIGNAL(openManualSupport()), glmodel->shadowModel, SLOT(openManualSupport()));
    QObject::connect(manualSupportPopup, SIGNAL(closeManualSupport()), glmodel->shadowModel, SLOT(closeManualSupport()));
    QObject::connect(manualSupportPopup, SIGNAL(generateManualSupport()), glmodel, SLOT(generateManualSupport()));

    // auto Repair popup codes
    QObject::connect(repairPopup, SIGNAL(runFeature(int)), glmodel->ft, SLOT(setTypeAndStart(int)));

    // auto arrange popup codes
    //unused, signal from qml goes right into QmlManager.runArrange
    //QObject::connect(arrangePopup, SIGNAL(runFeature(int)), glmodel->ft, SLOT(setTypeAndStart(int)));
    //QObject::connect(glmodel->arsignal, SIGNAL(runArrange()), this, SLOT(runArrange()));

    // save button codes
    //QObject::connect(saveButton, SIGNAL(runFeature(int)), glmodel->ft, SLOT(setTypeAndRun(int)));

    // export button codes
    //QObject::connect(exportOKButton, SIGNAL(runGroupFeature(int, QString, double, double, double, QVariant)),this,SLOT(runGroupFeature(int,QString, double, double, double, QVariant)));
    //QObject::connect(exportButton, SIGNAL(runFeature(int, QVariant)), glmodel->ft, SLOT(setTypeAndRun(int, QVariant)));


    QObject::connect(layerViewSlider, SIGNAL(sliderValueChanged(double)), glmodel, SLOT(getLayerViewSliderSignal(double)));
}
void QmlManager::cleanselectedModel(int type){
    //selectedModels[0] = nullptr;
}

QVector3D QmlManager::getSelectedCenter(){
    QVector3D result = QVector3D(0,0,0);

    if(selectedModels[0] != nullptr){
        // set initial position
        float xmid = (selectedModels[0]->getMesh()->x_max() + selectedModels[0]->getMesh()->x_min())/2;
        float ymid = (selectedModels[0]->getMesh()->y_max() + selectedModels[0]->getMesh()->y_min())/2;
        float zmid = (selectedModels[0]->getMesh()->z_max() + selectedModels[0]->getMesh()->z_min())/2;
        result = selectedModels[0]->m_transform->translation()+QVector3D(xmid,ymid,zmid);//QVector3D(selectedModels[0]->m_transform->translation().x(), selectedModels[0]->m_transform->translation().y(), -selectedModels[0]->getMesh()->z_min());
    }
    return result;
}
QVector3D QmlManager::getSelectedSize(){
    QVector3D result = QVector3D(0,0,0);

    if(selectedModels[0] != nullptr){
        result = selectedModels[0]->m_transform->translation();
        result.setX(selectedModels[0]->getMesh()->x_max() - selectedModels[0]->getMesh()->x_min());
        result.setY(selectedModels[0]->getMesh()->y_max() - selectedModels[0]->getMesh()->y_min());
        result.setZ(selectedModels[0]->getMesh()->z_max() - selectedModels[0]->getMesh()->z_min());
    }

    return result;
}

int QmlManager::getselectedModelID(){
    int result = -1;
    if (selectedModels[0] != nullptr)
        result = selectedModels[0]->ID;

    return result;
}

int QmlManager::getSelectedModelsSize() {
    if (selectedModels.size() == 0 || selectedModels[0] == nullptr) return 0;
    return selectedModels.size();
}

float QmlManager::getBedXSize(){
    return scfg->bed_x;
}

float QmlManager::getBedYSize(){
    return scfg->bed_y;
}

void QmlManager::setBedXSize(float x){
    scfg->bed_x = x;
}

void QmlManager::setBedYSize(float y){
    scfg->bed_y = y;
}


bool QmlManager::getGroupSelectionActive() {
    return groupSelectionActive;
}

QString QmlManager::getVersion(){
    return version;
}
void QmlManager::keyboardHandlerFocus(){
    qDebug() << "keyboard focus on";
    keyboardHandler->setFocus(true);
    QMetaObject::invokeMethod(mv, "forceFocus");
    qDebug() << "keyboard handler focus : " << keyboardHandler->focus();
}

void QmlManager::fixMesh(){
    if (selectedModels[0] == nullptr)
        return;

    openProgressPopUp();
    selectedModels[0]->repairMesh();

}

void QmlManager::disableObjectPickers(){
    for (auto& pair : glmodels){
		auto glm = &pair.second;
        if (glm->shadowModel->m_objectPicker->isEnabled())
            glm->shadowModel->m_objectPicker->setEnabled(false);
    }
}

void QmlManager::enableObjectPickers(){
	for (auto& pair : glmodels) {
		auto glm = &pair.second;
        if (!glm->shadowModel->m_objectPicker->isEnabled())
            glm->shadowModel->m_objectPicker->setEnabled(true);
    }
}

void QmlManager::setHandCursor(){
    QApplication::setOverrideCursor(QCursor(Qt::PointingHandCursor));
}
void QmlManager::setClosedHandCursor(){
    QApplication::setOverrideCursor(QCursor(Qt::ClosedHandCursor));
}
void QmlManager::resetCursor(){
    QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));
}

float QmlManager::selected_x_max(size_t selectedNum) {
    float x_max = selectedModels[0]->m_transform->translation().x() + selectedModels[0]->getMesh()->x_max();
    for (size_t i = 1; i < selectedNum; i++) {
        if (x_max < selectedModels[i]->m_transform->translation().x() + selectedModels[i]->getMesh()->x_max())
            x_max = selectedModels[i]->m_transform->translation().x() + selectedModels[i]->getMesh()->x_max();
    }
    return x_max;
}

float QmlManager::selected_x_min(size_t selectedNum) {
    float x_min = selectedModels[0]->m_transform->translation().x() + selectedModels[0]->getMesh()->x_min();
    for (size_t i = 1; i < selectedNum; i++) {
        if (x_min > selectedModels[i]->m_transform->translation().x() + selectedModels[i]->getMesh()->x_min())
            x_min = selectedModels[i]->m_transform->translation().x() + selectedModels[i]->getMesh()->x_min();
    }
    return x_min;
}

float QmlManager::selected_y_max(size_t selectedNum) {
    float y_max = selectedModels[0]->m_transform->translation().y() + selectedModels[0]->getMesh()->y_max();
    for (size_t i = 1; i < selectedNum; i++) {
        if (y_max < selectedModels[i]->m_transform->translation().y() + selectedModels[i]->getMesh()->y_max())
            y_max = selectedModels[i]->m_transform->translation().y() + selectedModels[i]->getMesh()->y_max();
    }
    return y_max;
}

float QmlManager::selected_y_min(size_t selectedNum) {
    float y_min = selectedModels[0]->m_transform->translation().y() + selectedModels[0]->getMesh()->y_min();
    for (size_t i = 1; i < selectedNum; i++) {
        if (y_min > selectedModels[i]->m_transform->translation().y() + selectedModels[i]->getMesh()->y_min())
            y_min = selectedModels[i]->m_transform->translation().y() + selectedModels[i]->getMesh()->y_min();
    }
    return y_min;
}

float QmlManager::selected_z_max(size_t selectedNum) {
    float z_max = selectedModels[0]->m_transform->translation().z() + selectedModels[0]->getMesh()->z_max();
    for (size_t i = 1; i < selectedNum; i++) {
        if (z_max < selectedModels[i]->m_transform->translation().z() + selectedModels[i]->getMesh()->z_max())
            z_max = selectedModels[i]->m_transform->translation().z() + selectedModels[i]->getMesh()->z_max();
    }
    return z_max;
}

float QmlManager::selected_z_min(size_t selectedNum) {
    float z_min = selectedModels[0]->m_transform->translation().z() + selectedModels[0]->getMesh()->z_min();
    for (size_t i = 1; i < selectedNum; i++) {
        if (z_min > selectedModels[i]->m_transform->translation().z() + selectedModels[i]->getMesh()->z_min())
            z_min = selectedModels[i]->m_transform->translation().z() + selectedModels[i]->getMesh()->z_min();
    }
    return z_min;
}

void QmlManager::updateBoundedBox(){
    size_t selectedNum = selectedModels.size();
    float x_max = selected_x_max(selectedNum);
    float x_min = selected_x_min(selectedNum);
    float y_max = selected_y_max(selectedNum);
    float y_min = selected_y_min(selectedNum);
    float z_max = selected_z_max(selectedNum);
    float z_min = selected_z_min(selectedNum);
    QMetaObject::invokeMethod(boundedBox, "setPosition", Q_ARG(QVariant, QVector3D((x_max + x_min)/2, (y_max + y_min)/2, (z_max + z_min)/2)));
    QMetaObject::invokeMethod(boundedBox, "setSize", Q_ARG(QVariant,  x_max - x_min), Q_ARG(QVariant,  y_max - y_min), Q_ARG(QVariant,  z_max - z_min));
}

void QmlManager::sendUpdateModelInfo(){
    qDebug() << "send update model info";
    if (selectedModels.size() == 0 || selectedModels[0] == nullptr || this->viewMode == VIEW_MODE_LAYER || this->viewMode == VIEW_MODE_SUPPORT){
        qDebug() << "sendUpdateModelInfo() - no selected model";

        slicingData->setProperty("visible", false);
        QMetaObject::invokeMethod(boundedBox, "hideBox");

        return;
    }

    qDebug() << "sendUpdateModelInfo() - selected model exists";

    slicingData->setProperty("visible", true);

    size_t selectedNum = selectedModels.size();
    float x_max = selected_x_max(selectedNum);
    float x_min = selected_x_min(selectedNum);
    float y_max = selected_y_max(selectedNum);
    float y_min = selected_y_min(selectedNum);
    float z_max = selected_z_max(selectedNum);
    float z_min = selected_z_min(selectedNum);

    QString size;
    size.sprintf("%.1f X %.1f X %.1f mm", x_max - x_min, y_max - y_min, z_max - z_min);

    QMetaObject::invokeMethod(boundedBox, "showBox");
    QMetaObject::invokeMethod(boundedBox, "setPosition", Q_ARG(QVariant, QVector3D((x_max + x_min)/2, (y_max + y_min)/2, (z_max + z_min)/2)));
    QMetaObject::invokeMethod(boundedBox, "setSize", Q_ARG(QVariant, x_max - x_min), Q_ARG(QVariant, y_max - y_min), Q_ARG(QVariant, z_max - z_min));
    updateModelInfo(-1,-1,size,-1);
    QMetaObject::invokeMethod(scalePopup, "updateSizeInfo", Q_ARG(QVariant, x_max - x_min), Q_ARG(QVariant, y_max - y_min), Q_ARG(QVariant, z_max - z_min));
}

// slicing information
void QmlManager::sendUpdateModelInfo(int printing_time, int layer, QString xyz, float volume){
    qDebug() << "sent update model Info";
    updateModelInfo(printing_time, layer, xyz, volume);
}

void QmlManager::openArrange(){
    //arrangePopup->setProperty("visible", true);
    openYesNoPopUp(false, "Click OK to auto-arrange models.", "", "", 18, "qrc:/Resource/popup_image/image_arrange.png", ftrArrange, 1);
}

void QmlManager::runArrange(){
    QFuture<void> future = QtConcurrent::run(this, &QmlManager::runArrange_internal);
}

void QmlManager::runArrange_internal(){
    qDebug() << "run arrange glmodels size : " <<glmodels.size();
    if (glmodels.size()>=2){
        std::vector<Mesh> meshes_to_arrange;
        std::vector<XYArrangement> arng_result_set;
        std::vector<Qt3DCore::QTransform*> m_transform_set;
        for(auto& pair : glmodels)
        {
            auto model = &pair.second;
            meshes_to_arrange.push_back(*(model->getMesh()));
            m_transform_set.push_back(model->m_transform);
        }
        autoarrange* ar;
        arng_result_set = ar->arngMeshes(meshes_to_arrange);
        std::vector<QVector3D> translations;
        std::vector<float> rotations;
        for (size_t i=0; i<arng_result_set.size(); i++){
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

void QmlManager::applyArrangeResult(std::vector<QVector3D> translations, std::vector<float> rotations){
    qDebug() << "apply arrange result ";
    size_t index = 0;
    for(auto& pair : glmodels)
    {
        auto model = &pair.second;
        model->m_transform->setTranslation(translations[index]);
        model->rotateModelMesh(3, rotations[index]);
        ++index;
    }

    qmlManager->setProgressText("Done");
    qmlManager->openResultPopUp("","Arrangement done","");
    if(selectedModels[0] != nullptr){
        //QMetaObject::invokeMethod(boundedBox, "setPosition", Q_ARG(QVariant, selectedModels[0]->m_transform->translation()+QVector3D((selectedModels[0]->getMesh()->x_max()+selectedModels[0]->getMesh()->x_min())/2,(selectedModels[0]->getMesh()->y_max()+selectedModels[0]->getMesh()->y_min())/2,(selectedModels[0]->getMesh()->z_max()+selectedModels[0]->getMesh()->z_min())/2)));
        //QMetaObject::invokeMethod(boundedBox, "setSize", Q_ARG(QVariant, selectedModels[0]->getMesh()->x_max() - selectedModels[0]->getMesh()->x_min()),
        //                                                 Q_ARG(QVariant, selectedModels[0]->getMesh()->y_max() - selectedModels[0]->getMesh()->y_min()),
        //                                                 Q_ARG(QVariant, selectedModels[0]->getMesh()->z_max() - selectedModels[0]->getMesh()->z_min()));
        sendUpdateModelInfo();
    }
}

GLModel* QmlManager::findGLModelByName(QString filename){

    for(auto& pair : glmodels)
    {
        auto model = &pair.second;
        qDebug() << "finding " << filename << model->filename;
        if (model->filename == filename){
            return model;
        }
    }
    return NULL;
}

void QmlManager::backgroundClicked(){
    qDebug() << "background clicked";
    unselectAll();
}

bool QmlManager::multipleModelSelected(int ID){
    //modelClicked = true;
    QMetaObject::invokeMethod(boxUpperTab, "all_off");
	auto target = getModelByID(ID);
	if (!target)
	{
		return false;
	}


    /*
    qDebug() << "multipleModelSelected():" << target << target->shadowModel;
    qDebug() << "-- printing selectedModels --" <<selectedModels.size();
    int i = 0;
    for (std::vector<GLModel*>::iterator it_ = selectedModels.begin(); it_ != selectedModels.end() ; ++it_){
        qDebug() << "model" << i << ":" <<*it_ ;
        i++;
    }
    qDebug() <<"------------------------";
    */

    // if target is already in selectedModels
    for (auto it=selectedModels.begin(); it!= selectedModels.end(); ++it){
        /* when selectedModels is an empty std::vector */
        if ((*it) == nullptr) {
            if (selectedModels.size() == 1)
                return false;
        }
        else if ((*it)->ID == ID){
            // do unselect model

            it = selectedModels.erase(it);
            target->changecolor(0);
            target->checkPrintingArea();
            QMetaObject::invokeMethod(partList, "unselectPartByModel", Q_ARG(QVariant, target->ID));
            QMetaObject::invokeMethod(yesno_popup, "deletePart", Q_ARG(QVariant, target->ID));

            // set slicing info box property visible true if slicing info exists
            //slicingData->setProperty("visible", false);
            sendUpdateModelInfo();

            disconnectHandlers(target);  //check
            QMetaObject::invokeMethod(qmlManager->mttab, "hideTab"); // off MeshTransformer Tab

            if (groupFunctionState == "active"){
                switch (groupFunctionIndex){
                //case 2:
                //    QMetaObject::invokeMethod(savePopup, "offApplyFinishButton");
                //    break;
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
            return true;
        }

    }

    selectedModels.push_back(target);
    connectHandlers(target);
    target->changecolor(3);
    target->changecolor(1);
    qDebug() << "multipleModelSelected invoke";
    QMetaObject::invokeMethod(partList, "selectPartByModel", Q_ARG(QVariant, target->ID));
    QMetaObject::invokeMethod(yesno_popup, "addPart", Q_ARG(QVariant, target->getFileName(target->filename.toStdString().c_str())), Q_ARG(QVariant, target->ID));
    // Set BoundedBox
//    float xmid = (target->getMesh()->x_max() + target->getMesh()->x_min())/2;
//    float ymid = (target->getMesh()->y_max() + target->getMesh()->y_min())/2;
//    float zmid = (target->getMesh()->z_max() + target->getMesh()->z_min())/2;
    //QVector3D center = (xmid, ymid, zmid);

    qDebug() << "[multi model selected] b box center"; //<< xmid << " " << ymid << " " << zmid ;
//    QMetaObject::invokeMethod(boundedBox, "showBox");
//        QMetaObject::invokeMethod(boundedBox, "setPosition", Q_ARG(QVariant, QVector3D(selectedModels[0]->m_transform->translation())));
//        QMetaObject::invokeMethod(boundedBox, "setSize", Q_ARG(QVariant, selectedModels[0]->getMesh()->x_max() - selectedModels[0]->getMesh()->x_min()),
//                                                         Q_ARG(QVariant, selectedModels[0]->getMesh()->y_max() - selectedModels[0]->getMesh()->y_min()),
//                                                         Q_ARG(QVariant, selectedModels[0]->getMesh()->z_max() - selectedModels[0]->getMesh()->z_min()));
//    QMetaObject::invokeMethod(boundedBox, "setPosition", Q_ARG(QVariant, target->m_transform->translation()+QVector3D((target->getMesh()->x_max()+target->getMesh()->x_min())/2,(target->getMesh()->y_max()+target->getMesh()->y_min())/2,(target->getMesh()->z_max()+target->getMesh()->z_min())/2)));
/*
    QMetaObject::invokeMethod(boundedBox, "setSize", Q_ARG(QVariant, target->getMesh()->x_max() - target->getMesh()->x_min()),
                                                     Q_ARG(QVariant, target->getMesh()->y_max() - target->getMesh()->y_min()),
                                                     Q_ARG(QVariant, target->getMesh()->z_max() - target->getMesh()->z_min()));
*/
    sendUpdateModelInfo();
    if (groupFunctionState == "active"){
        switch (groupFunctionIndex){
        //case 2:
        //    QMetaObject::invokeMethod(savePopup, "onApplyFinishButton");
        //    break;
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

    if (selectedModels.size() >= 2)
        groupFunctionState = "active";
    return true;
}

void QmlManager::lastModelSelected(){
    //qDebug() << "selectLastModel() ++++++++++++++++++++++++++++++++" << selectedModels.size();

    /* leaves only last model clicked */
    int size = selectedModels.size();

    /* if selected Models has only one element or none */
    if (size < 2)
        return;

    GLModel *last = selectedModels[size - 1];
    selectedModels.pop_back();

    qDebug() << "leave:" << last << last->shadowModel;

    /* remove all elements from the list */
    for (auto it = selectedModels.begin() ; it != selectedModels.end() ; ++it) {
        if (*it == nullptr) {
            // just in case
            break;
        }
        /* it is simillar to selectModel() */
        (*it)->changecolor(0);
        (*it)->checkPrintingArea();
        QMetaObject::invokeMethod(partList, "unselectPartByModel", Q_ARG(QVariant, (*it)->ID));
        QMetaObject::invokeMethod(yesno_popup, "deletePart", Q_ARG(QVariant, (*it)->ID));

        slicingData->setProperty("visible", false);

        disconnectHandlers((*it));
        QMetaObject::invokeMethod(qmlManager->mttab, "hideTab");
        QMetaObject::invokeMethod(boundedBox, "hideBox");

        if(groupFunctionState == "active") {
            switch (groupFunctionIndex) {
            //case 2:
            //    QMetaObject::invokeMethod(savePopup, "offApplyFinishButton");
            //    break;
            case 4:
                hideMoveArrow();
                QMetaObject::invokeMethod(movePopup, "offApplyFinishButton");
                break;
            case 5:
                hideRotateSphere();
                QMetaObject::invokeMethod(rotatePopup, "offApplyFinishButton");
                break;
            case 6:
                QMetaObject::invokeMethod(layflatPopup, "offApplyFinishButton");
                break;
            case 8:
                QMetaObject::invokeMethod(orientPopup, "offApplyFinishButton");
                break;
            case 10:
                QMetaObject::invokeMethod(repairPopup, "offApplyFinishButton");
                break;
            }
        }
    }
    selectedModels.clear();
    selectedModels.push_back(last);
    sendUpdateModelInfo();
}

void QmlManager::modelSelected(int ID){
    qDebug() << "modelSelected()";
    if (groupSelectionActive){
        if (multipleModelSelected(ID))
            return;
    }

    QMetaObject::invokeMethod(boxUpperTab, "all_off");
    QMetaObject::invokeMethod(leftTabViewMode, "setObjectView");
	auto target = getModelByID(ID);
	if(target)
    if (selectedModels.size() >= 2){ // remove selected models if multiple selected previously
        for (auto it=selectedModels.begin(); it!=selectedModels.end(); ++it){
            // unselect Model
            if (*it == nullptr) {
                break;
            }
            (*it)->changecolor(0);
            (*it)->checkPrintingArea();
            QMetaObject::invokeMethod(partList, "unselectPartByModel", Q_ARG(QVariant, (*it)->ID));
            QMetaObject::invokeMethod(yesno_popup, "deletePart", Q_ARG(QVariant, (*it)->ID));

            // set slicing info box property visible true if slicing info exists
            slicingData->setProperty("visible",false);

            disconnectHandlers((*it));  //check
            QMetaObject::invokeMethod(qmlManager->mttab, "hideTab"); // off MeshTransformer Tab

            QMetaObject::invokeMethod(boundedBox, "hideBox"); // Bounded Box
            if (groupFunctionState == "active"){
                switch (groupFunctionIndex){
                //case 2:
                //    QMetaObject::invokeMethod(savePopup, "offApplyFinishButton");
                //    break;
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
        selectedModels.clear();
        selectedModels.push_back(nullptr);
    } else if (selectedModels.size() == 1 && selectedModels[0] != nullptr){
        //qDebug() << "resetting model" << selectedModels[0]->ID;
        selectedModels[0]->changecolor(0);
        selectedModels[0]->checkPrintingArea();
        QMetaObject::invokeMethod(partList, "unselectPartByModel", Q_ARG(QVariant, selectedModels[0]->ID));
        QMetaObject::invokeMethod(yesno_popup, "deletePart", Q_ARG(QVariant, selectedModels[0]->ID));

        // set slicing info box property visible true if slicing info exists
        slicingData->setProperty("visible",false);

        disconnectHandlers(selectedModels[0]);  //check
        QMetaObject::invokeMethod(qmlManager->mttab, "hideTab"); // off MeshTransformer Tab

        QMetaObject::invokeMethod(boundedBox, "hideBox"); // Bounded Box
        if (groupFunctionState == "active"){
            switch (groupFunctionIndex){
            //case 2:
            //    QMetaObject::invokeMethod(savePopup, "offApplyFinishButton");
            //    break;
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
        selectedModels[0]->changeViewMode(VIEW_MODE_OBJECT);
    }
    if (selectedModels[0] != target){
        // change selectedModels[0]
        if (selectedModels[0] != nullptr)
            disconnectHandlers(selectedModels[0]);
        selectedModels[0] = target;

        //remove dupllicate hanlders
        //disconnectHandlers(selectedModels[0]);
        connectHandlers(selectedModels[0]);

        selectedModels[0]->changecolor(3);
        selectedModels[0]->changecolor(1);
        qDebug() << "modelSelected invoke";
        QMetaObject::invokeMethod(partList, "selectPartByModel", Q_ARG(QVariant, selectedModels[0]->ID));
        QMetaObject::invokeMethod(yesno_popup, "addPart", Q_ARG(QVariant, selectedModels[0]->getFileName(selectedModels[0]->filename.toStdString().c_str())), Q_ARG(QVariant, ID));
        qDebug() << "changing model" << selectedModels[0]->ID;

        // Set BoundedBox
//        float xmid = (selectedModels[0]->getMesh()->x_max() + selectedModels[0]->getMesh()->x_min())/2;
//        float ymid = (selectedModels[0]->getMesh()->y_max() + selectedModels[0]->getMesh()->y_min())/2;
//        float zmid = (selectedModels[0]->getMesh()->z_max() + selectedModels[0]->getMesh()->z_min())/2;
        //QVector3D center = (xmid, ymid, zmid);

        qDebug() << "[model selected] b box center"; //<< xmid << " " << ymid << " " << zmid ;
//        QMetaObject::invokeMethod(boundedBox, "showBox");
//        QMetaObject::invokeMethod(boundedBox, "setPosition", Q_ARG(QVariant, QVector3D(selectedModels[0]->m_transform->translation())));
//        QMetaObject::invokeMethod(boundedBox, "setSize", Q_ARG(QVariant, selectedModels[0]->getMesh()->x_max() - selectedModels[0]->getMesh()->x_min()),
//                                                         Q_ARG(QVariant, selectedModels[0]->getMesh()->y_max() - selectedModels[0]->getMesh()->y_min()),
//                                                         Q_ARG(QVariant, selectedModels[0]->getMesh()->z_max() - selectedModels[0]->getMesh()->z_min()));
/*        QMetaObject::invokeMethod(boundedBox, "setPosition", Q_ARG(QVariant, selectedModels[0]->m_transform->translation()+QVector3D((selectedModels[0]->getMesh()->x_max()+selectedModels[0]->getMesh()->x_min())/2,(selectedModels[0]->getMesh()->y_max()+selectedModels[0]->getMesh()->y_min())/2,(selectedModels[0]->getMesh()->z_max()+selectedModels[0]->getMesh()->z_min())/2)));
        QMetaObject::invokeMethod(boundedBox, "setSize", Q_ARG(QVariant, selectedModels[0]->getMesh()->x_max() - selectedModels[0]->getMesh()->x_min()),
                                                         Q_ARG(QVariant, selectedModels[0]->getMesh()->y_max() - selectedModels[0]->getMesh()->y_min()),
                                                         Q_ARG(QVariant, selectedModels[0]->getMesh()->z_max() - selectedModels[0]->getMesh()->z_min()));
*/
        QMetaObject::invokeMethod(layerViewSlider, "setThickness", Q_ARG(QVariant, (scfg->layer_height)));
        QMetaObject::invokeMethod(layerViewSlider, "setHeight", Q_ARG(QVariant, (selectedModels[0]->getMesh()->z_max() - selectedModels[0]->getMesh()->z_min() + scfg->raft_thickness)));

        // set slicing info box property visible true if slicing info exists
//        slicingData->setProperty("visible", true);
        sendUpdateModelInfo();
        /*if (selectedModels[0]->slicingInfo != NULL){
            slicingData->setProperty("visible", true);
        } else {
            slicingData->setProperty("visible", false);
        }*/

        qDebug() << "scale value   " << selectedModels[0]->getMesh()->x_max() - selectedModels[0]->getMesh()->x_min();



        if (groupFunctionState == "active"){
            switch (groupFunctionIndex){
            //case 2:
            //    QMetaObject::invokeMethod(savePopup, "onApplyFinishButton");
            //    break;
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
        //selectedModels[0] = nullptr;
        selectedModels.clear();
        selectedModels.push_back(nullptr);
    }

    QMetaObject::invokeMethod(leftTabViewMode, "setEnable", Q_ARG(QVariant, selectedModels[0] != nullptr));

    sendUpdateModelInfo();
}

void QmlManager::layFlatSelect(){
    QMetaObject::invokeMethod(layflatPopup,"onApplyFinishButton");
}
void QmlManager::layFlatUnSelect(){
    QMetaObject::invokeMethod(layflatPopup,"offApplyFinishButton");
}

void QmlManager::extensionSelect(){
    QMetaObject::invokeMethod(extensionPopup,"onApplyFinishButton");
}
void QmlManager::extensionUnSelect(){
    QMetaObject::invokeMethod(extensionPopup,"offApplyFinishButton");
}

void QmlManager::manualSupportSelect(){
    QMetaObject::invokeMethod(manualSupportPopup,"onApplyFinishButton");
}
void QmlManager::manualSupportUnselect(){
    QMetaObject::invokeMethod(manualSupportPopup,"offApplyFinishButton");
}

void QmlManager::selectPart(int ID){
    emit modelSelected(ID);
}
void QmlManager::unselectPart(int ID){
	auto target = getModelByID(ID);
	if (target)
	{
		qDebug() << "resetting model" << ID;
		unselectPartImpl(target);
	}

}

void QmlManager::unselectAll(){
    setViewMode(VIEW_MODE_OBJECT);
    for(GLModel* curModel : selectedModels){
        if (curModel != nullptr){
            unselectPart(curModel->ID);
            //QMetaObject::invokeMethod(partList, "unselectPartByModel", Q_ARG(QVariant, curModel->ID));
        }
    }
    hideMoveArrow();
    hideRotateSphere();
    QMetaObject::invokeMethod(qmlManager->mttab, "hideTab");
    QMetaObject::invokeMethod(boxUpperTab, "all_off");
    QMetaObject::invokeMethod(boundedBox, "hideBox");
}

bool QmlManager::isSelected(){
    if(selectedModels[0] == nullptr)
        return false;
    else
        return true;
}

void QmlManager::modelVisible(int ID, bool isVisible){
	auto target = getModelByID(ID);
	if (target)
	{
    	target->setEnabled(isVisible);
	}    
}

void QmlManager::doDelete(){
    if(selectedModels[0] == nullptr)
        return;
    for (int i=0; i<selectedModels.size(); i++){
        deleteModelFile(selectedModels[i]->ID);
    }
}

void QmlManager::doDeletebyID(int ID){
    deleteModelFile(ID);
}

void QmlManager::showMoveArrow(){
    if (selectedModels[0] == nullptr)
        return;
    moveArrow->setEnabled(1);
    moveArrowX->setEnabled(1);
    moveArrowY->setEnabled(1);

    size_t selectedNum = selectedModels.size();
    QQmlProperty::write(moveArrowobj, "center", QVector3D((selected_x_max(selectedNum) + selected_x_min(selectedNum))/2,
                                                          (selected_y_max(selectedNum) + selected_y_min(selectedNum))/2,
                                                          (selected_z_max(selectedNum) + selected_z_min(selectedNum))/2));
}

void QmlManager::hideMoveArrow(){
    moveArrow->setEnabled(0);
    moveArrowX->setEnabled(0);
    moveArrowY->setEnabled(0);
}

void QmlManager::hideRotateSphere(){
    rotateSphere->setEnabled(0);
}

void QmlManager::showRotatingSphere(){
    if (selectedModels[0] == nullptr)
        return;
    rotateSphere->setEnabled(1);
    rotateSphereX->setEnabled(1);
    rotateSphereY->setEnabled(1);
    rotateSphereZ->setEnabled(1);

//    size_t selectedNum = selectedModels.size();
//    QQmlProperty::write(rotateSphereobj, "center", QVector3D((selected_x_max(selectedNum) + selected_x_min(selectedNum))/2,
//                                                             (selected_y_max(selectedNum) + selected_y_min(selectedNum))/2,
//                                                             (selected_z_max(selectedNum) + selected_z_min(selectedNum))/2));

    QQmlProperty::write(rotateSphereobj,"center", //QVector3D(0,0,selectedModels[0]->m_transform->translation().z())+
            QVector3D((selectedModels[0]->getMesh()->x_max()+selectedModels[0]->getMesh()->x_min())/2,
            (selectedModels[0]->getMesh()->y_max()+selectedModels[0]->getMesh()->y_min())/2,
            (selectedModels[0]->getMesh()->z_max()-selectedModels[0]->getMesh()->z_min())/2));
}

void QmlManager::showRotateSphere(){
    if (selectedModels[0] == nullptr)
        return;
    rotateSphere->setEnabled(1);
    rotateSphereX->setEnabled(1);
    rotateSphereY->setEnabled(1);
    rotateSphereZ->setEnabled(1);

//    size_t selectedNum = selectedModels.size();
//    QQmlProperty::write(rotateSphereobj, "center", QVector3D((selected_x_max(selectedNum) + selected_x_min(selectedNum))/2,
//                                                             (selected_y_max(selectedNum) + selected_y_min(selectedNum))/2,
//                                                             (selected_z_max(selectedNum) + selected_z_min(selectedNum))/2));

    QQmlProperty::write(rotateSphereobj,"center",selectedModels[0]->m_transform->translation()+
            QVector3D((selectedModels[0]->getMesh()->x_max()+selectedModels[0]->getMesh()->x_min())/2,
            (selectedModels[0]->getMesh()->y_max()+selectedModels[0]->getMesh()->y_min())/2,
            (selectedModels[0]->getMesh()->z_max()+selectedModels[0]->getMesh()->z_min())/2));
}

void QmlManager::mouseHack(){
    //temporarily unuse. If no more bug about mouse release, delete this function.
    return ;

    const QPointF tmp_cor(265,105);
    QMouseEvent* evt = new QMouseEvent(QEvent::MouseButtonPress,tmp_cor,Qt::LeftButton, Qt::LeftButton,0);
    //QCoreApplication::postEvent(this->parent(),evt);
    QCoreApplication::postEvent(mainWindow,evt);
    QMouseEvent* evt2 = new QMouseEvent(QEvent::MouseButtonRelease,tmp_cor,Qt::LeftButton, Qt::LeftButton,0);
    //QCoreApplication::postEvent(this->parent(),evt2);
    QCoreApplication::postEvent(mainWindow,evt2);
}

void QmlManager::modelMoveInit(){
    for (GLModel* curModel: selectedModels){
        curModel->saveUndoState();
    }
}

void QmlManager::modelMoveDone(){
    if (selectedModels[0] == nullptr)
        return;

    QMetaObject::invokeMethod(boundedBox, "hideBox"); // Bounded Box
    hideMoveArrow();

    for (GLModel* curModel : selectedModels){
        curModel->shadowModel->m_objectPicker->setEnabled(true);
        //curModel->saveUndoState();

        /*QVector3D translationDiff = curModel->m_transform->translation()-curModel->m_translation;

        // move translation back to original
        curModel->m_transform->setTranslation(curModel->m_translation);
        curModel->moveModelMesh(translationDiff);*/
    }

//    QMetaObject::invokeMethod(boundedBox, "showBox"); // Bounded Box
//    QMetaObject::invokeMethod(boundedBox, "setPosition", Q_ARG(QVariant, selectedModels[0]->m_transform->translation()+QVector3D((selectedModels[0]->getMesh()->x_max()+selectedModels[0]->getMesh()->x_min())/2,(selectedModels[0]->getMesh()->y_max()+selectedModels[0]->getMesh()->y_min())/2,(selectedModels[0]->getMesh()->z_max()+selectedModels[0]->getMesh()->z_min())/2)));
//    QMetaObject::invokeMethod(boundedBox, "setSize", Q_ARG(QVariant, selectedModels[0]->getMesh()->x_max() - selectedModels[0]->getMesh()->x_min()),
//                                                     Q_ARG(QVariant, selectedModels[0]->getMesh()->y_max() - selectedModels[0]->getMesh()->y_min()),
//                                                     Q_ARG(QVariant, selectedModels[0]->getMesh()->z_max() - selectedModels[0]->getMesh()->z_min()));
    sendUpdateModelInfo();
    //if(Axis != 3){
        showMoveArrow();
        //QQmlProperty::write(moveArrowobj,"center",selectedModels[0]->m_transform->translation()+QVector3D((selectedModels[0]->getMesh()->x_max()+selectedModels[0]->getMesh()->x_min())/2,(selectedModels[0]->getMesh()->y_max()+selectedModels[0]->getMesh()->y_min())/2,(selectedModels[0]->getMesh()->z_max()+selectedModels[0]->getMesh()->z_min())/2));
    //}
    mouseHack();


}

void QmlManager::totalMoveDone(){
    for (GLModel* curModel : selectedModels){
        if (curModel == nullptr)
            continue;
        if (curModel->m_transform->translation().isNull()){
            qDebug() << "equals identity so don't save";
            continue;
        }

        //curModel->saveUndoState();

        curModel->moveModelMesh(curModel->m_transform->translation(), false);
        curModel->shadowModel->m_transform->setTranslation(curModel->shadowModel->m_transform->translation()+curModel->m_transform->translation());
        curModel->m_transform->setTranslation(QVector3D(0,0,0));
        // need to only update shadowModel & getMesh()
        emit curModel->_updateModelMesh(false);
    }
    sendUpdateModelInfo();
}

void QmlManager::modelRotateInit(){
    qDebug() << "model rotate init";
    for (GLModel* curModel : selectedModels){
        curModel->saveUndoState();
    }
    return;
}

void QmlManager::modelRotateDone(int Axis){
    if (selectedModels[0] == nullptr)
        return;

//    QMetaObject::invokeMethod(boundedBox, "hideBox"); // Bounded Box

    std::array<float,6> minmax;
    size_t selectedNum = selectedModels.size();
    for (size_t i=0; i < selectedNum; i++) {
        minmax = selectedModels[i]->getMesh()->calculateMinMax(quatToMat(selectedModels[i]->m_transform->rotation()).inverted());
        selectedModels[i]->m_transform->setTranslation(QVector3D(selectedModels[i]->m_transform->translation().x(),
                                                                 selectedModels[i]->m_transform->translation().y(),
                                                                 - minmax[4]));
    }
//    QMetaObject::invokeMethod(boundedBox, "showBox"); // Bounded Box
//    QMetaObject::invokeMethod(boundedBox, "setPosition", Q_ARG(QVariant, QVector3D(selectedModels[0]->m_transform->translation().x(),
//                                                               selectedModels[0]->m_transform->translation().y(),
//                                                               -minmax[4])+QVector3D((minmax[0]+minmax[1])/2,(minmax[2]+minmax[3])/2,(minmax[4]+minmax[5])/2)));
//    QMetaObject::invokeMethod(boundedBox, "setSize", Q_ARG(QVariant, minmax[1]-minmax[0]),
//                                                     Q_ARG(QVariant, minmax[3]-minmax[2]),
//                                                     Q_ARG(QVariant, minmax[5]-minmax[4]));

//    QString size;
//    size.sprintf("%.1f X %.1f X %.1f mm", (minmax[1]-minmax[0]),
//            (minmax[3]-minmax[2]),
//            (minmax[5]-minmax[4]));
//    sendUpdateModelInfo(0,0,size,0);

//    sendUpdateModelInfo();

    showRotatingSphere();
    mouseHack();
    rotateSnapAngle = 0;
}

void QmlManager::totalRotateDone(){
    qDebug() << "total rotate done" << selectedModels.size();
    for (int i=0; i<selectedModels.size(); i++){
        if (selectedModels[i] == nullptr)
            continue;
        qDebug() << selectedModels[i]->m_transform->rotation();
        if (selectedModels[i]->m_transform->rotation().isIdentity()){
            qDebug() << "equals identity so don't save";
            continue;
        }
        selectedModels[i]->rotationDone();
    }

    sendUpdateModelInfo();
}


void QmlManager::modelMove(int Axis, int Distance){ // for QML Signal -> float is not working in qml signal parameter
    if (selectedModels[0] == nullptr)
        return;

    for (int i=0; i<selectedModels.size(); i++){
        selectedModels[i]->shadowModel->m_objectPicker->setEnabled(false);
        switch(Axis){
            case 1:{  //X
                QVector3D tmp = selectedModels[i]->m_transform->translation();

                if ((tmp.x() + selectedModels[i]->getMesh()->x_max() + 1 < scfg->bed_x/2)
                        && (tmp.x() + selectedModels[i]->getMesh()->x_min() - 1 > -scfg->bed_x/2)) {
                    if(tmp.x() + selectedModels[i]->getMesh()->x_max() +1 + Distance > scfg->bed_x/2 )
                        return ;
                    if(tmp.x() + selectedModels[i]->getMesh()->x_min() -1 + Distance < - scfg->bed_x/2 )
                        return ;
                }
                selectedModels[i]->m_transform->setTranslation(QVector3D(tmp.x()+Distance,tmp.y(),tmp.z()));
                break;
            }
            case 2:{  //Y
                QVector3D tmp = selectedModels[i]->m_transform->translation();
                if ((tmp.y() + selectedModels[i]->getMesh()->y_max() + 1 < scfg->bed_y/2)
                        && (tmp.y() + selectedModels[i]->getMesh()->y_min() - 1 > -scfg->bed_y/2)) {
                    if(tmp.y() + selectedModels[i]->getMesh()->y_max() +1 + Distance > scfg->bed_y/2 )
                        return;
                    if(tmp.y() + selectedModels[i]->getMesh()->y_min() -1 + Distance < - scfg->bed_y/2 )
                        return;
                }
                selectedModels[i]->m_transform->setTranslation(QVector3D(tmp.x(),tmp.y()+Distance,tmp.z()));
                break;
            }
        }
        selectedModels[i]->checkPrintingArea();
    }

    // update bounded box position, size
    updateBoundedBox();

    //QMetaObject::invokeMethod(boundedBox, "setPosition", Q_ARG(QVariant, selectedModels[0]->m_transform->translation()+QVector3D((selectedModels[0]->getMesh()->x_max()+selectedModels[0]->getMesh()->x_min())/2,(selectedModels[0]->getMesh()->y_max()+selectedModels[0]->getMesh()->y_min())/2,(selectedModels[0]->getMesh()->z_max()+selectedModels[0]->getMesh()->z_min())/2)));
    //QMetaObject::invokeMethod(boundedBox, "setSize", Q_ARG(QVariant, selectedModels[0]->getMesh()->x_max() - selectedModels[0]->getMesh()->x_min()),
    //                                                 Q_ARG(QVariant, selectedModels[0]->getMesh()->y_max() - selectedModels[0]->getMesh()->y_min()),
    //                                                 Q_ARG(QVariant, selectedModels[0]->getMesh()->z_max() - selectedModels[0]->getMesh()->z_min()));
}

void QmlManager::modelMoveF(int Axis, float Distance){
    if (selectedModels[0] == nullptr)
        return;

    for (int i=0; i<selectedModels.size(); i++){
        switch(Axis){
            case 1:{  //X
                QVector3D tmp = selectedModels[i]->m_transform->translation();

                if ((tmp.x() + selectedModels[i]->getMesh()->x_max() + 1 < scfg->bed_x/2)
                        && (tmp.x() + selectedModels[i]->getMesh()->x_min() - 1 > -scfg->bed_x/2)) {
                    if(tmp.x() + selectedModels[i]->getMesh()->x_max() +1 + Distance > scfg->bed_x/2 )
                        return;
                    if(tmp.x() + selectedModels[i]->getMesh()->x_min() -1 + Distance < - scfg->bed_x/2 )
                        return;
                }
                selectedModels[i]->m_transform->setTranslation(QVector3D(tmp.x()+Distance, tmp.y(), tmp.z()));
                break;
            }
            case 2:{  //Y
                QVector3D tmp = selectedModels[i]->m_transform->translation();
                if ((tmp.y() + selectedModels[i]->getMesh()->y_max() + 1 < scfg->bed_y/2)
                        && (tmp.y() + selectedModels[i]->getMesh()->y_min() - 1 > -scfg->bed_y/2)) {
                    if(tmp.y() + selectedModels[i]->getMesh()->y_max() +1 + Distance > scfg->bed_y/2 )
                        return;
                    if(tmp.y() + selectedModels[i]->getMesh()->y_min() -1 + Distance < - scfg->bed_y/2 )
                        return;
                }
                selectedModels[i]->m_transform->setTranslation(QVector3D(tmp.x(), tmp.y()+Distance, tmp.z()));
                break;
            }
        }
        selectedModels[i]->checkPrintingArea();
    }
    // (bounded_box) move by drag
    updateBoundedBox();
}
void QmlManager::modelRotate(int Axis, int Angle){
    if (selectedModels[0] == nullptr)
        return;

    for (int i=0; i<selectedModels.size(); i++){
        rotateSnapAngle = (rotateSnapAngle + Angle +360) % 360;
        QVector3D transl = selectedModels[i]->m_transform->translation();
        //selectedModels[i]->m_transform->setTranslation(selectedModels[i]->m_translation);
        QVector3D rot_center = QVector3D((selectedModels[i]->getMesh()->x_max()+selectedModels[i]->getMesh()->x_min())/2,
                                                  (selectedModels[i]->getMesh()->y_max()+selectedModels[i]->getMesh()->y_min())/2,
                                                  (selectedModels[i]->getMesh()->z_max()+selectedModels[i]->getMesh()->z_min())/2);

        QMatrix4x4 rot;

        switch(Axis){
            case 1:{  //X
                float tmpx = selectedModels[i]->m_transform->rotationX();

                if (QApplication::queryKeyboardModifiers() & Qt::ShiftModifier){// snap mode
                    if(rotateSnapAngle>0 && rotateSnapAngle<90){
                        rot = selectedModels[i]->m_transform->rotateAround(rot_center,rotateSnapAngle-tmpx,(QVector3D(1,0,0).toVector4D()*selectedModels[i]->m_transform->matrix()).toVector3D());
    //                    selectedModels[i]->m_transform->setRotationX(rotateSnapStartAngle + 0);
                    }
                    else if(rotateSnapAngle>90 && rotateSnapAngle<180){
                        rot = selectedModels[i]->m_transform->rotateAround(rot_center,rotateSnapAngle+90-tmpx,(QVector3D(1,0,0).toVector4D()*selectedModels[i]->m_transform->matrix()).toVector3D());
    //                    selectedModels[i]->m_transform->setRotationX(rotateSnapStartAngle + 90);
                    }
                    else if(rotateSnapAngle>180 && rotateSnapAngle<270){
                        rot = selectedModels[i]->m_transform->rotateAround(rot_center,rotateSnapAngle+180-tmpx,(QVector3D(1,0,0).toVector4D()*selectedModels[i]->m_transform->matrix()).toVector3D());
    //                    selectedModels[i]->m_transform->setRotationX(rotateSnapStartAngle + 180);
                    }
                    else if(rotateSnapAngle>270 && rotateSnapAngle<360){
                        rot = selectedModels[i]->m_transform->rotateAround(rot_center,rotateSnapAngle+270-tmpx,(QVector3D(1,0,0).toVector4D()*selectedModels[i]->m_transform->matrix()).toVector3D());
    //                    selectedModels[i]->m_transform->setRotationX(rotateSnapStartAngle + 270);
                    }
                }
                else
    //                selectedModels[i]->m_transform->setRotationX(tmpx+Angle);
                    rot = selectedModels[i]->m_transform->rotateAround(rot_center,Angle,(QVector3D(1,0,0).toVector4D()*selectedModels[i]->m_transform->matrix()).toVector3D());
                selectedModels[i]->m_transform->setMatrix(selectedModels[i]->m_transform->matrix()*rot);
                break;
            }
            case 2:{  //Y
                float tmpy = selectedModels[i]->m_transform->rotationY();

                if (QApplication::queryKeyboardModifiers() & Qt::ShiftModifier){// snap mode
                    if(rotateSnapAngle>0 && rotateSnapAngle<90){
                        rot = selectedModels[i]->m_transform->rotateAround(rot_center,rotateSnapAngle-tmpy,(QVector3D(0,1,0).toVector4D()*selectedModels[i]->m_transform->matrix()).toVector3D());
    //                    selectedModels[i]->m_transform->setRotationY(rotateSnapStartAngle + 0);
                    }
                    else if(rotateSnapAngle>90 && rotateSnapAngle<180){
                        rot = selectedModels[i]->m_transform->rotateAround(rot_center,rotateSnapAngle+90-tmpy,(QVector3D(0,1,0).toVector4D()*selectedModels[i]->m_transform->matrix()).toVector3D());
    //                    selectedModels[i]->m_transform->setRotationY(rotateSnapStartAngle + 90);
                    }
                    else if(rotateSnapAngle>180 && rotateSnapAngle<270){
                        rot = selectedModels[i]->m_transform->rotateAround(rot_center,rotateSnapAngle+180-tmpy,(QVector3D(0,1,0).toVector4D()*selectedModels[i]->m_transform->matrix()).toVector3D());
    //                    selectedModels[i]->m_transform->setRotationY(rotateSnapStartAngle + 180);
                    }
                    else if(rotateSnapAngle>270 && rotateSnapAngle<360){
                        rot = selectedModels[i]->m_transform->rotateAround(rot_center,rotateSnapAngle+270-tmpy,(QVector3D(0,1,0).toVector4D()*selectedModels[i]->m_transform->matrix()).toVector3D());
    //                    selectedModels[i]->m_transform->setRotationY(rotateSnapStartAngle + 270);
                    }
                }
                else
    //                selectedModels[i]->m_transform->setRotationY(tmpy+Angle);
                    rot = selectedModels[i]->m_transform->rotateAround(rot_center,Angle,(QVector3D(0,1,0).toVector4D()*selectedModels[i]->m_transform->matrix()).toVector3D());
                selectedModels[i]->m_transform->setMatrix(selectedModels[i]->m_transform->matrix()*rot);
                break;
            }
            case 3:{  //Z
                float tmpz = selectedModels[i]->m_transform->rotationZ();

                if (QApplication::queryKeyboardModifiers() & Qt::ShiftModifier){// snap mode
                    if(rotateSnapAngle>0 && rotateSnapAngle<90){
                        rot = selectedModels[i]->m_transform->rotateAround(rot_center,rotateSnapAngle-tmpz,(QVector3D(0,0,1).toVector4D()*selectedModels[i]->m_transform->matrix()).toVector3D());
    //                    selectedModels[i]->m_transform->setRotationZ(rotateSnapStartAngle + 0);
                    }
                    else if(rotateSnapAngle>90 && rotateSnapAngle<180){
                        rot = selectedModels[i]->m_transform->rotateAround(rot_center,rotateSnapAngle+90-tmpz,(QVector3D(0,0,1).toVector4D()*selectedModels[i]->m_transform->matrix()).toVector3D());
    //                    selectedModels[i]->m_transform->setRotationZ(rotateSnapStartAngle + 90);
                    }
                    else if(rotateSnapAngle>180 && rotateSnapAngle<270){
                        rot = selectedModels[i]->m_transform->rotateAround(rot_center,rotateSnapAngle+180-tmpz,(QVector3D(0,0,1).toVector4D()*selectedModels[i]->m_transform->matrix()).toVector3D());
    //                    selectedModels[i]->m_transform->setRotationZ(rotateSnapStartAngle + 180);
                    }
                    else if(rotateSnapAngle>270 && rotateSnapAngle<360){
                        rot = selectedModels[i]->m_transform->rotateAround(rot_center,rotateSnapAngle+270-tmpz,(QVector3D(0,0,1).toVector4D()*selectedModels[i]->m_transform->matrix()).toVector3D());
    //                    selectedModels[i]->m_transform->setRotationZ(rotateSnapStartAngle + 270);
                    }
                }
                else
    //                selectedModels[i]->m_transform->setRotationZ(tmpz+Angle);
                    rot = selectedModels[i]->m_transform->rotateAround(rot_center,Angle,(QVector3D(0,0,1).toVector4D()*selectedModels[i]->m_transform->matrix()).toVector3D());
                selectedModels[i]->m_transform->setMatrix(selectedModels[i]->m_transform->matrix()*rot);
                break;
            }
        }
        //selectedModels[i]->m_transform->setTranslation(selectedModels[i]->m_transform->translation()+QVector3D(0,0,-selectedModels[i]->getMesh()->z_min()));
    }
}

void QmlManager::modelMoveByNumber(int axis, int X, int Y){
    if (selectedModels[0] == nullptr)
        return;

    for (int i=0; i<selectedModels.size(); i++){
        QVector3D tmp = selectedModels[i]->m_transform->translation();
        int targetX, targetY;

        targetX = tmp.x() + X;
        targetY = tmp.y() + Y;

        if(tmp.x() + selectedModels[i]->getMesh()->x_max() +1 + X> 80/2 )
            targetX = tmp.x() - (tmp.x() + selectedModels[i]->getMesh()->x_max() - scfg->bed_x/2 + 1);
        if(tmp.x() + selectedModels[i]->getMesh()->x_min() -1 + X< - 80/2 )
            targetX = tmp.x() - (tmp.x() + selectedModels[i]->getMesh()->x_min() + scfg->bed_x/2 - 1);

        if(tmp.y() + selectedModels[i]->getMesh()->y_max() +1 + Y> 80/2 )
            targetY = tmp.y() - (tmp.y() + selectedModels[i]->getMesh()->y_max() - scfg->bed_y/2 + 1);
        if(tmp.y() + selectedModels[i]->getMesh()->y_min() -1 + Y< - 80/2 )
            targetY = tmp.y() - (tmp.y() + selectedModels[i]->getMesh()->y_min() + scfg->bed_y/2 - 1);

        selectedModels[i]->m_transform->setTranslation(QVector3D(targetX, targetY, tmp.z()));
        //selectedModels[i]->moveModelMesh(QVector3D(targetX,targetY,tmp.z()));
    }

    modelMoveDone();
}
void QmlManager::modelRotateByNumber(int axis,  int X, int Y, int Z){
    if (selectedModels[0] == nullptr)
        return;

    for (int i=0; i<selectedModels.size(); i++){
        QVector3D rot_center = QVector3D((selectedModels[i]->getMesh()->x_max()+selectedModels[i]->getMesh()->x_min())/2,
                                                  (selectedModels[i]->getMesh()->y_max()+selectedModels[i]->getMesh()->y_min())/2,
                                                  (selectedModels[i]->getMesh()->z_max()+selectedModels[i]->getMesh()->z_min())/2);

        selectedModels[i]->rotateByNumber(rot_center, X, Y, Z);
    }
    //showRotateSphere();
    mouseHack();
}
void QmlManager::resetLayflat(){
    QMetaObject::invokeMethod(layflatPopup,"onApplyFinishButton");
}

void QmlManager::save() {
    STLexporter* ste = new STLexporter();

    qDebug() << "file save called";
    if (qmlManager->selectedModels.size() == 1 && qmlManager->selectedModels[0] == nullptr) {
        qmlManager->closeSave();
        QMetaObject::invokeMethod(boxUpperTab, "all_off");
        return;
    }
    QString fileName = QFileDialog::getSaveFileName(nullptr, tr("Save to STL file"), "", tr("3D Model file (*.stl)"));
    if(fileName == "") {
        qmlManager->closeSave();
        QMetaObject::invokeMethod(boxUpperTab, "all_off");
        return;
    }
    qmlManager->openProgressPopUp();
    QFuture<void> future = QtConcurrent::run(ste, &STLexporter::exportSTL, fileName);
    return;
}

void QmlManager::groupSelectionActivate(bool active){
    if (active){
        groupSelectionActive = true;
    } else {
        groupSelectionActive = false;
    }
}

void QmlManager::runGroupFeature(int ftrType, QString state, double arg1, double arg2, double arg3, QVariant data){
    groupFunctionIndex = ftrType;
    groupFunctionState = state;

    qDebug()<< "runGroupFeature | type:"<<ftrType<<"| state:" <<state << selectedModels.size();
    switch(ftrType){
    /*
    case ftrSave:
    {
        if (state == "active") {
            if (selectedModels[0] == nullptr){
                QMetaObject::invokeMethod(savePopup,"offApplyFinishButton");
            }else{
                QMetaObject::invokeMethod(savePopup,"onApplyFinishButton");
            }
        }
        else if (state == "inactive") {

        }
        break;
    }
    */
    case ftrRotate: //rotate
    {
        if (state == "inactive"){
            hideRotateSphere();

        }else if(state == "active"){
            showRotateSphere();
            if (selectedModels[0] == nullptr){
                QMetaObject::invokeMethod(rotatePopup,"offApplyFinishButton");
            }else{
                QMetaObject::invokeMethod(rotatePopup,"onApplyFinishButton");
            }

        }
        break;
    }
    case ftrMove:  //move
    {
        if (state == "inactive"){
            moveArrow->setEnabled(0);
        }else if(state == "active"){
            showMoveArrow();
            if (selectedModels[0] == nullptr){
                QMetaObject::invokeMethod(movePopup,"offApplyFinishButton");
            }else{
                QMetaObject::invokeMethod(movePopup,"onApplyFinishButton");
            }
        }
        break;
    }
    case ftrLayFlat:
    {   /*
        if (state == "active"){
            if (selectedModels[0] == nullptr){
                QMetaObject::invokeMethod(layflatPopup,"offApplyFinishButton");
            }else{
                QMetaObject::invokeMethod(layflatPopup,"onApplyFinishButton");
            }
        }else if (state == "inactive"){
            QApplication::restoreOverrideCursor();
        }
        */
        qDebug() << "run groupfeature lay flat";
        if (state == "active"){
            if (selectedModels[0] != nullptr){
                selectedModels[0]->uncolorExtensionFaces();
                selectedModels[0]->colorExtensionFaces();
            }
        }else if (state == "inactive"){
            if (selectedModels[0] != nullptr){
                selectedModels[0]->uncolorExtensionFaces();
                selectedModels[0]->shadowModel->closeExtension();
            }
        }
/*        if (selectedModels[0] != nullptr) {
            selectedModels[selectedModels.size() - 1]->uncolorExtensionFaces();
            selectedModels[selectedModels.size() - 1]->shadowModel->closeLayflat();
        }
*/
        break;
    }
    case ftrOrient:  //orient
    {
        if (state == "active"){
            if (selectedModels[0] == nullptr){
                QMetaObject::invokeMethod(orientPopup,"offApplyFinishButton");
            }else{
                QMetaObject::invokeMethod(orientPopup,"onApplyFinishButton");
            }
        }
        break;
    }
    case ftrRepair:  //repair
    {
        if (state == "active"){
            if (selectedModels[0] == nullptr){
                QMetaObject::invokeMethod(repairPopup,"offApplyFinishButton");
            }else{
                QMetaObject::invokeMethod(repairPopup,"onApplyFinishButton");
            }
        }
        break;
    }
    case ftrExtend:
        qDebug() << "run groupfeature extend";
        if (state == "active"){
            if (selectedModels[0] != nullptr){
                selectedModels[0]->uncolorExtensionFaces();
                selectedModels[0]->colorExtensionFaces();
            }
        }else if (state == "inactive"){
            if (selectedModels[0] != nullptr){
                selectedModels[0]->uncolorExtensionFaces();
                selectedModels[0]->shadowModel->closeExtension();
            }
        }
        qDebug() << "groupfeature done";
        break;
    case ftrScale:
        qDebug() << "run feature scale" << selectedModels.size();
        if (state == "active"){
            for (GLModel* model : selectedModels){
                model->ft->setTypeAndRun(ftrType, arg1, arg2, arg3);
            }
            sendUpdateModelInfo();
        } else {

        }
        break;
    case ftrExport:
        // save to temporary folder
        qDebug() << "file export called";
        QString fileName;

        // look for data if it is temporary
        QVariantMap config = data.toMap();
        bool isTemporary = false;
        for (QVariantMap::const_iterator iter = config.begin(); iter != config.end(); ++iter){
            if (!strcmp(iter.key().toStdString().c_str(), "temporary")){
                qDebug() << "iter value : " << iter.value().toString();
                if (iter.value().toString() == "true"){
                    isTemporary = true;
                }
            }
        }

        if (! isTemporary){ // export view
            qDebug() << "export to file";
            fileName = QFileDialog::getSaveFileName(nullptr, tr("Export sliced file"), "");
            //ste->exportSTL(m_glmodel->getMesh(), fileName);
            if(fileName == "")
                return;
        } else { // support view & layerview
            qDebug() << "export to temp file";
            fileName =  QDir::tempPath();
            qDebug() << fileName;
        }


        STLexporter* ste = new STLexporter();
        SlicingEngine* se = new SlicingEngine();

        qmlManager->openProgressPopUp();

        // merge selected models
        Mesh* mergedShellMesh = ste->mergeSelectedModels();//ste->mergeModels(qmlManager->selectedModels);
        //GLModel* mergedModel = new GLModel(mainWindow, models, mergedMesh, "temporary", false);

        qDebug() << "1111";
        // generate support
        GenerateSupport generatesupport;
        Mesh* mergedSupportMesh = generatesupport.generateSupport(mergedShellMesh);

        qDebug() << "2222";
        // generate raft
        GenerateRaft generateraft;
        Mesh* mergedRaftMesh = generateraft.generateRaft(mergedShellMesh);

        qDebug() << "3333";
        // need to generate support, raft

        //se->slice(data, mergedMesh, fileName);
        QFuture<Slicer*> future = QtConcurrent::run(se, &SlicingEngine::slice, data, mergedShellMesh, mergedSupportMesh, mergedRaftMesh, fileName);
        //deleteOneModelFile(mergedModel->ID);

        //m_glmodel->futureWatcher.setFuture(future);
        break;
    }
}

void QmlManager::unDo(){
    if (selectedModels.size() == 1 && selectedModels[0] == nullptr && glmodels.size()>=1){
        // do global undo
        GLModel* recentModel = nullptr;
        // find global recent model
        for (auto& pair : glmodels){
			auto glmodel = &pair.second;
            if (glmodel->getMesh()->getPrev() == nullptr || glmodel->getMesh()->getPrev()->getTime().isNull())
                continue;
            else if (recentModel == nullptr)
                recentModel = glmodel;
            else if (glmodel->getMesh()->getPrev()->getTime() >= recentModel->getMesh()->getPrev()->getTime()){
                recentModel = glmodel;
            }
        }

        // undo recentModel
        if (recentModel != nullptr)
            recentModel->loadUndoState();
    } else if (glmodels.size()>=1){
        // do local undo
        selectedModels[0]->loadUndoState();
    }
    sendUpdateModelInfo();
    return;
}

void QmlManager::reDo(){
    if (selectedModels.size() == 1 && selectedModels[0] == nullptr && glmodels.size()>=1){
        // do global redo
        GLModel* recentModel = nullptr;
        // find global recent model
		for (auto& pair : glmodels) {
			auto glmodel = &pair.second;
            if (glmodel->getMesh()->getNext() == nullptr || glmodel->getMesh()->getNext()->getTime().isNull())
                continue;
            else if (recentModel == nullptr)
                recentModel = glmodel;
            else if (glmodel->getMesh()->getNext()->getTime() >= recentModel->getMesh()->getNext()->getTime()){
                recentModel = glmodel;
            }
        }
        // undo recentModel
        if (recentModel != nullptr)
            recentModel->loadRedoState();
    } else if (glmodels.size()>=1) {
        // do local redo
        selectedModels[0]->loadRedoState();
    }
    sendUpdateModelInfo();
    return;
}

void QmlManager::copyModel(){
    copyMeshes.clear();
    copyMeshNames.clear();

    qDebug() << "copying current selected Models";
    for (GLModel* model : selectedModels){
        if (model == nullptr)
            continue;
        Mesh* copied = model->getMesh()->copyMesh();

        copyMeshes.push_back(copied);
        copyMeshNames.push_back(model->filename);
        /*foreach (MeshFace mf, model->getMesh()->faces){
            copyMesh->addFace(model->getMesh()->idx2MV(mf.mesh_vertex[0]).position, model->getMesh()->idx2MV(mf.mesh_vertex[1]).position, model->getMesh()->idx2MV(mf.mesh_vertex[2]).position, mf.idx);
        }
        copyMesh->connectFaces();
        copyMeshes.push_back(copyMesh);*/
    }
    return;
}

void QmlManager::pasteModel(){
    if(copyMeshes.size() < 1) // clipboard check
        return ;

    qDebug() << "pasting current saved selected Meshes";
    int counter = 0;
    for (Mesh* copyMesh : copyMeshes){
        QString temp = copyMeshNames.at(counter).split(".").first() + QString::fromStdString("_copy_") + QString::number(modelIDCounter);

        createModelFile(copyMesh, temp);
        counter++;
    }

    openArrange();

    return;
}

void QmlManager::addPart(QString fileName, int ID){
    QMetaObject::invokeMethod(partList, "addPart", Q_ARG(QVariant, fileName), Q_ARG(QVariant, ID));
}

void QmlManager::deletePart(int ID){
    QMetaObject::invokeMethod(partList, "deletePart", Q_ARG(QVariant, ID));
    QMetaObject::invokeMethod(yesno_popup, "deletePart", Q_ARG(QVariant, ID));
}

void QmlManager::deleteList(int ID) {
    qDebug() << "deleteList";
    QMetaObject::invokeMethod(yesno_popup, "deletePart", Q_ARG(QVariant, ID));
}

void QmlManager::openProgressPopUp(){
    progress = 0;
    QMetaObject::invokeMethod(progress_popup, "openPopUp");
}

void QmlManager::openYesNoPopUp(bool selectedList_vis, std::string inputText_h, std::string inputText_m, std::string inputText_l, int inputText_fontsize, std::string image_source, int inputPopupType, int yesNo_okCancel){
    QMetaObject::invokeMethod(yesno_popup, "openYesNoPopUp",
                              Q_ARG(QVariant, selectedList_vis),
                              Q_ARG(QVariant, QString::fromStdString(inputText_h)),
                              Q_ARG(QVariant, QString::fromStdString(inputText_m)),
                              Q_ARG(QVariant, QString::fromStdString(inputText_l)),
                              Q_ARG(QVariant, inputText_fontsize),
                              Q_ARG(QVariant, QString::fromStdString(image_source)),
                              Q_ARG(QVariant, inputPopupType),
                              Q_ARG(QVariant, yesNo_okCancel));
}

void QmlManager::openResultPopUp(std::string inputText_h, std::string inputText_m, std::string inputText_l){
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

void QmlManager::openSave() {
    qDebug() << "open Save";
    saveActive = true;
    return;
}

void QmlManager::closeSave() {
    qDebug() << "close Save";
    saveActive = false;
    return;
}

void QmlManager::openMove(){
    //moveActive = true;
    return;
}

void QmlManager::closeMove(){
    qDebug() << "close move";
    totalMoveDone();
    return;
}


void QmlManager::openRotate(){
    qDebug() << "open Rotate";
    QMetaObject::invokeMethod(qmlManager->boundedBox, "hideBox");
    slicingData->setProperty("visible", false);
    rotateActive = true;
    return;
}

void QmlManager::closeRotate(){
    qDebug() << "close Rotate";
    rotateActive = false;
    totalRotateDone();
    return;
}

void QmlManager::openOrientation(){
    qDebug() << "open orientation";
    orientationActive = true;
    return;
}

void QmlManager::closeOrientation(){
    qDebug() << "close orientation";
    orientationActive = false;
    sendUpdateModelInfo();
    return;
}

void QmlManager::setProgressText(std::string inputText){
    QMetaObject::invokeMethod(progress_popup, "updateText",
                              Q_ARG(QVariant, QString::fromStdString(inputText)));
}

void QmlManager::viewObjectChanged(bool checked){
    qInfo() << "viewObjectChanged" << checked;
    qDebug() << "selected Num = " << selectedModels.size();
    if( checked ) {
        setViewMode(VIEW_MODE_OBJECT);
    }
}

void QmlManager::viewSupportChanged(bool checked){
    qInfo() << "viewSupportChanged" << checked;
    qDebug() << "selected Num = " << selectedModels.size();
    if( checked ) {
        if( selectedModels[0] != nullptr ) {
            /*if( selectedModels[0]->slicer == nullptr ) {
                qmlManager->openYesNoPopUp(false, "The model should be sliced for support view.", "", "Would you like to continue?", 16, "", ftrSupportViewMode, 0);
            }*/
            if (selectedModels[0]->getSupport() == nullptr) {
                qmlManager->openYesNoPopUp(false, "[Support View]", "", "Would you like to continue?", 16, "", ftrSupportViewMode, 0);
            } else {
                QMetaObject::invokeMethod(qmlManager->boxUpperTab, "all_off");
                setViewMode(VIEW_MODE_SUPPORT);
            }
        }
    }
}

void QmlManager::viewLayerChanged(bool checked){
    qInfo() << "viewLayerChanged" << checked;
    qDebug() << "selected Num = " << selectedModels.size();
    if( checked ) {
        if( selectedModels[0] != nullptr ) {
            if( selectedModels[0]->slicer == nullptr ) {
                qmlManager->openYesNoPopUp(false, "The model should be sliced for layer view.", "", "Would you like to continue?", 16, "", ftrLayerViewMode, 0);
            } else {
                QMetaObject::invokeMethod(qmlManager->boxUpperTab, "all_off");
                setViewMode(VIEW_MODE_LAYER);
            }
        }
    }
}

void QmlManager::layerInfillButtonChanged(bool checked){
    qInfo() << "layerInfillButtonChanged" << checked;
    if( checked ) {
        layerViewFlags |= LAYER_INFILL;
    } else {
        layerViewFlags &= ~LAYER_INFILL;
    }

    if( selectedModels[0] != nullptr ) {
        emit selectedModels[0]->_updateModelMesh(true);
    }
}

void QmlManager::layerSupportersButtonChanged(bool checked){
    qInfo() << "layerSupportersButtonChanged" << checked;
    if( checked ) {
        layerViewFlags |= LAYER_SUPPORTERS;
    } else {
        layerViewFlags &= ~LAYER_SUPPORTERS;
    }

    if( selectedModels[0] != nullptr ) {
        qDebug() << "calling updatemodelmesh ";
        emit selectedModels[0]->_updateModelMesh(true);
    }
}

void QmlManager::layerRaftButtonChanged(bool checked){
    qInfo() << "layerRaftButtonChanged" << checked << layerViewFlags;
    if( checked ) {
        layerViewFlags |= LAYER_RAFT;
    } else {
        layerViewFlags &= ~LAYER_RAFT;
    }
    qInfo() << LAYER_INFILL << LAYER_SUPPORTERS << LAYER_RAFT;
    qInfo() << "layerRaftButtonChanged" << checked << layerViewFlags;

    if( selectedModels[0] != nullptr ) {
        emit selectedModels[0]->_updateModelMesh(true);
    }
}

void QmlManager::setViewMode(int viewMode) {
    qInfo() << "setViewMode" << viewMode;
    qDebug() << "current mode" << this->viewMode;

    if( this->viewMode != viewMode ) {
        //if (viewMode == 0) viewObjectButton->setProperty("checked", true);
        if (viewMode == 1) viewSupportButton->setProperty("checked", true);
        else if (viewMode == 2) viewLayerButton->setProperty("checked", true);

        this->viewMode = viewMode;
        layerViewPopup->setProperty("visible", this->viewMode == VIEW_MODE_LAYER);
        layerViewSlider->setProperty("visible", this->viewMode == VIEW_MODE_LAYER);

        if( selectedModels[0] != nullptr ) {
            QMetaObject::invokeMethod(layerViewSlider, "setThickness", Q_ARG(QVariant, (scfg->layer_height)));
            QMetaObject::invokeMethod(layerViewSlider, "setHeight", Q_ARG(QVariant, (selectedModels[0]->getMesh()->z_max() - selectedModels[0]->getMesh()->z_min() + scfg->raft_thickness)));
            this->selectedModels[0]->changeViewMode(viewMode);
        }
    }

    if( this->viewMode == VIEW_MODE_OBJECT ) {
        QMetaObject::invokeMethod(yesno_popup, "closePopUp");
        QMetaObject::invokeMethod(leftTabViewMode, "setObjectView");
    } else if (this->viewMode == VIEW_MODE_SUPPORT){
        QMetaObject::invokeMethod(qmlManager->boundedBox, "hideBox");
        selectedModels[0]->setSupport();
        emit  selectedModels[0]->_updateModelMesh(true);
    } else if (this->viewMode == VIEW_MODE_LAYER){
        qDebug() << "view mode layer called";
        QMetaObject::invokeMethod(qmlManager->boundedBox, "hideBox");
        selectedModels[0]->setSupport();
        qDebug() << "generated support";
        emit selectedModels[0]->_updateModelMesh(true);
    } else {
        qDebug() << "view mode layer called";
        QMetaObject::invokeMethod(qmlManager->boundedBox, "hideBox");
    }
}

int QmlManager::getViewMode() {
    return viewMode;
}

int QmlManager::getLayerViewFlags() {
    return layerViewFlags;
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

void QmlManager::unselectPartImpl(GLModel* target)
{

    target->changecolor(0);
    target->checkPrintingArea();
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
    if( selectedModels[0] != nullptr ) {
        selectedModels[0]->changeViewMode(VIEW_MODE_OBJECT);
    }

    for (auto it=selectedModels.begin(); it!= selectedModels.end(); ++it){
        if (*it == target) {
            selectedModels.erase(it);
            break;
        }
    }
    if (selectedModels.size() == 0) selectedModels.push_back(nullptr);
    //selectedModels.clear();
    //selectedModels.push_back(nullptr);
    //selectedModels[0] = nullptr;
    QMetaObject::invokeMethod(leftTabViewMode, "setEnable", Q_ARG(QVariant, false));
    //QMetaObject::invokeMethod(boundedBox, "hideBox"); // Bounded Box
    sendUpdateModelInfo();
}


