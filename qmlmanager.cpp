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
#include <Qt3DCore>
#include <qquickitem.h>
#include <ctype.h>
#include <QCoreApplication>
#include <QTextStream>
#include <QFileDialog>
#include <exception>
#include "qmlmanager.h"
#include "utils/utils.h"
#include "render/lights.h"
#include "DentEngine/src/configuration.h"
#include "feature/stlexporter.h"
#include <functional>
using namespace Hix::Input;
using namespace Hix::UI;
using namespace Hix::Render;
using namespace Hix::Tasking;

QmlManager::QmlManager(QObject *parent) : QObject(parent), _optBackend(this, scfg)
  ,layerViewFlags(LAYER_INFILL | LAYER_SUPPORTERS | LAYER_RAFT), modelIDCounter(0), _cursorEraser(QPixmap(":/Resource/cursor_eraser.png"))
{
}

void QmlManager::initializeUI(QQmlApplicationEngine* e){
    engine = e;
	//initialize ray casting mouse input controller
	QEntity* camera = dynamic_cast<QEntity*>(FindItemByName(engine, "cm"));
	_rayCastController.initialize(camera);

    mainWindow = FindItemByName(engine, "mainWindow");
    loginWindow = FindItemByName(engine, "loginWindow");
    loginButton = FindItemByName(engine, "loginButton");
    keyboardHandler = (Qt3DInput::QKeyboardHandler*)FindItemByName(engine, "keyboardHandler");
    models = (QEntity *)FindItemByName(engine, "Models");
    Lights* lights = new Lights(models);
    mv = dynamic_cast<QEntity*> (FindItemByName(engine, "MainView"));
    systemTransform = (Qt3DCore::QTransform *) FindItemByName(engine, "systemTransform");
    mttab = (QEntity *)FindItemByName(engine, "mttab");
    QMetaObject::invokeMethod(mv, "initCamera");

	auto total = dynamic_cast<QEntity*> (FindItemByName(engine, "total"));
	_camera = dynamic_cast<Qt3DRender::QCamera*> (FindItemByName(engine, "camera"));

	_widgetManager.initialize(total, &_rayCastController);
	_supportRaftManager.initialize(models);

    // model move componetns
    moveButton = FindItemByName(engine, "moveButton");
    movePopup = FindItemByName(engine, "movePopup");
    QObject::connect(movePopup, SIGNAL(runFeature(int,int,int)),this, SLOT(modelMoveByNumber(int,int,int)));
    QObject::connect(movePopup, SIGNAL(closeMove()), this, SLOT(closeMove()));
    QObject::connect(movePopup, SIGNAL(openMove()), this, SLOT(openMove()));
	boundedBox = (QEntity*)FindItemByName(engine, "boundedBox");

    // model rotate components
    rotatePopup = FindItemByName(engine, "rotatePopup");
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


	ltso = FindItemByName(engine, "ltso");
	_optBackend.createSlicingOptControls();

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

	// manual support popup codes
	QObject::connect(manualSupportPopup, SIGNAL(clearSupports()), this, SLOT(clearSupports()));
	QObject::connect(manualSupportPopup, SIGNAL(generateAutoSupport()), this, SLOT(generateAutoSupport()));
	QObject::connect(manualSupportPopup, SIGNAL(supportEditEnabled(bool)), this, SLOT(supportEditEnabled(bool)));
	QObject::connect(manualSupportPopup, SIGNAL(supportApplyEdit()), this, SLOT(supportApplyEdit()));
	QObject::connect(manualSupportPopup, SIGNAL(supportCancelEdit()), this, SLOT(supportCancelEdit()));
	QObject::connect(manualSupportPopup, SIGNAL(regenerateRaft()), this, SLOT(regenerateRaft()));


	
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
    scene3d = dynamic_cast<QQuickItem*> (FindItemByName(engine, "scene3d"));

    QObject::connect(boxUpperTab,SIGNAL(runGroupFeature(int,QString, double, double, double, QVariant)),this,SLOT(runGroupFeature(int,QString, double, double, double, QVariant)));

    QObject::connect(this, SIGNAL(arrangeDone(std::vector<QVector3D>, std::vector<float>)), this, SLOT(applyArrangeResult(std::vector<QVector3D>, std::vector<float>)));

    leftTabViewMode = FindItemByName(engine, "ltvm");
    layerViewPopup = FindItemByName(engine, "layerViewPopup");
    layerViewSlider = FindItemByName(engine, "layerViewSlider");
    viewObjectButton = FindItemByName(engine, "viewObjectButton");
    QObject::connect(viewObjectButton, SIGNAL(onChanged(bool)), this, SLOT(viewObjectChanged(bool)));
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

	QObject::connect(mv, SIGNAL(cameraViewChanged()), this, SLOT(cameraViewChanged()));




}

GLModel* QmlManager::createModelFile(Mesh* target_mesh, QString fname) {
    openProgressPopUp();
    auto res = glmodels.try_emplace(modelIDCounter, mainWindow, models, target_mesh, fname, modelIDCounter);
    ++modelIDCounter;
    auto latestAdded = &(res.first->second);
    qDebug() << "created new model file";
    // model selection codes, connect handlers later when model selected
    qDebug() << "connected model selected signal";

    // set initial position

    latestAdded->setZToBed();

    emit latestAdded->_updateModelMesh();


	//add to raytracer
	latestAdded->setHitTestable(true);
    // 승환 100%
    setProgress(1);
	return latestAdded;
}
void QmlManager::openModelFile(QString fname){
    auto latest = createModelFile(new Mesh(), fname);

    // check for defects
    checkModelFile(latest);

    // do auto arrange
    if (glmodels.size() >= 2)
        openArrange();
    //runArrange();
    QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));
}

void QmlManager::checkModelFile(GLModel* target){
    ;

    //size_t holesCount = MeshRepair::identifyHoles(target->getMesh()).size();
    //if (holesCount!= 0){
    //    selectPart(target->ID);
    //    qmlManager->openYesNoPopUp(false, "Model has flaws.", "", "Do you want to fix the model?", 16, "", ftrRepair, 0);
    //}
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
	deleteOneModelFile(target);
}


