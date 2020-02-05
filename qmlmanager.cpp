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
#include "ApplicationManager.h"
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


#include <functional>
using namespace Hix::Input;
using namespace Hix::UI;
using namespace Hix::Render;
using namespace Hix::Tasking;
using namespace Hix::Features;


QObject* FindItemByName(QList<QObject*> nodes, const QString& name);
QObject* FindItemByName(QQmlApplicationEngine* engine, const QString& name);


QmlManager::QmlManager(QObject *parent) : QObject(parent), _optBackend(this, scfg)
	, _cursorEraser(QPixmap(":/Resource/cursor_eraser.png")), _currentMode(nullptr), viewMode(0)
{
	auto engine =  &Hix::Application::ApplicationManager::getInstance().engine();
	featureArea = dynamic_cast<QQuickItem*>(FindItemByName(engine, "featureArea"));
	mainWindow = FindItemByName(engine, "mainWindow");
	loginWindow = FindItemByName(engine, "loginWindow");
	loginButton = FindItemByName(engine, "loginButton");
	mv = dynamic_cast<QEntity*> (FindItemByName(engine, "MainView"));
	QMetaObject::invokeMethod(mv, "initCamera");

	//initialize ray casting mouse input controller
	QEntity* camera = dynamic_cast<QEntity*>(FindItemByName(engine, "cm"));
	_rayCastController.initialize(camera);

	models = (QEntity*)FindItemByName(engine, "Models");
	Lights* lights = new Lights(models);
	systemTransform = (Qt3DCore::QTransform*) FindItemByName(engine, "systemTransform");
	mttab = (QEntity*)FindItemByName(engine, "mttab");

	total = dynamic_cast<QEntity*> (FindItemByName(engine, "total"));
	_camera = dynamic_cast<Qt3DRender::QCamera*> (FindItemByName(engine, "camera"));

	_supportRaftManager.initialize(models);
#ifdef _DEBUG
	Hix::Debug::DebugRenderObject::getInstance().initialize(models);
#endif


	boundedBox = (QEntity*)FindItemByName(engine, "boundedBox");


	undoRedoButton = FindItemByName(engine, "undoRedoButton");
	slicingData = FindItemByName(engine, "slicingData");


	_optBackend.createSlicingOptControls();




	QObject::connect(mttab, SIGNAL(runGroupFeature(int, QString, double, double, double, QVariant)), this, SLOT(runGroupFeature(int, QString, double, double, double, QVariant)));



	boxUpperTab = FindItemByName(engine, "boxUpperTab");
	boxLeftTab = FindItemByName(engine, "boxLeftTab");
	scene3d = dynamic_cast<QQuickItem*> (FindItemByName(engine, "scene3d"));

	QObject::connect(boxUpperTab, SIGNAL(runGroupFeature(int, QString, double, double, double, QVariant)), this, SLOT(runGroupFeature(int, QString, double, double, double, QVariant)));

	QObject::connect(this, SIGNAL(arrangeDone(std::vector<QVector3D>, std::vector<float>)), this, SLOT(applyArrangeResult(std::vector<QVector3D>, std::vector<float>)));



	httpreq* hr = new httpreq();
	QObject::connect(loginButton, SIGNAL(loginTrial(QString)), hr, SLOT(get_iv(QString)));


	QObject::connect(mv, SIGNAL(copy()), this, SLOT(copyModel()));
	QObject::connect(mv, SIGNAL(paste()), this, SLOT(pasteModel()));
	QObject::connect(mv, SIGNAL(groupSelectionActivate(bool)), this, SLOT(groupSelectionActivate(bool)));
	QObject::connect(mv, SIGNAL(cameraViewChanged()), this, SLOT(cameraViewChanged()));

	//init settings
	_bed.drawBed();

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


QVector3D QmlManager::getSelectedCenter()
{
	//get full bound
	return getSelectedBound().centre();
}
QVector3D QmlManager::selectedModelsLengths(){
	return getSelectedBound().lengths();
}






QString QmlManager::getVersion(){
    return QString::fromStdString(settings().deployInfo.version);
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
	return FindItemByName(engine, name);
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
