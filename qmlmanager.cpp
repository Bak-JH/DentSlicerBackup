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
#include "feature/sliceExport.h"
#include "feature/UndoRedo.h"
#include "feature/stlexport.h"

#include "render/CircleMeshEntity.h"
#include "Qml/components/PopupShell.h"
#include "Qml/components/Toast.h"
#include "Qml/components/SlideBar.h"
#include "Qml/components/ViewMode.h"
#include "Qml/components/PrintInfo.h"
#include "Qml/components/Buttons.h"
#include "Qml/components/Inputs.h"
#include "Qml/components/FeatureMenu.h"


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


	qmlRegisterType<Hix::QML::FeaturePopupShell>("hix.qml", 1, 0, "FeaturePopupShell");
	qmlRegisterType<Hix::QML::ProgressPopupShell>("hix.qml", 1, 0, "ProgressPopupShell");
	qmlRegisterType<Hix::QML::ToastShell>("hix.qml", 1, 0, "ToastShell");
	qmlRegisterType<Hix::QML::SlideBarShell>("hix.qml", 1, 0, "SlideBarShell");
	qmlRegisterType<Hix::QML::RangeSlideBarShell>("hix.qml", 1, 0, "RangeSlideBarShell");
	qmlRegisterType<Hix::QML::ModalShell>("hix.qml", 1, 0, "ModalShell");
	qmlRegisterType<Hix::QML::ViewModeShell>("hix.qml", 1, 0, "ViewModeShell");

	qmlRegisterType<Hix::QML::PartList>("hix.qml", 1, 0, "PartList");

	qmlRegisterType<Hix::QML::PrintInfoText>("hix.qml", 1, 0, "PrintInfoText");

	qmlRegisterType<Hix::QML::FeatureMenu>("hix.qml", 1, 0, "FeatureMenu");

	qmlRegisterType<Hix::QML::Controls::Button>("hix.qml", 1, 0, "Button");
	qmlRegisterType<Hix::QML::Controls::ToggleSwitch>("hix.qml", 1, 0, "ToggleSwitch");

	qmlRegisterType<Hix::QML::Controls::DropdownBox>("hix.qml", 1, 0, "DropdownBox");
	qmlRegisterType<Hix::QML::Controls::InputSpinBox>("hix.qml", 1, 0, "InputSpinBox");
	qmlRegisterType<Hix::QML::Controls::TextInputBox>("hix.qml", 1, 0, "TextInputBox");
}

void QmlManager::initializeUI(){
	featureArea = dynamic_cast<QQuickItem*>(FindItemByName(engine, "featureArea"));
	mainWindow = FindItemByName(engine, "mainWindow");
	loginWindow = FindItemByName(engine, "loginWindow");
	loginButton = FindItemByName(engine, "loginButton");
	mv = dynamic_cast<QEntity*> (FindItemByName(engine, "MainView"));
	QMetaObject::invokeMethod(mv, "initCamera");

	//initialize ray casting mouse input controller
	QEntity* camera = dynamic_cast<QEntity*>(FindItemByName(engine, "cm"));
	_rayCastController.initialize(camera);

    keyboardHandler = (Qt3DInput::QKeyboardHandler*)FindItemByName(engine, "keyboardHandler");
    models = (QEntity *)FindItemByName(engine, "Models");
    Lights* lights = new Lights(models);
    systemTransform = (Qt3DCore::QTransform *) FindItemByName(engine, "systemTransform");
    mttab = (QEntity *)FindItemByName(engine, "mttab");

	total = dynamic_cast<QEntity*> (FindItemByName(engine, "total"));
	_camera = dynamic_cast<Qt3DRender::QCamera*> (FindItemByName(engine, "camera"));

	_supportRaftManager.initialize(models);
#ifdef _DEBUG
	Hix::Debug::DebugRenderObject::getInstance().initialize(models);
#endif


	boundedBox = (QEntity*)FindItemByName(engine, "boundedBox");


    undoRedoButton = FindItemByName(engine, "undoRedoButton");
    slicingData = FindItemByName(engine, "slicingData");


	ltso = FindItemByName(engine, "ltso");
	_optBackend.createSlicingOptControls();

    // selection popup
    yesno_popup = FindItemByName(engine, "yesno_popup");
    result_popup = FindItemByName(engine, "result_popup");


    QObject::connect(mttab,SIGNAL(runGroupFeature(int,QString, double, double, double,QVariant)),this,SLOT(runGroupFeature(int,QString, double, double, double, QVariant)));



    boxUpperTab = FindItemByName(engine, "boxUpperTab");
    boxLeftTab = FindItemByName(engine, "boxLeftTab");
    scene3d = dynamic_cast<QQuickItem*> (FindItemByName(engine, "scene3d"));

    QObject::connect(boxUpperTab,SIGNAL(runGroupFeature(int,QString, double, double, double, QVariant)),this,SLOT(runGroupFeature(int,QString, double, double, double, QVariant)));

    QObject::connect(this, SIGNAL(arrangeDone(std::vector<QVector3D>, std::vector<float>)), this, SLOT(applyArrangeResult(std::vector<QVector3D>, std::vector<float>)));



    QObject::connect(undoRedoButton, SIGNAL(unDo()), this, SLOT(unDo()));
    QObject::connect(undoRedoButton, SIGNAL(reDo()), this, SLOT(reDo()));
    QObject::connect(mv, SIGNAL(unDo()), this, SLOT(unDo()));
    QObject::connect(mv, SIGNAL(reDo()), this, SLOT(reDo()));

    httpreq* hr = new httpreq();
    QObject::connect(loginButton, SIGNAL(loginTrial(QString)), hr, SLOT(get_iv(QString)));


    QObject::connect(mv, SIGNAL(copy()), this, SLOT(copyModel()));
    QObject::connect(mv, SIGNAL(paste()), this, SLOT(pasteModel()));
    QObject::connect(mv, SIGNAL(groupSelectionActivate(bool)), this, SLOT(groupSelectionActivate(bool)));
    QObject::connect(yesno_popup, SIGNAL(runGroupFeature(int, QString, double, double, double, QVariant)),this,SLOT(runGroupFeature(int,QString, double, double, double, QVariant)));
	QObject::connect(mv, SIGNAL(cameraViewChanged()), this, SLOT(cameraViewChanged()));




	
	//init settings
	_bed.drawBed();

}

