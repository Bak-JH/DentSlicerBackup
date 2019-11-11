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
#include "DentEngine/src/MeshIterators.h"
#include "feature/cut/modelcut.h"
#include "feature/label/Labelling.h"
#include "feature/layFlat.h"
#include "feature/repair/meshrepair.h"
#include "feature/layerview/layerview.h"
#include "feature/SupportFeature.h"

#include <functional>
using namespace Hix::Input;
using namespace Hix::UI;
using namespace Hix::Render;
using namespace Hix::Tasking;
using namespace Hix::Features;
QmlManager::QmlManager(QObject *parent) : QObject(parent), _optBackend(this, scfg)
  ,layerViewFlags(LAYER_INFILL | LAYER_SUPPORTERS | LAYER_RAFT), modelIDCounter(-1), _cursorEraser(QPixmap(":/Resource/cursor_eraser.png"))
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

	total = dynamic_cast<QEntity*> (FindItemByName(engine, "total"));
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
	QObject::connect(manualSupportPopup, SIGNAL(openSupport()), this, SLOT(openSupport()));
	QObject::connect(manualSupportPopup, SIGNAL(closeSupport()), this, SLOT(closeSupport()));

	
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


	QObject::connect(layflatPopup, SIGNAL(openLayflat()), this, SLOT(openLayFlat()));
	QObject::connect(extensionPopup, SIGNAL(openExtension()), this, SLOT(openExtension()));
	QObject::connect(layflatPopup, SIGNAL(closeLayflat()), this, SLOT(closeLayFlat()));
	QObject::connect(extensionPopup, SIGNAL(closeExtension()), this, SLOT(closeExtension()));

	QObject::connect(layflatPopup, SIGNAL(generateLayFlat()), this, SLOT(generateLayFlat()));



	// label popup codes
	QObject::connect(labelPopup, SIGNAL(openLabelling()), this, SLOT(openLabelling()));
	QObject::connect(labelPopup, SIGNAL(closeLabelling()), this, SLOT(closeLabelling()));
	QObject::connect(labelPopup, SIGNAL(sendTextChanged(QString)), this, SLOT(setLabelText(QString)));
	QObject::connect(labelPopup, SIGNAL(stateChangeLabelling()), this, SLOT(stateChangeLabelling()));
	//QObject::connect(labelPopup, SIGNAL(runFeature(int)),glmodel->ft, SLOT(setTypeAndStart(int)));
	QObject::connect(labelPopup, SIGNAL(generateLabelMesh()), this, SLOT(generateLabelMesh()));
	QObject::connect(labelFontBox, SIGNAL(sendFontName(QString)), this, SLOT(setLabelFontName(QString)));
	QObject::connect(labelFontBoldBox, SIGNAL(sendFontBold(bool)), this, SLOT(setLabelFontBold(bool)));
	QObject::connect(labelFontSizeBox, SIGNAL(sendFontSize(int)), this, SLOT(setLabelFontSize(int)));

	// extension popup codes
	QObject::connect(extensionPopup, SIGNAL(generateExtensionFaces(double)), this, SLOT(generateExtensionFaces(double)));
	// model cut popup codes
	QObject::connect(cutPopup, SIGNAL(modelCut()), this, SLOT(modelCut()));
	QObject::connect(cutPopup, SIGNAL(cutModeSelected(int)), this, SLOT(cutModeSelected(int)));
	QObject::connect(cutPopup, SIGNAL(cutFillModeSelected(int)), this, SLOT(cutFillModeSelected(int)));
	QObject::connect(cutPopup, SIGNAL(openCut()), this, SLOT(openCut()));
	QObject::connect(cutPopup, SIGNAL(closeCut()), this, SLOT(closeCut()));
	QObject::connect(cutPopup, SIGNAL(resultSliderValueChanged(double)), this, SLOT(getSliderSignal(double)));

	QObject::connect(repairPopup, SIGNAL(modelRepair()), this, SLOT(modelRepair()));



	// shelloffset popup codes
	QObject::connect(shelloffsetPopup, SIGNAL(openShellOffset()), this, SLOT(openShellOffset()));
	QObject::connect(shelloffsetPopup, SIGNAL(closeShellOffset()), this, SLOT(closeShellOffset()));
	QObject::connect(shelloffsetPopup, SIGNAL(shellOffset(double)), this, SLOT(generateShellOffset(double)));
	QObject::connect(shelloffsetPopup, SIGNAL(resultSliderValueChanged(double)), this, SLOT(getSliderSignal(double)));

	QObject::connect(layerViewSlider, SIGNAL(sliderValueChanged(int)), this, SLOT(getCrossSectionSignal(int)));


}

