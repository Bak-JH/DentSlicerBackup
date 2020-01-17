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
#include "common/debugging/DebugRenderObject.h"
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
#include "feature/extension.h"
#include "feature/scale.h"
#include "feature/arrange/autoarrange.h"
#include "feature/interfaces/WidgetMode.h"
#include "feature/interfaces/PPShaderMode.h"
#include "feature/rotate.h"
#include "feature/move.h"
#include "feature/deleteModel.h"
#include "feature/addModel.h"
#include "feature/ModelBuilder/ModelBuilderMode.h"
#include "render/CircleMeshEntity.h"
#include "Qml/components/PopupShell.h"
#include "Qml/components/Toast.h"
#include "Qml/components/SlideBar.h"
#include "Qml/components/ModalWindow.h"
#include "Qml/components/ViewMode.h"
#include "Qml/components/PartList.h"
#include "Qml/components/PrintInfo.h"

#include <functional>
using namespace Hix::Input;
using namespace Hix::UI;
using namespace Hix::Render;
using namespace Hix::Tasking;
using namespace Hix::Features;
QmlManager::QmlManager(QObject *parent) : QObject(parent), _optBackend(this, scfg)
	,layerViewFlags(LAYER_INFILL | LAYER_SUPPORTERS | LAYER_RAFT)
	, _cursorEraser(QPixmap(":/Resource/cursor_eraser.png")), _currentMode(nullptr), viewMode(0)
{
	qmlRegisterType<Hix::QML::CloseButton>("hix.qml", 1, 0, "CloseButton");
	qmlRegisterType<Hix::QML::RoundButton>("hix.qml", 1, 0, "RoundButton");
	qmlRegisterType<Hix::QML::MenuButton>("hix.qml", 1, 0, "MenuButton");
	qmlRegisterType<Hix::QML::ToggleSwitch>("hix.qml", 1, 0, "ToggleSwitch");
	qmlRegisterType<Hix::QML::ImageToggleSwitch>("hix.qml", 1, 0, "ImageToggleSwitch");

	qmlRegisterType<Hix::QML::LeftPopupShell>("hix.qml", 1, 0, "LeftPopupShell");
	qmlRegisterType<Hix::QML::ProgressPopupShell>("hix.qml", 1, 0, "ProgressPopupShell");
	qmlRegisterType<Hix::QML::ToastShell>("hix.qml", 1, 0, "ToastShell");
	qmlRegisterType<Hix::QML::SlideBarShell>("hix.qml", 1, 0, "SlideBarShell");
	qmlRegisterType<Hix::QML::RangeSlideBarShell>("hix.qml", 1, 0, "RangeSlideBarShell");

	qmlRegisterType<Hix::QML::ModalShell>("hix.qml", 1, 0, "ModalShell");
	qmlRegisterType<Hix::QML::ViewModeShell>("hix.qml", 1, 0, "ViewModeShell");

	qmlRegisterType<Hix::QML::DropdownBox>("hix.qml", 1, 0, "DropdownBox");
	qmlRegisterType<Hix::QML::InputSpinBox>("hix.qml", 1, 0, "InputSpinBox");
	qmlRegisterType<Hix::QML::TextInputBox>("hix.qml", 1, 0, "TextInputBox");

	qmlRegisterType<Hix::QML::PartList>("hix.qml", 1, 0, "PartList");

	qmlRegisterType<Hix::QML::PartDeleteButton>("hix.qml", 1, 0, "PartDeleteButton");
	qmlRegisterType<Hix::QML::PrintInfoText>("hix.qml", 1, 0, "PrintInfoText");
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

	_supportRaftManager.initialize(models);
#ifdef _DEBUG
	Hix::Debug::DebugRenderObject::getInstance().initialize(models);
#endif

    // model move componetns
    moveButton = FindItemByName(engine, "moveButton");
    movePopup = FindItemByName(engine, "movePopup");
    QObject::connect(movePopup, SIGNAL(applyMove(int, qreal, qreal)),this, SLOT(applyMove(int, qreal, qreal)));
    QObject::connect(movePopup, SIGNAL(closeMove()), this, SLOT(closeMove()));
    QObject::connect(movePopup, SIGNAL(openMove()), this, SLOT(openMove()));
	boundedBox = (QEntity*)FindItemByName(engine, "boundedBox");

    // model rotate components
    rotatePopup = FindItemByName(engine, "rotatePopup");
    // model rotate popup codes
    QObject::connect(rotatePopup, SIGNAL(applyRotation(int, qreal, qreal, qreal)),this, SLOT(applyRotation(int, qreal, qreal, qreal)));
    QObject::connect(rotatePopup, SIGNAL(openRotate()), this, SLOT(openRotate()));
    QObject::connect(rotatePopup, SIGNAL(closeRotate()), this, SLOT(closeRotate()));
    //rotateSphere->setEnabled(0);
    QObject *rotateButton = FindItemByName(engine, "rotateButton");

	scalePopup = FindItemByName(engine, "scalePopup");
	QObject::connect(scalePopup, SIGNAL(openScale()), this, SLOT(openScale()));
	QObject::connect(scalePopup, SIGNAL(closeScale()), this, SLOT(closeScale()));
	QObject::connect(scalePopup, SIGNAL(applyScale(double, double, double)), this, SLOT(applyScale(double, double, double)));


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

    // extension components
    extensionButton = FindItemByName(engine,"extendButton");
    extensionPopup = FindItemByName(engine, "extensionPopup");
	QObject::connect(extensionButton, SIGNAL(runGroupFeature(int, QString, double, double, double, QVariant)), this, SLOT(runGroupFeature(int, QString, double, double, double, QVariant)));
	QObject::connect(extensionPopup, SIGNAL(openExtension()), this, SLOT(openExtension()));
	QObject::connect(extensionPopup, SIGNAL(closeExtension()), this, SLOT(closeExtension()));
	QObject::connect(extensionPopup, SIGNAL(generateExtensionFaces(double)), this, SLOT(generateExtensionFaces(double)));

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

	modelBuilderPopup = FindItemByName(engine, "modelBuilderPopup");
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
	QObject::connect(layflatPopup, SIGNAL(closeLayflat()), this, SLOT(closeLayFlat()));

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

	QObject::connect(modelBuilderPopup, SIGNAL(openModelBuilder()), this, SLOT(openModelBuilder()));
	QObject::connect(modelBuilderPopup, SIGNAL(closeModelBuilder()), this, SLOT(closeModelBuilder()));
	QObject::connect(modelBuilderPopup, SIGNAL(buildModel()), this, SLOT(buildModel()));
	QObject::connect(modelBuilderPopup, SIGNAL(rangeSliderValueChangedFirst(double)), this, SLOT(mbRangeSliderValueChangedFirst(double)));
	QObject::connect(modelBuilderPopup, SIGNAL(rangeSliderValueChangedSecond(double)), this, SLOT(mbRangeSliderValueChangedSecond(double)));



	
	//init settings
	_bed.drawBed();
}