void QmlManager::deleteOneModelFile(GLModel* target) {
	if (target)
	{
		//TODO: move these into glmodel destructor
		target->removePlane();
		disconnectHandlers(target);
		//    target->deleteLater();
		//    target->deleteLater();
		deletePartListItem(target->ID);
		//if selected, remove from selected list
		selectedModels.erase(target);
		//clear related supports
		_supportRaftManager.clear(target);
		glmodels.erase(target->ID);
	}
}

void QmlManager::deleteModelFileDone() {
    QMetaObject::invokeMethod(leftTabViewMode, "setEnable", Q_ARG(QVariant, false));
	QMetaObject::invokeMethod(qmlManager->boundedBox, "hideBox");
	updateModelInfo(0, 0, "0.0 X 0.0 X 0.0 mm", 0);

    // UI
	_widgetManager.setWidgetMode(WidgetMode::None);
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

    if (selectedModels.size() == 0) {
        deleteModelFileDone();
        return;
    }
	for (auto it = selectedModels.begin(); it != selectedModels.end();)
	{
		auto model = *it;
		++it;
		deleteOneModelFile(model);


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

	//	// auto orientation popup codes
	//QObject::disconnect(orientPopup, SIGNAL(runFeature(int)), glmodel->ft, SLOT(setTypeAndStart(int)));
	//// auto Repair popup codes
	//QObject::disconnect(repairPopup, SIGNAL(runFeature(int)), glmodel->ft, SLOT(setTypeAndStart(int)));


    // model layflat popup codes
	QObject::disconnect(layflatPopup, SIGNAL(generateLayFlat()), glmodel, SLOT(generateLayFlat()));
	//QObject::disconnect(layflatPopup, SIGNAL(openLayflat()), glmodel, SLOT(openLayflat()));

	// hollow shell popup codes
	//QObject::disconnect(hollowShellPopup, SIGNAL(hollowShell(double)), glmodel, SLOT(indentHollowShell(double)));


	// scale popup codes
	//QObject::disconnect(scalePopup, SIGNAL(runFeature(int,double,double,double)), glmodel->ft, SLOT(setTypeAndRun(int, double, double, double)));

	// label popup codes
	QObject::disconnect(labelPopup, SIGNAL(stateChangeLabelling()), glmodel, SLOT(stateChangeLabelling()));
	QObject::disconnect(labelPopup, SIGNAL(sendLabelUpdate(QString, QString, bool, int)), glmodel, SLOT(applyLabelInfo(QString, QString, bool, int)));
	//QObject::connect(labelPopup, SIGNAL(runFeature(int)),glmodel->ft, SLOT(setTypeAndStart(int)));
	
	// extension popup codes
	QObject::disconnect(extensionPopup, SIGNAL(generateExtensionFaces(double)), glmodel, SLOT(generateExtensionFaces(double)));

	// shelloffset popup codes
	QObject::disconnect(shelloffsetPopup, SIGNAL(shellOffset(double)), glmodel, SLOT(generateShellOffset(double)));


	

	
	// model layflat popup codes
	QObject::disconnect(layflatPopup, SIGNAL(openLayflat()), glmodel, SLOT(openLayflat()));
	QObject::disconnect(layflatPopup, SIGNAL(closeLayflat()), glmodel, SLOT(closeLayflat()));
	QObject::disconnect(glmodel, SIGNAL(layFlatSelect()), this, SLOT(layFlatSelect()));
	QObject::disconnect(glmodel, SIGNAL(layFlatUnSelect()), this, SLOT(layFlatUnSelect()));

	// model cut popup codes
	QObject::disconnect(cutPopup, SIGNAL(modelCut()), glmodel, SLOT(modelCut()));
	QObject::disconnect(cutPopup, SIGNAL(cutModeSelected(int)), glmodel, SLOT(cutModeSelected(int)));
	QObject::disconnect(cutPopup, SIGNAL(cutFillModeSelected(int)), glmodel, SLOT(cutFillModeSelected(int)));
	QObject::disconnect(cutPopup, SIGNAL(openCut()), glmodel, SLOT(openCut()));
	QObject::disconnect(cutPopup, SIGNAL(closeCut()), glmodel, SLOT(closeCut()));
	QObject::disconnect(cutPopup, SIGNAL(resultSliderValueChanged(double)), glmodel, SLOT(getSliderSignal(double)));

	//// hollow shell popup codes
	//QObject::disconnect(hollowShellPopup, SIGNAL(hollowShell(double)), glmodel, SLOT(hollowShell(double)));
	//QObject::disconnect(hollowShellPopup, SIGNAL(openHollowShell()), glmodel, SLOT(openHollowShell()));
	//QObject::disconnect(hollowShellPopup, SIGNAL(resultSliderValueChanged(double)), glmodel, SLOT(getSliderSignal(double)));


	// scale popup codes
	QObject::disconnect(scalePopup, SIGNAL(openScale()), glmodel, SLOT(openScale()));
	QObject::disconnect(scalePopup, SIGNAL(closeScale()), glmodel, SLOT(closeScale()));

	// label popup codes
	QObject::disconnect(labelPopup, SIGNAL(sendTextChanged(QString)), glmodel, SLOT(getTextChanged(QString)));
	QObject::disconnect(labelPopup, SIGNAL(openLabelling()), glmodel, SLOT(openLabelling()));
	QObject::disconnect(labelPopup, SIGNAL(closeLabelling()), glmodel, SLOT(closeLabelling()));
	QObject::disconnect(labelPopup, SIGNAL(generateText3DMesh()), glmodel, SLOT(generateText3DMesh()));
	QObject::disconnect(labelFontBox, SIGNAL(sendFontName(QString)), glmodel, SLOT(getFontNameChanged(QString)));
	QObject::disconnect(labelFontBoldBox, SIGNAL(sendFontBold(bool)), glmodel, SLOT(getFontBoldChanged(bool)));
	QObject::disconnect(labelFontSizeBox, SIGNAL(sendFontSize(int)), glmodel, SLOT(getFontSizeChanged(int)));

	// extension popup codes
	QObject::disconnect(extensionPopup, SIGNAL(openExtension()), glmodel, SLOT(openExtension()));
	QObject::disconnect(extensionPopup, SIGNAL(closeExtension()), glmodel, SLOT(closeExtension()));
	QObject::disconnect(glmodel, SIGNAL(extensionSelect()), this, SLOT(extensionSelect()));
	QObject::disconnect(glmodel, SIGNAL(extensionUnSelect()), this, SLOT(extensionUnSelect()));

	//manual supprt popup codes
	QObject::disconnect(glmodel, SIGNAL(manualSupportSelect()), this, SLOT(manualSupportSelect()));
	QObject::disconnect(glmodel, SIGNAL(manualSupportUnselect()), this, SLOT(manualSupportUnselect()));


	// shelloffset popup codes
	QObject::disconnect(shelloffsetPopup, SIGNAL(openShellOffset()), glmodel, SLOT(openShellOffset()));
	QObject::disconnect(shelloffsetPopup, SIGNAL(closeShellOffset()), glmodel, SLOT(closeShellOffset()));
	QObject::disconnect(shelloffsetPopup, SIGNAL(resultSliderValueChanged(double)), glmodel, SLOT(getSliderSignal(double)));

	


    // auto arrange popup codes
    //unused, signal from qml goes right into QmlManager.runArrange
    //QObject::disconnect(arrangePopup, SIGNAL(runFeature(int)), glmodel->ft, SLOT(setTypeAndStart(int)));
    //QObject::disconnect(glmodel->arsignal, SIGNAL(runArrange()), this, SLOT(runArrange()));

    // save button codes
    //QObject::disconnect(saveButton, SIGNAL(runFeature(int)), glmodel->ft, SLOT(setTypeAndRun(int)));

    // export button codes
    //QObject::disconnect(exportOKButton, SIGNAL(runGroupFeature(int, QString, double, double, double, QVariant)),this,SLOT(runGroupFeature(int,QString, double, double, double, QVariant))); //glmodel->ft, SLOT(setTypeAndRun(int, QVariant)));
    //QObject::disconnect(exportButton, SIGNAL(runFeature(int, QVariant)), glmodel->ft, SLOT(setTypeAndRun(int, QVariant)));



    QObject::disconnect(layerViewSlider, SIGNAL(sliderValueChanged(int)), glmodel, SLOT(getLayerViewSliderSignal(int)));
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

	//// auto Repair popup codes
	//QObject::connect(repairPopup, SIGNAL(runFeature(int)), glmodel->ft, SLOT(setTypeAndStart(int)));
	//// auto orientation popup codes
	//QObject::connect(orientPopup, SIGNAL(runFeature(int)), glmodel->ft, SLOT(setTypeAndStart(int)));


    QObject::connect(layflatPopup, SIGNAL(openLayflat()), glmodel, SLOT(openLayflat()));
    QObject::connect(layflatPopup, SIGNAL(closeLayflat()), glmodel, SLOT(closeLayflat()));
    QObject::connect(layflatPopup, SIGNAL(generateLayFlat()), glmodel, SLOT(generateLayFlat()));
    QObject::connect(glmodel,SIGNAL(layFlatSelect()),this,SLOT(layFlatSelect()));
    QObject::connect(glmodel,SIGNAL(layFlatUnSelect()),this,SLOT(layFlatUnSelect()));

    // model cut popup codes
    QObject::connect(cutPopup,SIGNAL(modelCut()),glmodel , SLOT(modelCut()));
    QObject::connect(cutPopup,SIGNAL(cutModeSelected(int)),glmodel,SLOT(cutModeSelected(int)));
    QObject::connect(cutPopup,SIGNAL(cutFillModeSelected(int)),glmodel,SLOT(cutFillModeSelected(int)));
    QObject::connect(cutPopup, SIGNAL(openCut()), glmodel, SLOT(openCut()));
    QObject::connect(cutPopup, SIGNAL(closeCut()), glmodel, SLOT(closeCut()));
    QObject::connect(cutPopup, SIGNAL(resultSliderValueChanged(double)), glmodel, SLOT(getSliderSignal(double)));

    /*// hollow shell popup codes
    QObject::connect(hollowShellPopup, SIGNAL(openHollowShell()), glmodel, SLOT(openHollowShell()));
    QObject::connect(hollowShellPopup, SIGNAL(closeHollowShell()), glmodel, SLOT(closeHollowShell()));
    QObject::connect(hollowShellPopup, SIGNAL(hollowShell(double)), glmodel, SLOT(indentHollowShell(double)));
    QObject::connect(hollowShellPopup, SIGNAL(resultSliderValueChanged(double)), glmodel, SLOT(getSliderSignal(double)));
    */

 
    // scale popup codes
    //QObject::connect(scalePopup, SIGNAL(runFeature(int,double,double,double)), glmodel->ft, SLOT(setTypeAndRun(int, double, double, double)));
    QObject::connect(scalePopup, SIGNAL(openScale()), glmodel, SLOT(openScale()));
    QObject::connect(scalePopup, SIGNAL(closeScale()), glmodel, SLOT(closeScale()));

    // label popup codes
    QObject::connect(labelPopup, SIGNAL(sendTextChanged(QString)),glmodel,SLOT(getTextChanged(QString)));
    QObject::connect(labelPopup, SIGNAL(openLabelling()),glmodel,SLOT(openLabelling()));
    QObject::connect(labelPopup, SIGNAL(closeLabelling()),glmodel,SLOT(closeLabelling()));
    QObject::connect(labelPopup, SIGNAL(stateChangeLabelling()), glmodel, SLOT(stateChangeLabelling()));
    QObject::connect(labelPopup, SIGNAL(sendLabelUpdate(QString, QString, bool, int)), glmodel, SLOT(applyLabelInfo(QString, QString, bool, int)));
    //QObject::connect(labelPopup, SIGNAL(runFeature(int)),glmodel->ft, SLOT(setTypeAndStart(int)));
    QObject::connect(labelPopup, SIGNAL(generateText3DMesh()), glmodel, SLOT(generateText3DMesh()));
    QObject::connect(labelFontBox, SIGNAL(sendFontName(QString)),glmodel, SLOT(getFontNameChanged(QString)));
    QObject::connect(labelFontBoldBox, SIGNAL(sendFontBold(bool)),glmodel, SLOT(getFontBoldChanged(bool)));
    QObject::connect(labelFontSizeBox, SIGNAL(sendFontSize(int)),glmodel, SLOT(getFontSizeChanged(int)));

    // extension popup codes
    QObject::connect(extensionPopup, SIGNAL(openExtension()), glmodel, SLOT(openExtension()));
    QObject::connect(extensionPopup, SIGNAL(closeExtension()), glmodel, SLOT(closeExtension()));
    QObject::connect(extensionPopup, SIGNAL(generateExtensionFaces(double)), glmodel, SLOT(generateExtensionFaces(double)));
    QObject::connect(glmodel,SIGNAL(extensionSelect()),this,SLOT(extensionSelect()));
    QObject::connect(glmodel,SIGNAL(extensionUnSelect()),this,SLOT(extensionUnSelect()));

	//manual supprt popup codes
	QObject::connect(glmodel, SIGNAL(manualSupportSelect()), this, SLOT(manualSupportSelect()));
	QObject::connect(glmodel, SIGNAL(manualSupportUnselect()), this, SLOT(manualSupportUnselect()));



    // shelloffset popup codes
    QObject::connect(shelloffsetPopup, SIGNAL(openShellOffset()), glmodel, SLOT(openShellOffset()));
    QObject::connect(shelloffsetPopup, SIGNAL(closeShellOffset()), glmodel, SLOT(closeShellOffset()));
    QObject::connect(shelloffsetPopup, SIGNAL(shellOffset(double)), glmodel, SLOT(generateShellOffset(double)));
    QObject::connect(shelloffsetPopup, SIGNAL(resultSliderValueChanged(double)), glmodel, SLOT(getSliderSignal(double)));


    // auto arrange popup codes
    //unused, signal from qml goes right into QmlManager.runArrange
    //QObject::connect(arrangePopup, SIGNAL(runFeature(int)), glmodel->ft, SLOT(setTypeAndStart(int)));
    //QObject::connect(glmodel->arsignal, SIGNAL(runArrange()), this, SLOT(runArrange()));

    // save button codes
    //QObject::connect(saveButton, SIGNAL(runFeature(int)), glmodel->ft, SLOT(setTypeAndRun(int)));

    // export button codes
    //QObject::connect(exportOKButton, SIGNAL(runGroupFeature(int, QString, double, double, double, QVariant)),this,SLOT(runGroupFeature(int,QString, double, double, double, QVariant)));
    //QObject::connect(exportButton, SIGNAL(runFeature(int, QVariant)), glmodel->ft, SLOT(setTypeAndRun(int, QVariant)));


    QObject::connect(layerViewSlider, SIGNAL(sliderValueChanged(int)), glmodel, SLOT(getLayerViewSliderSignal(int)));
}
void QmlManager::cleanselectedModel(int type){
    //selectedModels[0] = nullptr;
}

QVector3D QmlManager::getSelectedCenter()
{
	//get full bound
	return getSelectedBound().centre();
}
QVector3D QmlManager::selectedModelsLengths(){
	return getSelectedBound().lengths();
}


int QmlManager::getSelectedModelsSize() {
    return selectedModels.size();
}

float QmlManager::getBedXSize(){
    return scfg->bedX();
}

float QmlManager::getBedYSize(){
    return scfg->bedY();
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
    if (selectedModels.empty())
        return;

    openProgressPopUp();
    for(auto glm : selectedModels)
        glm->repairMesh();

}

void QmlManager::disableObjectPickers(){
    for (auto& pair : glmodels){
		auto glm = &pair.second;
		glm->setHitTestable(false);
    }
}

void QmlManager::enableObjectPickers(){
	for (auto& pair : glmodels) {
		auto glm = &pair.second;
		glm->setHitTestable(true);
    }
}

void QmlManager::setHandCursor(){
    QApplication::setOverrideCursor(QCursor(Qt::PointingHandCursor));
}
void QmlManager::setClosedHandCursor(){
    QApplication::setOverrideCursor(QCursor(Qt::ClosedHandCursor));
}

void QmlManager::setEraserCursor() {
	QApplication::setOverrideCursor(_cursorEraser);
}

void QmlManager::resetCursor(){
    QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));
}