GLModel* QmlManager::listModel(GLModel* model)
{
	Qt3DCore::QTransform toRoot;
	toRoot.setMatrix(model->toRootMatrix());
	auto res = glmodels.try_emplace(modelIDCounter, models, model->getMeshModd(), model->modelName(), modelIDCounter, &toRoot);
	--modelIDCounter;
	auto latestAdded = &(res.first->second);
	// set initial position
	//add to raytracer
	latestAdded->setHitTestable(true);
	qmlManager->addPart(latestAdded->modelName(), latestAdded->ID);
	//steal children, mixing RAII with QT is difficult
	for (auto& childNode : model->childNodes())
	{
		auto childModel = dynamic_cast<GLModel*>(childNode);
		if (childModel)
		{
			childModel->setParent(latestAdded);
		}
	}
	model->setMesh(nullptr);
	delete model;
	return latestAdded;
}


GLModel* QmlManager::createAndListModel(Hix::Engine3D::Mesh* mesh, QString fname, const Qt3DCore::QTransform* transform) {
	auto res = glmodels.try_emplace(modelIDCounter, models, mesh, fname, modelIDCounter, transform);
    --modelIDCounter;
    auto latestAdded = &(res.first->second);
    // set initial position
	//add to raytracer
	latestAdded->setHitTestable(true);
	qmlManager->addPart(latestAdded->modelName(), latestAdded->ID);
	return latestAdded;
}
void QmlManager::openModelFile(QString fname){
	openProgressPopUp();

	auto mesh = new Mesh();
	if (fname != "" && (fname.contains(".stl") || fname.contains(".STL"))) {
		FileLoader::loadMeshSTL(mesh, fname.toLocal8Bit().constData());
	}
	else if (fname != "" && (fname.contains(".obj") || fname.contains(".OBJ"))) {
		FileLoader::loadMeshOBJ(mesh, fname.toLocal8Bit().constData());
	}
	fname = filenameToModelName(fname.toStdString());
	setProgress(0.3);
	mesh->centerMesh();
	auto latest = createAndListModel(mesh, fname, nullptr);
	latest->setZToBed();
	setProgress(0.6);

	//repair mode
	if (Hix::Features::isRepairNeeded(mesh))
	{
		qmlManager->setProgressText("Repairing mesh.");
		std::unordered_set<GLModel*> repairModels;
		repairModels.insert(latest);
		_currentFeature.reset(new MeshRepair(repairModels));
		_currentFeature.reset();
	}
	setProgress(1.0);
    // do auto arrange
    if (glmodels.size() >= 2)
        openArrange();
    //runArrange();
    QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));
}

void QmlManager::modelRepair()
{
	_currentFeature.reset(new MeshRepair(selectedModels));
	//if cut is finished;
	_currentFeature.reset();

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
		//    target->deleteLater();
		//    target->deleteLater();
		deletePartListItem(target->ID);
		//if selected, remove from selected list
		_supportRaftManager.clear(*target);
		selectedModels.erase(target);
		//clear related supports
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
    openProgressPopUp();
	qmlManager->setProgressText("Repairing mesh.");
	qmlManager->setProgress(0.1);
	_currentFeature.reset(new MeshRepair(selectedModels));
	_currentFeature.reset();
	qmlManager->setProgress(1.0);
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

GLModel* QmlManager::findGLModelByName(QString modelName){

    for(auto& pair : glmodels)
    {
        auto model = &pair.second;
        qDebug() << "finding " << modelName << model->modelName();
        if (model->modelName() == modelName){
            return model;
        }
    }
    return NULL;
}


void QmlManager::backgroundClicked(){
    qDebug() << "background clicked";
	if (!isFeatureActive())
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
            QMetaObject::invokeMethod(partList, "unselectPartByModel", Q_ARG(QVariant, target->ID));
            // set slicing info box property visible true if slicing info exists
            //slicingData->setProperty("visible", false);
            sendUpdateModelInfo();

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
    target->changeColor(Hix::Render::Colors::Selected);
    qDebug() << "multipleModelSelected invoke";
    QMetaObject::invokeMethod(partList, "selectPartByModel", Q_ARG(QVariant, target->ID));
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
        }
    }

    if (selectedModels.size() >= 2)
        groupFunctionState = "active";
    return true;
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
            QMetaObject::invokeMethod(partList, "unselectPartByModel", Q_ARG(QVariant, (*it)->ID));
            QMetaObject::invokeMethod(yesno_popup, "deletePartListItem", Q_ARG(QVariant, (*it)->ID));
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

		if( target->isPrintable())
			target->changeColor(Hix::Render::Colors::Selected);
		qDebug() << "modelSelected invoke";
		QMetaObject::invokeMethod(partList, "selectPartByModel", Q_ARG(QVariant, target->ID));
		qDebug() << "changing model" << target->ID;
		qDebug() << "[model selected] b box center"; //<< xmid << " " << ymid << " " << zmid ;

		//sendUpdateModelInfo();
		//qDebug() << "scale value   " << target->getMesh()->x_max() - target->getMesh()->x_min();
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