void QmlManager::openModelFile(){
	openProgressPopUp();

	auto mesh = new Mesh();
	auto fileUrl = QFileDialog::getOpenFileUrl(nullptr, "Please choose a file", QUrl(), "3D files(*.stl *.obj)");
	auto filename = fileUrl.fileName();
	
	if (filename == "")
	{
		setProgress(1.0);
		return;
	}

	if (filename.contains(".stl") || filename.contains(".STL")) {
		FileLoader::loadMeshSTL(mesh, fileUrl);
	}
	else if (filename.contains(".obj") || filename.contains(".OBJ")) {
		FileLoader::loadMeshOBJ(mesh, fileUrl);
	}
	filenameToModelName(filename.toStdString());
	setProgress(0.3);
	mesh->centerMesh();

	auto addModel = new Hix::Features::ListModel(mesh, filename, nullptr);
	_featureHistoryManager.addFeature(addModel);

	auto latest = addModel->getAddedModel();
	latest->setZToBed();
	setProgress(0.6);
	//repair mode
	if (Hix::Features::isRepairNeeded(mesh))
	{
		qmlManager->setProgressText("Repairing mesh.");
		std::unordered_set<GLModel*> repairModels;
		repairModels.insert(latest);
		new MeshRepair(repairModels);
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
	//_currentFeature.reset(new MeshRepair(selectedModels));
	//if cut is finished;

}

GLModel* QmlManager::getModelByID(int ID)
{
	for (auto& modelItr : glmodels)
	{
		if (modelItr.first->ID() == ID)
			return modelItr.first;
	}

	return nullptr;
}

std::unique_ptr<GLModel> QmlManager::removeFromGLModels(GLModel* target) 
{
	selectedModels.erase(target);
	auto node = glmodels.extract(target);
	return std::unique_ptr<GLModel>(std::move(node.mapped()));

}

GLModel* QmlManager::releaseFromGLModels(GLModel* target)
{
	selectedModels.erase(target);
	auto node = glmodels.extract(target);
	return node.mapped().release();
}


void QmlManager::addToGLModels(GLModel* target)
{
	if (target)
	{
		glmodels[target] = std::move(std::unique_ptr<GLModel>(target));
	}
}

void QmlManager::addToGLModels(std::unique_ptr<GLModel>&& target)
{
	if (target)
	{
		glmodels[target.get()] = std::move(target);
	}
}


void QmlManager::deleteModelFileDone() {
    QMetaObject::invokeMethod(leftTabViewMode, "setEnable", Q_ARG(QVariant, false));
	QMetaObject::invokeMethod(qmlManager->boundedBox, "hideBox");
	updateModelInfo(0, 0, "0.0 X 0.0 X 0.0 mm", 0);

    // UI
    QMetaObject::invokeMethod(qmlManager->mttab, "hideTab");
    QMetaObject::invokeMethod(boxUpperTab, "all_off");
    QMetaObject::invokeMethod(leftTabViewMode, "setObjectView");
}

void QmlManager::deleteModelFile(GLModel* model){
    qDebug() << "deletemodelfile" << glmodels.size();
	_featureHistoryManager.addFeature(new DeleteModel(model));
    deleteModelFileDone();
}

void QmlManager::deleteSelectedModels() {
    qDebug() << "deleteSelectedModels()";
    if (selectedModels.size() == 0) {
        deleteModelFileDone();
        return;
    }
	auto copySelectedModels = selectedModels;
	Hix::Features::FeatureContainerFlushSupport* container = new Hix::Features::FeatureContainerFlushSupport();
	for (auto it = copySelectedModels.begin(); it != copySelectedModels.end(); ++it)
	{
		container->addFeature(new DeleteModel(*it));
	}
	if(!container->empty())
		_featureHistoryManager.addFeature(container);
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



bool QmlManager::getGroupSelectionActive() {
    return groupSelectionActive;
}

QString QmlManager::getVersion(){
    return QString::fromStdString(settings().deployInfo.version);
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
	//_currentFeature.reset(new MeshRepair(selectedModels));
	_currentMode.reset();
	qmlManager->setProgress(1.0);
}



void QmlManager::disableObjectPickers(){
    for (auto& pair : glmodels){
		auto glm = pair.first;
		//qDebug() << glm->ID;
		glm->setHitTestable(false);
    }
}

void QmlManager::enableObjectPickers(){
	for (auto& pair : glmodels) {
		auto glm = pair.first;
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

	std::unordered_set<GLModel*> listedModels;
	for (auto& each : glmodels)
	{
		listedModels.insert(each.first);
	}
    qmlManager->openProgressPopUp();
	Autoarrange arrange(listedModels);
	qmlManager->setProgress(1);

}



GLModel* QmlManager::findGLModelByName(QString modelName){

    for(auto& pair : glmodels)
    {
		auto model = pair.second.get();
        qDebug() << "finding " << modelName << model->modelName();
        if (model->modelName() == modelName){
            return model;
        }
    }
    return NULL;
}


void QmlManager::backgroundClicked(){
    qDebug() << "background clicked";
	if (!isActive<PPShaderMode>())
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
        if ((*it)->ID() == ID){
            // do unselect model

            it = selectedModels.erase(it);
			if (target->isPrintable())
			{
				target->changeColor(Hix::Render::Colors::Default);
			}
            QMetaObject::invokeMethod(partList, "unselectPartByModel", Q_ARG(QVariant, target->ID()));
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
					QMetaObject::invokeMethod(movePopup, "offApplyFinishButton");
					break;
                case ftrRotate:
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
    QMetaObject::invokeMethod(partList, "selectPartByModel", Q_ARG(QVariant, target->ID()));
    qDebug() << "[multi model selected] b box center"; //<< xmid << " " << ymid << " " << zmid ;

    sendUpdateModelInfo();
    if (groupFunctionState == "active"){
        switch (groupFunctionIndex){
        //case 2:
        //    QMetaObject::invokeMethod(savePopup, "onApplyFinishButton");
        //    break;
        case 5:
            QMetaObject::invokeMethod(rotatePopup,"onApplyFinishButton");
			break;
        case 4:
            QMetaObject::invokeMethod(movePopup,"onApplyFinishButton");
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
            QMetaObject::invokeMethod(partList, "unselectPartByModel", Q_ARG(QVariant, (*it)->ID()));
            QMetaObject::invokeMethod(yesno_popup, "deletePartListItem", Q_ARG(QVariant, (*it)->ID()));
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
				QMetaObject::invokeMethod(rotatePopup, "offApplyFinishButton");
				break;
			case 4:
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
		QMetaObject::invokeMethod(partList, "selectPartByModel", Q_ARG(QVariant, target->ID()));
		qDebug() << "changing model" << target->ID();
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
				break;
			case 4:
				QMetaObject::invokeMethod(movePopup, "onApplyFinishButton");
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

Hix::Features::Mode* QmlManager::currentMode()const
{
	return _currentMode.get();
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
	_currentMode.reset(new LayFlatMode(selectedModels));
}

void QmlManager::closeLayFlat()
{
	_currentMode.reset();
}

void QmlManager::generateLayFlat()
{
	auto layflat = dynamic_cast<LayFlatMode*>(_currentMode.get())->applyLayFlat();
	_featureHistoryManager.addFeature(layflat);
}

void QmlManager::openLabelling()
{
	_currentMode.reset(new LabellingMode());
}

void QmlManager::closeLabelling()
{
	stateChangeLabelling();
	_currentMode.reset();
}

void QmlManager::setLabelText(QString text)
{
	dynamic_cast<LabellingMode*>(_currentMode.get())->setText(text);
}

void QmlManager::setLabelFontName(QString fontName)
{
	dynamic_cast<LabellingMode*>(_currentMode.get())->setFontName(fontName);
}

void QmlManager::setLabelFontBold(bool isBold)
{
	dynamic_cast<LabellingMode*>(_currentMode.get())->setFontBold(isBold);
}

void QmlManager::setLabelFontSize(int fontSize)
{
	dynamic_cast<LabellingMode*>(_currentMode.get())->setFontSize(fontSize);
}

void QmlManager::stateChangeLabelling()
{
	keyboardHandlerFocus();
	keyboardHandler->setFocus(true);
}

void QmlManager::generateLabelMesh()
{
	auto labelling = dynamic_cast<LabellingMode*>(_currentMode.get())->applyLabelMesh();
	if(labelling != nullptr)
		_featureHistoryManager.addFeature(labelling);
}

void QmlManager::openExtension()
{
	_currentMode.reset(new ExtendMode(selectedModels));
}

void QmlManager::closeExtension()
{
	_currentMode.reset();
}

void QmlManager::generateExtensionFaces(double distance)
{
	auto extend = dynamic_cast<ExtendMode*>(_currentMode.get())->applyExtend(distance);
	if(extend != nullptr)
		_featureHistoryManager.addFeature(extend);
}

QString QmlManager::filenameToModelName(const std::string& s)
{
	char sep = '/';

	size_t i = s.rfind(sep, s.length());
	if (i != std::string::npos) {
		return QString::fromStdString(s.substr(i + 1, s.length() - i));
	}

	return QString::fromStdString(s);
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
		unselectPart(target);
	}
}

void QmlManager::unselectAll(){
	_currentMode.reset();
	for(auto itr = selectedModels.begin(); itr != selectedModels.end();)
	{
		auto model = *itr;
		++itr;
		unselectPart(model->ID());
    }
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

	Hix::Features::FeatureContainer* container = new Hix::Features::FeatureContainer();
	for (auto it = selectedModels.begin(); it != selectedModels.end();)
	{
		auto model = *it;
		it = selectedModels.erase(it);
		container->addFeature(new DeleteModel(model));
	}

	_featureHistoryManager.addFeature(container);
	deleteModelFileDone();
}

RayCastController& QmlManager::getRayCaster()
{
	return _rayCastController;
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
	const auto& printBound = _setting.printerSetting.bedBound;
	for (auto selectedModel : selectedModels) {
		bndCheckedDisp = printBound.displaceWithin(selectedModel->recursiveAabb(), displacement);
		selectedModel->moveModel(bndCheckedDisp);
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



void QmlManager::applyMove(int axis, qreal X, qreal Y){
    if (selectedModels.empty())
        return;

	QVector3D displacement(X, Y, 0);

	auto move = dynamic_cast<Hix::Features::MoveMode*>(_currentMode.get())->applyMove(displacement);
	_featureHistoryManager.addFeature(move);

}
void QmlManager::applyRotation(int mode, qreal X, qreal Y, qreal Z){
    if (selectedModels.empty())
        return;

	for (auto selectedModel : selectedModels) {
		auto rotation = QQuaternion::fromEulerAngles(X,Y,Z);
		auto rotate = dynamic_cast<Hix::Features::RotateMode*>(_currentMode.get())->applyRotate(rotation);
		_featureHistoryManager.addFeature(rotate);
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
	emit cameraViewChangedNative();
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
        break;
 
    case ftrMove:  //move
        break;

    case ftrLayFlat:
	{
        break;
    }
    case ftrOrient:  //orient
    {
        if (state == "active"){
			for (auto selectedModel : selectedModels) {
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
	_featureHistoryManager.undo();
}

void QmlManager::reDo(){
	_featureHistoryManager.redo();
}

void QmlManager::copyModel(){
    copyMeshes.clear();

    qDebug() << "copying current selected Models";
    for (GLModel* model : selectedModels){
        copyMeshes.push_back(model->ID());
    }
    return;
}

void QmlManager::pasteModel(){
    for (auto copyIdx : copyMeshes){
		auto model = getModelByID(copyIdx);
		QString temp = model->modelName() + "_copy";
		_featureHistoryManager.addFeature( new Hix::Features::ListModel(new Mesh(*model->getMesh()), temp, nullptr));
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
	_currentMode.reset(new MoveMode(selectedModels, &_rayCastController));
    return;
}

void QmlManager::closeMove(){
    qDebug() << "close move";
	_currentMode.reset();
    return;
}


void QmlManager::openRotate(){
    qDebug() << "open Rotate";

	_currentMode.reset(new RotateMode(selectedModels, &_rayCastController));

	QMetaObject::invokeMethod(qmlManager->boundedBox, "hideBox");
    slicingData->setProperty("visible", false);
    return;
}

void QmlManager::closeRotate(){
    qDebug() << "close Rotate";
	_currentMode.reset();

    return;
}

void QmlManager::openScale() 
{
	_currentMode.reset(new ScaleMode(selectedModels));
}

void QmlManager::closeScale() 
{
	_currentMode.reset();
}

void QmlManager::applyScale(double arg1, double arg2, double arg3)
{
	float scaleX = arg1;
	float scaleY = arg2;
	float scaleZ = arg3;

	auto scale = dynamic_cast<ScaleMode*>(_currentMode.get())->applyScale(QVector3D(scaleX, scaleY, scaleZ));
	_featureHistoryManager.addFeature(scale);
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
		//auto layerview = dynamic_cast<LayerView*>(_currentFeature.get());
		//if (layerview)
		//{
		//	_currentMode.reset();
		//}		
		//break;
	}


	case VIEW_MODE_LAYER:
	{
		//_currentFeature.reset(new LayerView(selectedModels, getSelectedBound()));
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

void QmlManager::unselectPart(GLModel* target)
{
	QMetaObject::invokeMethod(partList, "unselectPartByModel", Q_ARG(QVariant, target->ID()));

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
	_currentMode.reset(new SupportMode(selectedModels));
}
void QmlManager::closeSupport()
{
	supportEditEnabled(false);
	_currentMode.reset();
}

void QmlManager::generateAutoSupport()
{
	auto autoGenSupport = dynamic_cast<SupportMode*>(_currentMode.get())->generateAutoSupport();
	if (autoGenSupport != nullptr)
		_featureHistoryManager.addFeature(autoGenSupport);
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
	auto clearSupport = dynamic_cast<SupportMode*>(_currentMode.get())->clearSupport();
	if(clearSupport != nullptr)
		_featureHistoryManager.addFeature(clearSupport);
}


void QmlManager::supportApplyEdit()
{
}


void QmlManager::supportCancelEdit()
{
}

void QmlManager::regenerateRaft()
{
	Hix::Features::FeatureContainer* container = new FeatureContainer();
	if (_supportRaftManager.raftActive())
	{
		//delete
		container->addFeature(dynamic_cast<SupportMode*>(_currentMode.get())->removeRaft());
	}
	//add
	container->addFeature(dynamic_cast<SupportMode*>(_currentMode.get())->generateRaft());
		_featureHistoryManager.addFeature(container);
}

//temp features

void QmlManager::modelCut()
{
	auto modelCut = dynamic_cast<ModelCut*>(_currentMode.get());
	modelCut->applyCut();
}
void QmlManager::cutModeSelected(int mode)
{
	auto modelCut = dynamic_cast<ModelCut*>(_currentMode.get());
		modelCut->cutModeSelected(mode);
}
void QmlManager::openCut()
{
	_currentMode.reset(new ModelCut(selectedModels, getSelectedBound()));

}
void QmlManager::closeCut()
{
	_currentMode.reset();
}
void QmlManager::getSliderSignal(double sliderPos)
{
	auto modelCut = dynamic_cast<ModelCut*>(_currentMode.get());
	if (modelCut)
	{
		modelCut->getSliderSignal(sliderPos);
		return;
	}

	auto shellOffset = dynamic_cast<ShellOffsetMode*>(_currentMode.get());
	if (shellOffset)
	{
		shellOffset->getSliderSignal(sliderPos);
		return;
	}
}

void QmlManager::getCrossSectionSignal(int val)
{
	//auto layerview = dynamic_cast<LayerView*>(_currentFeature.get());
	//if (layerview)
	//{
	//	layerview->crossSectionSliderSignal(val);
	//}
}


void QmlManager::openShellOffset() {
	if (selectedModels.size() == 1)
	{
		_currentMode.reset(new ShellOffsetMode(*selectedModels.begin()));

	}
	else
	{
		qmlManager->openResultPopUp("A single model must be selected", "", "");
	}

}

void QmlManager::closeShellOffset() {
	_currentMode.reset();
}


// for shell offset
void QmlManager::generateShellOffset(double factor) {
	openProgressPopUp();
	setProgress(0.1);
	auto shellOffset = dynamic_cast<ShellOffsetMode*>(_currentMode.get());
	_featureHistoryManager.addFeature(shellOffset->doOffset(factor));
	setProgress(1.0);

}

bool QmlManager::isFeatureActive()
{
	return _currentMode.get() != nullptr;
}


Hix::Features::Mode* QmlManager::getCurrentMode()
{
	return _currentMode.get();
}

Hix::Features::FeatureHisroyManager& QmlManager::featureHistoryManager()
{
	return _featureHistoryManager;
}

const Hix::Settings::AppSetting& QmlManager::settings() const
{
	return _setting;
}



void QmlManager::settingFileChanged(QString path)
{
	_setting.setPrinterPath(path.toStdString());
	_bed.drawBed();
}

void QmlManager::openModelBuilder()
{
	_currentMode.reset(new ModelBuilderMode());

}

void QmlManager::closeModelBuilder()
{
	_currentMode.reset();
}

void QmlManager::buildModel()
{
	auto builder = dynamic_cast<ModelBuilderMode*>(_currentMode.get());
	if (builder)
	{
		builder->build();
	}
}

void QmlManager::mbRangeSliderValueChangedFirst(double value)
{
	auto builder = dynamic_cast<ModelBuilderMode*>(_currentMode.get());
	if (builder)
	{
		builder->getSliderSignalBot(value);
	}
}

void QmlManager::mbRangeSliderValueChangedSecond(double value)
{
	auto builder = dynamic_cast<ModelBuilderMode*>(_currentMode.get());
	if (builder)
	{
		builder->getSliderSignalTop(value);
	}
}
