#include "modelcut.h"
#include "ZAxialCut.h"
#include "polylinecut.h"
#include "../qml/components/Buttons.h"
#include "../qml/components/ControlOwner.h"
#include "feature/interfaces/Feature.h"
#include "../application/ApplicationManager.h"
#include "qmlmanager.h"

using namespace Hix;
using namespace Hix::Features;
using namespace Hix::Features::Cut;
using namespace Hix::Engine3D;
using namespace Hix::Slicer;
using namespace ClipperLib;

const QUrl CUT_POPUP_URL = QUrl("qrc:/Qml/FeaturePopup/PopupCut.qml");

Hix::Features::ModelCut::ModelCut():
	_models(Hix::Application::ApplicationManager::getInstance().partManager().selectedModels()), _cuttingPlane(qmlManager->total),
	_modelsBound(Hix::Application::ApplicationManager::getInstance().partManager().selectedBound()), DialogedMode(CUT_POPUP_URL)
{
	auto& co = controlOwner();
	co.getControl(_cutSwitch, "cutswitch");
}

Hix::Features::ModelCut::~ModelCut()
{
	qmlManager->getRayCaster().setHoverEnabled(false);
	_cuttingPlane.enablePlane(false);
}

void ModelCut::cutModeSelected(int type) {
	//if flat cut		

	if (!_cutSwitch->isChecked())
	{
		_cutType = ZAxial;
		_cuttingPlane.enableDrawing(false);
		_cuttingPlane.clearPt();
		qmlManager->getRayCaster().setHoverEnabled(false);
		_cuttingPlane.transform().setTranslation(QVector3D(0, 0, _modelsBound.zMin() + 1 * _modelsBound.lengthZ() / 1.8));
		_cuttingPlane.enablePlane(true);
	}
	else if (_cutSwitch->isChecked())
	{
		_cutType = Polyline;
		_cuttingPlane.enableDrawing(true);
		//want cutting plane to be over model mesh
		float zOverModel = _modelsBound.zMax() + 0.1f;
		_cuttingPlane.transform().setTranslation(QVector3D(0, 0, zOverModel));
		_cuttingPlane.enablePlane(true);
		qmlManager->getRayCaster().setHoverEnabled(true);
	}
	return;
}



void ModelCut::getSliderSignal(double value) {
	float zlength = _modelsBound.lengthZ();
	qDebug() << value;
	_cuttingPlane.transform().setTranslation(QVector3D(0, 0, _modelsBound.zMin() + value * zlength / 1.8));
}

void Hix::Features::ModelCut::applyCut()
{
	switch (_cutType)
	{
	case Hix::Features::ModelCut::ZAxial:
	{
		for (auto each : _models)
		{
			qmlManager->taskManager().enqueTask(new ZAxialCut(each, _cuttingPlane.transform().translation().z(), Hix::Features::Cut::KeepBoth));
		}
		break;
	}
	case Hix::Features::ModelCut::Polyline:
	{
		for (auto each : _models)
		{
			qmlManager->taskManager().enqueTask(new PolylineCut(each, _cuttingPlane.contour()));
		}
		break;
	}
	default:
		break;
	}
	//Hix::Application::ApplicationManager::getInstance().partManager().unselectAll();
}