Hix::Features::Feature* QmlManager::currentFeature()const
{
	return _currentFeature.get();
}

//void QmlManager::faceSelectionEnable()
//{
//	for (auto selectedModel : selectedModels)
//	{
//		selectedModel->setMaterialMode(Hix::Render::ShaderMode::PerPrimitiveColor);
//		selectedModel->updateMesh(true);
//	}
//}
//
//void QmlManager::faceSelectionDisable()
//{
//	for (auto selectedModel : selectedModels)
//	{
//		selectedModel->setMaterialMode(Hix::Render::ShaderMode::SingleColor);
//		selectedModel->updateMesh(true);
//		selectedModel->rotateDone();
//		selectedModel->unselectMeshFaces();
//	}
//}

void QmlManager::openLayFlat()
{
	_currentFeature.reset(new LayFlat(selectedModels));
}

void QmlManager::closeLayFlat()
{
	_currentFeature.reset();
}

void QmlManager::generateLayFlat()
{
	auto layFlat = dynamic_cast<LayFlat*>(_currentFeature.get());
	layFlat->generateLayFlat();
}

void QmlManager::openLabelling()
{
	_currentFeature.reset(new Labelling());
}

void QmlManager::closeLabelling()
{
	stateChangeLabelling();
	_currentFeature.reset();
}

void QmlManager::setLabelText(QString text)
{
	auto labelling = dynamic_cast<Labelling*>(_currentFeature.get());
	labelling->setText(text);
}

void QmlManager::setLabelFontName(QString fontName)
{
	auto labelling = dynamic_cast<Labelling*>(_currentFeature.get());
	labelling->setFontName(fontName);
}

void QmlManager::setLabelFontBold(bool isBold)
{
	auto labelling = dynamic_cast<Labelling*>(_currentFeature.get());
	labelling->setFontBold(isBold);
}

void QmlManager::setLabelFontSize(int fontSize)
{
	auto labelling = dynamic_cast<Labelling*>(_currentFeature.get());
	labelling->setFontSize(fontSize);
}

void QmlManager::stateChangeLabelling()
{
	keyboardHandlerFocus();
	keyboardHandler->setFocus(true);
}

void QmlManager::generateLabelMesh()
{
	auto labelling = dynamic_cast<Labelling*>(_currentFeature.get());
	labelling->generateLabelMesh();
}

void QmlManager::openExtension()
{
	_currentFeature.reset(new Extend(selectedModels));
}

void QmlManager::closeExtension()
{
	_currentFeature.reset();
}

void QmlManager::generateExtensionFaces(double distance)
{
	auto extend = dynamic_cast<Extend*>(_currentFeature.get());
	extend->extendMesh(distance);
}

QString QmlManager::filenameToModelName(const std::string& s)
{
	char sep = '/';

	size_t i = s.rfind(sep, s.length());
	if (i != std::string::npos) {
		return QString::fromStdString(s.substr(i + 1, s.length() - i));
	}

	return QString::fromStdString("");
}

const std::unordered_set<GLModel*>& QmlManager::getSelectedModels()
{
	return selectedModels;
}

void QmlManager::layFlatSelect(){
    QMetaObject::invokeMethod(layflatPopup,"onApplyFinishButton");
}

void QmlManager::layFlatUnSelect() {
	QMetaObject::invokeMethod(layflatPopup, "offApplyFinishButton");
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

    qDebug() << "copying current selected Models";
    for (GLModel* model : selectedModels){
        copyMeshes.push_back(model->ID);
    }
    return;
}