Hix::Engine3D::Bounds3D QmlManager::getSelectedBound()const
{
	Hix::Engine3D::Bounds3D bound;
	for (auto& selected : selectedModels)
	{
		bound += selected->recursiveAabb();
	}
	return bound;
}



void QmlManager::sendUpdateModelInfo(){
    qDebug() << "send update model info";
    if (selectedModels.size() == 0 || this->viewMode == VIEW_MODE_LAYER){
        qDebug() << "sendUpdateModelInfo() - no selected model";

        slicingData->setProperty("visible", false);
		QMetaObject::invokeMethod(boundedBox, "hideBox"); // Bounded Box
        return;
    }

    qDebug() << "sendUpdateModelInfo() - selected model exists";

    slicingData->setProperty("visible", true);


	auto selectedSize = selectedModelsLengths();

    QString size;
    size.sprintf("%.1f X %.1f X %.1f mm", selectedSize.x(), selectedSize.y(), selectedSize.z());
    updateModelInfo(-1,-1,size,-1);
    QMetaObject::invokeMethod(scalePopup, "updateSizeInfo", Q_ARG(QVariant, selectedSize.x()), Q_ARG(QVariant, selectedSize.y()), Q_ARG(QVariant, selectedSize.z()));
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

    qmlManager->openProgressPopUp();
    QFuture<void> future = QtConcurrent::run(this, &QmlManager::runArrange_internal);
}