void QmlManager::openModelFile(){
	openProgressPopUp();

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
	QMetaObject::invokeMethod(qmlManager->boundedBox, "hideBox");
	updateModelInfo(0, 0, "0.0 X 0.0 X 0.0 mm", 0);

    // UI
    QMetaObject::invokeMethod(qmlManager->mttab, "hideTab");
    QMetaObject::invokeMethod(boxUpperTab, "all_off");
}

void QmlManager::deleteModelFile(GLModel* model){
    qDebug() << "deletemodelfile" << glmodels.size();
	_taskManager.enqueTask(new DeleteModel(model));
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
		_taskManager.enqueTask(container);
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
	return Hix::Engine3D::combineBounds(selectedModels);
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
}

// slicing information
void QmlManager::sendUpdateModelInfo(int printing_time, int layer, QString xyz, float volume){
    qDebug() << "sent update model Info";
    updateModelInfo(printing_time, layer, xyz, volume);
}

void QmlManager::openArrange(){
	if (glmodels.size() > 1)
	{
		openYesNoPopUp(false, "Click OK to auto-arrange models.", "", "", 18, "qrc:/Resource/popup_image/image_arrange.png", ftrArrange, 1);
	}
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

    //        if (groupFunctionState == "active"){
    //            switch (groupFunctionIndex){
    //            //case 2:
    //            //    QMetaObject::invokeMethod(savePopup, "offApplyFinishButton");
    //            //    break;
				//case ftrMove:
				//	QMetaObject::invokeMethod(movePopup, "offApplyFinishButton");
				//	break;
    //            case ftrRotate:
    //                QMetaObject::invokeMethod(rotatePopup,"offApplyFinishButton");
    //                break;
    //            }
    //        }
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
   //     switch (groupFunctionIndex){
   //     //case 2:
   //     //    QMetaObject::invokeMethod(savePopup, "onApplyFinishButton");
   //     //    break;
   //     case 5:
   //         QMetaObject::invokeMethod(rotatePopup,"onApplyFinishButton");
			//break;
   //     case 4:
   //         QMetaObject::invokeMethod(movePopup,"onApplyFinishButton");
			//break;
   //     case 6:
   //         QMetaObject::invokeMethod(layflatPopup,"onApplyFinishButton");
   //         break;
   //     case 8:
   //         QMetaObject::invokeMethod(orientPopup,"onApplyFinishButton");
   //         break;
   //     }
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
			//switch (groupFunctionIndex) {
			//	//case 2:
			//	//    QMetaObject::invokeMethod(savePopup, "offApplyFinishButton");
			//	//    break;
			//case 5:
			//	QMetaObject::invokeMethod(rotatePopup, "offApplyFinishButton");
			//	break;
			//case 4:
			//	QMetaObject::invokeMethod(movePopup, "offApplyFinishButton");
			//	break;
			//case 6:
			//	QMetaObject::invokeMethod(layflatPopup, "offApplyFinishButton");
			//	break;
			//case 8:
			//	QMetaObject::invokeMethod(orientPopup, "offApplyFinishButton");
			//	break;
			//case 10:
			//	QMetaObject::invokeMethod(repairPopup, "offApplyFinishButton");
			//	break;
			//}
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
   //         qDebug() << "@@@@ selected2 @@@@" << groupFunctionIndex;
			//switch (groupFunctionIndex) {
			//	//case 2:
			//	//    QMetaObject::invokeMethod(savePopup, "onApplyFinishButton");
			//	//    break;
			//case 5:
			//	QMetaObject::invokeMethod(rotatePopup, "onApplyFinishButton");
			//	break;
			//case 4:
			//	QMetaObject::invokeMethod(movePopup, "onApplyFinishButton");
			//	break;
			//case 6:
			//	QMetaObject::invokeMethod(layflatPopup, "onApplyFinishButton");
			//	break;
			//case 8:
			//	QMetaObject::invokeMethod(orientPopup, "onApplyFinishButton");
			//	break;
			//case 10:
			//	QMetaObject::invokeMethod(repairPopup, "onApplyFinishButton");
			//	break;
			//}
		}
	}


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
	_currentMode.reset(new LayFlatMode());
}