void QmlManager::pasteModel(){
    for (auto copyIdx : copyMeshes){
		auto model = getModelByID(copyIdx);
		QString temp = model->modelName() + "_copy";
		createAndListModel(new Mesh(*model->getMesh()), temp, nullptr);
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
    return;
}

void QmlManager::closeSave() {
    qDebug() << "close Save";
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
    return;
}

void QmlManager::closeRotate(){
    qDebug() << "close Rotate";
    totalRotateDone();
    return;
}

void QmlManager::openOrientation(){
    qDebug() << "open orientation";
    return;
}

void QmlManager::closeOrientation(){
    qDebug() << "close orientation";
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

	switch (mode) {
	case VIEW_MODE_OBJECT:
	{
		auto layerview = dynamic_cast<LayerView*>(_currentFeature.get());
		if (layerview)
		{
			_currentFeature.reset();
		}		
		break;
	}


	case VIEW_MODE_LAYER:
	{
		_currentFeature.reset(new LayerView(selectedModels, getSelectedBound()));
		break;
	}

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


void QmlManager::openSupport()
{
	//just empty placeholder to give modality to Support.
	_currentFeature.reset(new SupportFeature());

}
void QmlManager::closeSupport()
{
	_currentFeature.reset();
}

void QmlManager::generateAutoSupport()
{
	_supportRaftManager.clear();
	for (auto selectedModel : selectedModels)
	{
		if (scfg->support_type != SlicingConfiguration::SupportType::None)
		{
			selectedModel->setZToBed();
			selectedModel->moveModel(QVector3D(0, 0, Hix::Support::SupportRaftManager::supportRaftMinLength()));
		}
		_supportRaftManager.autoGen(*selectedModel, scfg->support_type);
	}
}


void QmlManager::supportEditEnabled(bool enabled)
{
	if (enabled)
	{
		_supportRaftManager.setSupportEditMode(Hix::Support::EditMode::Manual);
		qmlManager->openResultPopUp("Click a model surface to add support.", "", "Click an existing support to remove it.");
	}
	else
	{
		_supportRaftManager.setSupportEditMode(Hix::Support::EditMode::None);
	}

}
void QmlManager::clearSupports()
{
	for (auto selectedModel : selectedModels)
	{
		selectedModel->setZToBed();
		_supportRaftManager.clear(*selectedModel);
	}

}


void QmlManager::supportApplyEdit()
{
	_supportRaftManager.applyEdits();
}


void QmlManager::supportCancelEdit()
{
	_supportRaftManager.cancelEdits();
}

void QmlManager::regenerateRaft()
{
	_supportRaftManager.generateRaft();
}

//temp features

void QmlManager::modelCut()
{
	auto modelCut = dynamic_cast<ModelCut*>(_currentFeature.get());
	modelCut->applyCut();
}
void QmlManager::cutModeSelected(int mode)
{
	auto modelCut = dynamic_cast<ModelCut*>(_currentFeature.get());
	modelCut->cutModeSelected(mode);
}
void QmlManager::openCut()
{
	_currentFeature.reset(new ModelCut(selectedModels, getSelectedBound()));

}
void QmlManager::closeCut()
{
	_currentFeature.reset();
}
void QmlManager::getSliderSignal(double sliderPos)
{
	auto modelCut = dynamic_cast<ModelCut*>(_currentFeature.get());
	if (modelCut)
	{
		modelCut->getSliderSignal(sliderPos);
		return;
	}
	auto shellOffset = dynamic_cast<ShellOffset*>(_currentFeature.get());
	if (shellOffset)
	{
		shellOffset->getSliderSignal(sliderPos);
		return;
	}
}

void QmlManager::getCrossSectionSignal(int val)
{
	auto layerview = dynamic_cast<LayerView*>(_currentFeature.get());
	if (layerview)
	{
		layerview->crossSectionSliderSignal(val);
	}
}


void QmlManager::openShellOffset() {
	if (selectedModels.size() == 1)
	{
		_currentFeature.reset(new ShellOffset(*selectedModels.begin()));

	}
	else
	{
		qmlManager->openResultPopUp("A single model must be selected", "", "");
	}

}

void QmlManager::closeShellOffset() {
	_currentFeature.reset();
}


// for shell offset
void QmlManager::generateShellOffset(double factor) {
	qmlManager->openProgressPopUp();
	qmlManager->setProgress(0.1);
	auto shellOffset = dynamic_cast<ShellOffset*>(_currentFeature.get());
	shellOffset->doOffset(factor);
	qmlManager->setProgress(1.0);

}

bool QmlManager::isFeatureActive()
{
	return _currentFeature.get() != nullptr;
}