void QmlManager::runArrange_internal(){
    qDebug() << "run arrange glmodels size : " <<glmodels.size();

    for(auto& pair : glmodels)
    {
        const auto* const model = &pair.second;
        qDebug() << "before " <<model->transform().translation();
    }
    if (glmodels.size()>=2){
        std::vector<XYArrangement> arng_result_set;
        std::vector<const Mesh*> meshes_to_arrange;
		std::vector<Qt3DCore::QTransform> m_transform_set;

   //     for(auto& pair : glmodels)
   //     {
			//const auto* const  model = &pair.second;
   //         meshes_to_arrange.push_back(model->getMesh());
   //         m_transform_set.push_back(model->transform());
   //     }

   //     autoarrange* ar;
   //     arng_result_set = ar->arngMeshes(meshes_to_arrange, m_transform_set);
   //     std::vector<QVector3D> translations;
   //     std::vector<float> rotations;
   //     for (size_t i=0; i<arng_result_set.size(); i++){
   //         XYArrangement arng_result = arng_result_set[i];
   //         QVector3D trans_vec = QVector3D(arng_result.first.X/100, arng_result.first.Y/100, 0);
   //         translations.push_back(trans_vec);
   //         rotations.push_back(arng_result.second);
   //     }
   //     emit arrangeDone(translations, rotations);

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
        model->moveModel(translations[index]);
        ++index;
    }

    qmlManager->setProgress(1);

    qmlManager->setProgressText("Done");
    qmlManager->openResultPopUp("","Arrangement done","");
    if(selectedModels.size() > 0){
        sendUpdateModelInfo();
    }
}