void QmlManager::closeLayFlat()
{
	_currentMode.reset();
}

void QmlManager::generateLayFlat()
{
	auto layflat = dynamic_cast<LayFlatMode*>(_currentMode.get());
	if (layflat)
		layflat->applyLayFlat();
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
		_taskManager.enqueTask(labelling);
}

void QmlManager::openExtension()
{
	_currentMode.reset(new ExtendMode());
}

void QmlManager::closeExtension()
{
	_currentMode.reset();
}


const std::unordered_set<GLModel*>& QmlManager::getSelectedModels()
{
	return selectedModels;
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

	_taskManager.enqueTask(container);
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
	_taskManager.enqueTask(move);

}
void QmlManager::applyRotation(int mode, qreal X, qreal Y, qreal Z){
    if (selectedModels.empty())
        return;

	for (auto selectedModel : selectedModels) {
		auto rotation = QQuaternion::fromEulerAngles(X,Y,Z);
		auto rotate = dynamic_cast<Hix::Features::RotateMode*>(_currentMode.get())->applyRotate(rotation);
		_taskManager.enqueTask(rotate);
    }
}



void QmlManager::cameraViewChanged()
{
	auto widgetMode = dynamic_cast<Hix::Features::WidgetMode*>(_currentMode.get());
	if (widgetMode)
	{
		widgetMode->updatePosition();
	}
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
	{
		//mode-less since no UI change except for the file dialog.
		break;

	}

    }
}

void QmlManager::unDo(){
	_taskManager.enqueTask(std::make_unique<Undo>());
}

void QmlManager::reDo(){
	_taskManager.enqueTask(std::make_unique<Redo>());
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
		_taskManager.enqueTask(new Hix::Features::ListModel(new Mesh(*model->getMesh()), temp, nullptr));
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
    //progress = 0;
    //QMetaObject::invokeMethod(progress_popup, "openPopUp");
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
    //if (value == 0 || value >= progress){
    //    QMetaObject::invokeMethod(progress_popup, "updateNumber",
    //                                  Q_ARG(QVariant, value));
    //    progress = value;
    //}
}



void QmlManager::openMove(){
    //moveActive = true;
	_currentMode.reset(new MoveMode());
    return;
}

void QmlManager::closeMove(){
    qDebug() << "close move";
	_currentMode.reset();
    return;
}


void QmlManager::openRotate(){
    qDebug() << "open Rotate";

	_currentMode.reset(new RotateMode());

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
	_currentMode.reset(new ScaleMode());
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
	_taskManager.enqueTask(scale);
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
    //QMetaObject::invokeMethod(progress_popup, "updateText",
    //                          Q_ARG(QVariant, QString::fromStdString(inputText)));
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
  //      if (viewMode == 2) viewLayerButton->setProperty("checked", true);
		//this->viewMode = viewMode;
		//switch (viewMode) {
		//case VIEW_MODE_OBJECT:
		//{
		//	QMetaObject::invokeMethod(yesno_popup, "closePopUp");
		//	QMetaObject::invokeMethod(leftTabViewMode, "setObjectView");
		//	auto layerview = dynamic_cast<LayerView*>(_currentMode.get());
		//	if (layerview)
		//		_currentMode.reset();
		//	break;
		//}
		//case VIEW_MODE_LAYER:
		//{
		//	_currentMode.reset(new LayerView(selectedModels, getSelectedBound()));
		//	break;
		//}
		//}
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
    //QMetaObject::invokeMethod(leftTabViewMode, "setEnable", Q_ARG(QVariant, false));
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
	_currentMode.reset(new SupportMode());
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
		_taskManager.enqueTask(autoGenSupport);
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
	auto mode = dynamic_cast<SupportMode*>(_currentMode.get());
	if (mode)
	{
		_taskManager.enqueTask(mode->clearSupport());
	}
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
		_taskManager.enqueTask(container);
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
	_currentMode.reset(new ModelCut());

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




// for shell offset
void QmlManager::generateShellOffset(double factor) {
	openProgressPopUp();
	setProgress(0.1);
	auto shellOffset = dynamic_cast<ShellOffsetMode*>(_currentMode.get());
	_taskManager.enqueTask(shellOffset->doOffset(factor));
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

void QmlManager::setMode(Hix::Features::Mode* mode)
{

	_currentMode.reset(mode);
	auto instantMode = dynamic_cast<Hix::Features::InstantMode*>(mode);
	if (instantMode)
	{
		_currentMode.reset(nullptr);
	}

}

const std::unordered_set<GLModel*>& QmlManager::getAllModels()
{
	std::unordered_set<GLModel*> listedModels;
	for (auto& each : glmodels)
	{
		listedModels.insert(each.first);
	}
	return listedModels;
}
