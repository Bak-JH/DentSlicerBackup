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
#include "Application/ApplicationManager.h"
#include "utils/utils.h"
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
#include "qml/util/QMLUtil.h"
#include "render/CircleMeshEntity.h"


#include <functional>
using namespace Qt3DCore;
using namespace Hix::Input;
using namespace Hix::UI;
using namespace Hix::Render;
using namespace Hix::Tasking;
using namespace Hix::Features;
using namespace Hix::QML;


QmlManager::QmlManager(QObject *parent) : QObject(parent), _optBackend(this, scfg)
{



	//_optBackend.createSlicingOptControls();
	//init settings

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





void QmlManager::settingFileChanged(QString path)
{
	_setting.setPrinterPath(path.toStdString());
	_bed.drawBed();
}