GLModel* QmlManager::findGLModelByName(QString filename){

    for(auto& pair : glmodels)
    {
        auto model = &pair.second;
        qDebug() << "finding " << filename << model->filename();
        if (model->filename() == filename){
            return model;
        }
    }
    return NULL;
}


void QmlManager::backgroundClicked(){
    qDebug() << "background clicked";
	if (deselectAllowed())
	{
		unselectAll();
	}
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
    qDebug() << "multipleModelSelected():" << target << target;
    qDebug() << "-- printing selectedModels --" <<selectedModels.size();
    int i = 0;
    for (std::vector<GLModel*>::iterator it_ = selectedModels.begin(); it_ != selectedModels.end() ; ++it_){
        qDebug() << "model" << i << ":" <<*it_ ;
        i++;
    }
    qDebug() <<"------------------------";
    */

    // if target is already in selectedModels
	for (auto it = selectedModels.begin(); it != selectedModels.end();) {
        /* when selectedModels is an empty std::vector */
        if ((*it)->ID == ID){
            // do unselect model

            it = selectedModels.erase(it);
			if (target->isPrintable())
			{
				target->changeColor(Hix::Render::Colors::Default);
			}
            (*it)->inactivateFeatures();
            QMetaObject::invokeMethod(partList, "unselectPartByModel", Q_ARG(QVariant, target->ID));
            QMetaObject::invokeMethod(yesno_popup, "deletePartListItem", Q_ARG(QVariant, target->ID));

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
				case ftrMove:
					_widgetManager.setWidgetMode(WidgetMode::None);
					QMetaObject::invokeMethod(movePopup, "offApplyFinishButton");
					break;
                case ftrRotate:
					_widgetManager.setWidgetMode(WidgetMode::None);
                    QMetaObject::invokeMethod(rotatePopup,"offApplyFinishButton");
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
		else
		{
			++it;
		}

    }

	selectedModels.insert(target);
	_lastSelected = target;
    connectHandlers(target);
    target->changeColor(Hix::Render::Colors::Selected);
    qDebug() << "multipleModelSelected invoke";
    QMetaObject::invokeMethod(partList, "selectPartByModel", Q_ARG(QVariant, target->ID));
    QMetaObject::invokeMethod(yesno_popup, "addPart", Q_ARG(QVariant, target->getFileName(target->filename().toStdString().c_str())), Q_ARG(QVariant, target->ID));
    qDebug() << "[multi model selected] b box center"; //<< xmid << " " << ymid << " " << zmid ;

    sendUpdateModelInfo();
    if (groupFunctionState == "active"){
        switch (groupFunctionIndex){
        //case 2:
        //    QMetaObject::invokeMethod(savePopup, "onApplyFinishButton");
        //    break;
        case 5:
            QMetaObject::invokeMethod(rotatePopup,"onApplyFinishButton");
			_widgetManager.setWidgetMode(WidgetMode::Rotate);
			break;
        case 4:
            QMetaObject::invokeMethod(movePopup,"onApplyFinishButton");
			_widgetManager.setWidgetMode(WidgetMode::Move);
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

    selectedModels.erase(_lastSelected);

    qDebug() << "leave:" << _lastSelected;

    /* remove all elements from the list */
    for (auto it = selectedModels.begin() ; it != selectedModels.end() ; ++it) {
        /* it is simillar to selectModel() */
		if ((*it)->isPrintable())
		{
			(*it)->changeColor(Hix::Render::Colors::Default);
		}
        (*it)->inactivateFeatures();
        QMetaObject::invokeMethod(partList, "unselectPartByModel", Q_ARG(QVariant, (*it)->ID));
        QMetaObject::invokeMethod(yesno_popup, "deletePartListItem", Q_ARG(QVariant, (*it)->ID));

        slicingData->setProperty("visible", false);

        disconnectHandlers((*it));
        QMetaObject::invokeMethod(qmlManager->mttab, "hideTab");
		QMetaObject::invokeMethod(boundedBox, "hideBox"); // Bounded Box
        if(groupFunctionState == "active") {
            switch (groupFunctionIndex) {
            //case 2:
            //    QMetaObject::invokeMethod(savePopup, "offApplyFinishButton");
            //    break;
            case ftrMove:
				_widgetManager.setWidgetMode(WidgetMode::None);
				QMetaObject::invokeMethod(movePopup, "offApplyFinishButton");
                break;
            case ftrRotate:
				_widgetManager.setWidgetMode(WidgetMode::None);
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
    selectedModels.insert(_lastSelected);
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
	bool modelAlreadySelected = false;
	bool deselectNeeded = !selectedModels.empty();
    if (deselectNeeded){ // remove selected models if multiple selected previously
        for (auto it=selectedModels.begin(); it!=selectedModels.end(); ++it){
            // unselect Model
            if (*it == target) {
				modelAlreadySelected = true;
            }
			if((*it)->isPrintable())
				(*it)->changeColor(Hix::Render::Colors::Default);
            (*it)->inactivateFeatures();
            QMetaObject::invokeMethod(partList, "unselectPartByModel", Q_ARG(QVariant, (*it)->ID));
            QMetaObject::invokeMethod(yesno_popup, "deletePartListItem", Q_ARG(QVariant, (*it)->ID));
            disconnectHandlers((*it));  //check
        }
        selectedModels.clear();
		// set slicing info box property visible true if slicing info exists
		slicingData->setProperty("visible", false);
		QMetaObject::invokeMethod(qmlManager->mttab, "hideTab"); // off MeshTransformer Tab
		QMetaObject::invokeMethod(boundedBox, "hideBox"); // Bounded Box
		if (groupFunctionState == "active") {
            //qDebug() << "@@@@ selected @@@@" << groupFunctionIndex;
			switch (groupFunctionIndex) {
				//case 2:
				//    QMetaObject::invokeMethod(savePopup, "offApplyFinishButton");
				//    break;
			case 5:
				_widgetManager.setWidgetMode(WidgetMode::None);
				QMetaObject::invokeMethod(rotatePopup, "offApplyFinishButton");
				break;
			case 4:
				_widgetManager.setWidgetMode(WidgetMode::None);
				QMetaObject::invokeMethod(movePopup, "offApplyFinishButton");
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
	//if part is not selected
	if (!modelAlreadySelected)
	{
		selectedModels.insert(target);
		_lastSelected = target;
		connectHandlers(target);

		if( target->isPrintable())
			target->changeColor(Hix::Render::Colors::Selected);
		qDebug() << "modelSelected invoke";
		QMetaObject::invokeMethod(partList, "selectPartByModel", Q_ARG(QVariant, target->ID));
		QMetaObject::invokeMethod(yesno_popup, "addPart", Q_ARG(QVariant, 
			target->getFileName(target->filename().toStdString().c_str())), Q_ARG(QVariant, ID));
		qDebug() << "changing model" << target->ID;
		qDebug() << "[model selected] b box center"; //<< xmid << " " << ymid << " " << zmid ;

		QMetaObject::invokeMethod(layerViewSlider, "setThickness", Q_ARG(QVariant, (scfg->layer_height)));
		QMetaObject::invokeMethod(layerViewSlider, "setHeight", Q_ARG(QVariant,
			(target->getMesh()->z_max() - target->getMesh()->z_min() + scfg->raft_thickness+ scfg->support_base_height)));
		sendUpdateModelInfo();
		qDebug() << "scale value   " << target->getMesh()->x_max() - target->getMesh()->x_min();
		if (groupFunctionState == "active") {
            qDebug() << "@@@@ selected2 @@@@" << groupFunctionIndex;
			switch (groupFunctionIndex) {
				//case 2:
				//    QMetaObject::invokeMethod(savePopup, "onApplyFinishButton");
				//    break;
			case 5:
				QMetaObject::invokeMethod(rotatePopup, "onApplyFinishButton");
				_widgetManager.setWidgetMode(WidgetMode::Rotate);
				break;
			case 4:
				QMetaObject::invokeMethod(movePopup, "onApplyFinishButton");
				_widgetManager.setWidgetMode(WidgetMode::Move);
				break;
			case 6:
				QMetaObject::invokeMethod(layflatPopup, "onApplyFinishButton");
				break;
			case 8:
				QMetaObject::invokeMethod(orientPopup, "onApplyFinishButton");
				break;
			case 10:
				QMetaObject::invokeMethod(repairPopup, "onApplyFinishButton");
				break;
			}
		}
	}


    QMetaObject::invokeMethod(leftTabViewMode, "setEnable", Q_ARG(QVariant, !selectedModels.empty()));
    sendUpdateModelInfo();
}

const std::unordered_set<GLModel*>& QmlManager::getSelectedModels()
{
	return selectedModels;
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
	for(auto itr = selectedModels.begin(); itr != selectedModels.end();)
	{
		auto model = *itr;
		++itr;
		unselectPart(model->ID);
    }
	_widgetManager.setWidgetMode(WidgetMode::None);
    QMetaObject::invokeMethod(qmlManager->mttab, "hideTab");
    QMetaObject::invokeMethod(boxUpperTab, "all_off");
	QMetaObject::invokeMethod(boundedBox, "hideBox");

}

bool QmlManager::isSelected(){
    if(selectedModels.empty())
        return false;
    else
        return true;
}
bool QmlManager::isSelected(int ID)
{
	auto target = getModelByID(ID);
	return isSelected(target);
}
bool QmlManager::isSelected(GLModel* model)
{
	return selectedModels.end() != selectedModels.find(model);
}

void QmlManager::showCubeWidgets(GLModel* model)
{
}

void QmlManager::addSupport(GLModel* model, QVector3D position)
{
	
}

void QmlManager::modelVisible(int ID, bool isVisible){
	auto target = getModelByID(ID);
	if (target)
	{
    	target->setEnabled(isVisible);
	}    
}

void QmlManager::doDelete(){
    if(selectedModels.empty())
        return;
	for (auto it = selectedModels.begin(); it != selectedModels.end();)
	{
		auto model = *it;
		it = selectedModels.erase(it);
		deleteOneModelFile(model);
	}
	deleteModelFileDone();
}

void QmlManager::doDeletebyID(int ID){
    deleteModelFile(ID);
}

RayCastController& QmlManager::getRayCaster()
{
	return _rayCastController;
}

void QmlManager::totalMoveDone(){
	for (auto each : selectedModels)
	{
		each->moveDone();
    }
    sendUpdateModelInfo();
	_widgetManager.updatePosition();
}

void QmlManager::totalRotateDone(){
	for (auto each : selectedModels)
	{
		each->rotateDone();
	}
    sendUpdateModelInfo();
}

void QmlManager::totalScaleDone() {
	for (auto each : selectedModels)
	{
		each->scaleDone();
	}
	sendUpdateModelInfo();
}

void QmlManager::modelMoveWithAxis(QVector3D axis, double distance) { // for QML Signal -> float is not working in qml signal parameter
	auto displacement = distance * axis;
	modelMove(displacement);
}

void QmlManager::modelMove(QVector3D displacement)
{
	QVector3D bndCheckedDisp;
	const auto& printBound = scfg->bedBound();
	for (auto selectedModel : selectedModels) {
		bndCheckedDisp = printBound.displaceWithin(selectedModel->recursiveAabb(), displacement);
		selectedModel->moveModel(bndCheckedDisp);
	}
}
void QmlManager::modelRotateWithAxis(const QVector3D& axis, double angle)
{
	auto axis4D = axis.toVector4D();
	for (auto selectedModel : selectedModels) {
		auto rotation = QQuaternion::fromAxisAndAngle(axis, angle);
		selectedModel->rotateModel(rotation);
		selectedModel->updatePrintable();

	}


}

QVector3D QmlManager::cameraViewVector()
{
	return _camera->position() - systemTransform->translation();
}

TaskManager& QmlManager::taskManager()
{
	return _taskManager;
}

Hix::Support::SupportRaftManager& QmlManager::supportRaftManager()
{
	return _supportRaftManager;
}



void QmlManager::modelMoveByNumber(int axis, int X, int Y){
    if (selectedModels.empty())
        return;

	QVector3D displacement(X, Y, 0);
	for (auto selectedModel : selectedModels) {
		selectedModel->moveModel(displacement);
		selectedModel->moveDone();
    }

}
void QmlManager::modelRotateByNumber(int mode,  int X, int Y, int Z){
    if (selectedModels.empty())
        return;

	for (auto selectedModel : selectedModels) {
		auto rotation = QQuaternion::fromEulerAngles(X,Y,Z);
		selectedModel->rotateModel(rotation);
		selectedModel->rotateDone();
    }
}
void QmlManager::resetLayflat(){
    QMetaObject::invokeMethod(layflatPopup,"onApplyFinishButton");
}

void QmlManager::save() {
    STLexporter* ste = new STLexporter();

    qDebug() << "file save called";
    if (selectedModels.empty()) {
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

void QmlManager::cameraViewChanged()
{
	_widgetManager.updatePosition();
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
	if (state == "active")
	{
		clearSupports();
	}
    qDebug()<< "runGroupFeature | type:"<<ftrType<<"| state:" <<state << selectedModels.size();
    switch(ftrType){
    /*
    case ftrSave:
    {
        if (state == "active") {
            if (selectedModels.empty()){
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
			_widgetManager.setWidgetMode(WidgetMode::None);

        }else if(state == "active"){
			_widgetManager.setWidgetMode(WidgetMode::Rotate);
        }
        break;
    }
    case ftrMove:  //move
    {
        if (state == "inactive"){
			_widgetManager.setWidgetMode(WidgetMode::None);
		}else if(state == "active"){
			_widgetManager.setWidgetMode(WidgetMode::Move);
        }
        break;
    }
    case ftrLayFlat:
	{
        break;
    }
    case ftrOrient:  //orient
    {
        if (state == "active"){
			for (auto selectedModel : selectedModels) {
				if (selectedModel->updateLock)
					return;
				selectedModel->updateLock = true;
				qmlManager->openProgressPopUp();
				qDebug() << "tweak start";
				rotateResult* rotateres = autoorientation::Tweak(selectedModel->getMesh(), true, 45, &selectedModel->appropriately_rotated);
				qDebug() << "got rotate result";
				if (rotateres == NULL)
					break;
				//else
					//selectedModel->rotateModelMesh(rotateres->R);
			}
        }
        break;
    }
    case ftrRepair:  //repair
    {
        if (state == "active"){

			for (auto selectedModel : selectedModels) {
				selectedModel->repairMesh();

			}
        }
        break;
    }
    case ftrExtend:
        break;
    case ftrScale:
        qDebug() << "run feature scale" << selectedModels.size();
        if (state == "active"){
			float scaleX = arg1;
			float scaleY = arg2;
			float scaleZ = arg3;
			for (auto selectedModel : selectedModels) {
				selectedModel->scaleModel(QVector3D(scaleX, scaleY, scaleZ));
				selectedModel->scaleDone();
			}
            sendUpdateModelInfo();
        } else {

        }
        break;
    case ftrExport:
        // save to temporary folder
        qDebug() << "file export called";
		auto exportTaskFlow = exportSelectedAsync(getExportPath(), false);
		_taskManager.enqueTask(exportTaskFlow);
		break;
    }
}

void QmlManager::unDo(){

}

void QmlManager::reDo(){

}

void QmlManager::copyModel(){
    copyMeshes.clear();
    copyMeshNames.clear();

    qDebug() << "copying current selected Models";
    for (GLModel* model : selectedModels){
        if (model == nullptr)
            continue;
        Mesh* copied = new Mesh( *model->getMesh());

        copyMeshes.push_back(copied);
        copyMeshNames.push_back(model->filename());
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

void QmlManager::deletePartListItem(int ID){
    QMetaObject::invokeMethod(partList, "deletePartListItem", Q_ARG(QVariant, ID));
    QMetaObject::invokeMethod(yesno_popup, "deletePartListItem", Q_ARG(QVariant, ID));
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


void QmlManager::viewLayerChanged(bool checked){
    qInfo() << "viewLayerChanged" << checked;
    qDebug() << "selected Num = " << selectedModels.size();
    if( checked ) {
		//since slice generated depends on current selection AND state of each selected meshes, just slice new one each time for now.
		qmlManager->openYesNoPopUp(false, "The model should be sliced for layer view.", "", "Would you like to continue?", 16, "", ftrLayerViewMode, 0);

    }
}

void QmlManager::layerInfillButtonChanged(bool checked){
    qInfo() << "layerInfillButtonChanged" << checked;
    if( checked ) {
        layerViewFlags |= LAYER_INFILL;
    } else {
        layerViewFlags &= ~LAYER_INFILL;
    }
	for (auto each : selectedModels)
	{
		emit each->_updateModelMesh();
	}
  
}

void QmlManager::layerSupportersButtonChanged(bool checked){
    qInfo() << "layerSupportersButtonChanged" << checked;
    if( checked ) {
        layerViewFlags |= LAYER_SUPPORTERS;
    } else {
        layerViewFlags &= ~LAYER_SUPPORTERS;
    }

	for (auto each : selectedModels)
	{
		emit each->_updateModelMesh();
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

	for (auto each : selectedModels)
	{
		emit each->_updateModelMesh();
	}
}

void QmlManager::setViewMode(int viewMode) {
    qInfo() << "setViewMode" << viewMode;
    qDebug() << "current mode" << this->viewMode;

    if( this->viewMode != viewMode ) {
		QMetaObject::invokeMethod(boxUpperTab, "all_off");
        //if (viewMode == 0) viewObjectButton->setProperty("checked", true);
        if (viewMode == 2) viewLayerButton->setProperty("checked", true);

        this->viewMode = viewMode;
		layerViewPopup->setProperty("visible", this->viewMode == VIEW_MODE_LAYER);
		layerViewSlider->setProperty("visible", this->viewMode == VIEW_MODE_LAYER);
		bool sliceNeeded = false;

		switch (viewMode) {
		case VIEW_MODE_OBJECT:
			QMetaObject::invokeMethod(yesno_popup, "closePopUp");
			QMetaObject::invokeMethod(leftTabViewMode, "setObjectView");
			break;

		case VIEW_MODE_LAYER:
			sliceNeeded = true;
			break;
		}
		if (sliceNeeded)
		{
			auto exportTaskFlow = exportSelectedAsync(QDir::tempPath(), true);
			auto f = std::bind(&QmlManager::setModelViewMode, this, viewMode);
			_taskManager.enqueTask(exportTaskFlow);
			_taskManager.enqueUITask(f);
		}
		else
		{
			setModelViewMode(viewMode);
		}
    }
}
QString QmlManager::getExportPath()
{
	QString fileName;
	fileName = QFileDialog::getSaveFileName(nullptr, tr("Export sliced file"), "");
	return fileName;
}
Hix::Tasking::GenericTask* QmlManager::exportSelectedAsync(QString exportPath, bool isTemp)
{
	if (exportPath.isEmpty())
		return nullptr;
	if (selectedModels.empty())
		return nullptr;
	auto task = new GenericTask();
	task->getFlow().emplace([this, exportPath, isTemp](tf::Subflow& subflow) {


		STLexporter* ste = new STLexporter();

		qmlManager->openProgressPopUp();


		
		// need to generate support, raft
		std::vector<std::reference_wrapper<const GLModel>> constSelectedModels;
		constSelectedModels.reserve(selectedModels.size());
		std::transform(std::begin(selectedModels), std::end(selectedModels), std::back_inserter(constSelectedModels),
			[](GLModel* ptr)-> std::reference_wrapper<const GLModel> {
				return std::cref(*ptr);
			});
		//constSelectedModels.reserve(selectedModels.size());
		//for (auto each : selectedModels)
		//{
		//	constSelectedModels.emplace_back(each);
		//}
		auto selectedBound = getSelectedBound();
		auto result = SlicingEngine::sliceModels(isTemp, subflow, selectedBound.zMax(), constSelectedModels, _supportRaftManager, exportPath);
		QMetaObject::invokeMethod(layerViewSlider, "setThickness", Q_ARG(QVariant, (scfg->layer_height)));
		QMetaObject::invokeMethod(layerViewSlider, "setLayerCount", Q_ARG(QVariant, (result.layerCount -1))); //0 based index
	});
	return task;

}
void QmlManager::setModelViewMode(int mode)
{
	qDebug() << "setModelViewMode called";
	for (auto each : selectedModels)
	{
		each->changeViewMode(viewMode);
		each->updateModelMesh();

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
	QMetaObject::invokeMethod(partList, "unselectPartByModel", Q_ARG(QVariant, target->ID));

	for (auto each : selectedModels)
	{
		each->changeViewMode(VIEW_MODE_OBJECT);
	}
	selectedModels.erase(target);
	if (target->isPrintable())
	{
		target->changeColor(Hix::Render::Colors::Default);
	}
    target->inactivateFeatures();
    disconnectHandlers(target);
    if (groupFunctionState == "active"){
        switch (groupFunctionIndex){
            case 5:
            case 4:
				_widgetManager.setWidgetMode(WidgetMode::None);
				break;
        }
    }

    //selectedModels.clear();
    QMetaObject::invokeMethod(leftTabViewMode, "setEnable", Q_ARG(QVariant, false));
    //QMetaObject::invokeMethod(boundedBox, "hideBox"); // Bounded Box
    sendUpdateModelInfo();
}


QVector2D QmlManager::world2Screen(QVector3D target) {
	QVariant value;
	qRegisterMetaType<QVariant>("QVariant");
	QMetaObject::invokeMethod(mttab, "world2Screen", Qt::DirectConnection, Q_RETURN_ARG(QVariant, value),
		Q_ARG(QVariant, target));
	QVector2D result = qvariant_cast<QVector2D>(value);
	return result;
}


void QmlManager::generateAutoSupport()
{
	for (auto selectedModel : selectedModels)
	{
		if (scfg->support_type != SlicingConfiguration::SupportType::None)
		{
			selectedModel->setZToBed();
			selectedModel->moveModel(QVector3D(0, 0, Hix::Support::SupportRaftManager::supportRaftMinLength()));
		}
		_supportRaftManager.autoGenSuppRaft(*selectedModel, scfg->support_type, scfg->raft_type);
	}
}


void QmlManager::supportEditEnabled(bool enabled)
{
	if (enabled)
	{
		_supportRaftManager.setSupportEditMode(Hix::Support::EditMode::Manual);
		_currentActiveFeature = ftrManualSupport;
		qmlManager->openResultPopUp("Click a model surface to add support.", "", "Click an existing support to remove it.");
	}
	else
	{
		_supportRaftManager.setSupportEditMode(Hix::Support::EditMode::None);
		_currentActiveFeature = 0;
	}

}
void QmlManager::clearSupports()
{
	std::unordered_set<const GLModel*> constSelection;
	for (auto selectedModel : selectedModels)
	{
		selectedModel->setZToBed();
		constSelection.emplace(selectedModel);
	}
	_supportRaftManager.clear(constSelection);

}


void QmlManager::supportApplyEdit()
{
	_supportRaftManager.applyEdits();
}


void QmlManager::supportCancelEdit()
{
	_supportRaftManager.cancelEdits();
}

bool QmlManager::deselectAllowed()
{
	return _currentActiveFeature != ftrManualSupport;
}

void QmlManager::regenerateRaft()
{
	_supportRaftManager.generateRaft();
}
