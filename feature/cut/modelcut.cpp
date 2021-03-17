#include "modelcut.h"
#include "ZAxialCut.h"
#include "polylinecut.h"
#include "../qml/components/Buttons.h"
#include "../qml/components/ControlOwner.h"
#include "feature/interfaces/Feature.h"
#include "../application/ApplicationManager.h"


using namespace Hix;
using namespace Hix::Features;
using namespace Hix::Features::Cut;
using namespace Hix::Engine3D;
using namespace Hix::Slicer;
using namespace ClipperLib;

const QUrl CUT_POPUP_URL = QUrl("qrc:/Qml/FeaturePopup/PopupCut.qml");

Hix::Features::ModelCut::ModelCut() :
	_models(Hix::Application::ApplicationManager::getInstance().partManager().selectedModels()),
	_modelsBound(Hix::Application::ApplicationManager::getInstance().partManager().selectedBound()),
	DialogedMode(CUT_POPUP_URL),
	SliderMode(0, Hix::Application::ApplicationManager::getInstance().partManager().selectedBound().lengthZ())
{
	if (Hix::Application::ApplicationManager::getInstance().partManager().selectedModels().empty())
	{
		Hix::Application::ApplicationManager::getInstance().modalDialogManager().needToSelectModels();
		return;
	}
	auto& co = controlOwner();
	co.getControl(_cutSwitch, "cutswitch");
	QObject::connect(_cutSwitch, &Hix::QML::Controls::ToggleSwitch::checkedChanged, [this]() { cutModeSelected(); });
	QObject::connect(&slider(), &Hix::QML::SlideBarShell::valueChanged, [this]() {
		_cuttingPlane->transform().setTranslation(QVector3D(0, 0, _modelsBound.zMin() + slider().getValue()));
		});
	cutModeSelected();
}

Hix::Features::ModelCut::~ModelCut()
{
	Hix::Application::ApplicationManager::getInstance().getRayCaster().setHoverEnabled(false);
}

void ModelCut::cutModeSelected() 
{
	//if flat cut		
	if (!_cutSwitch->isChecked())
	{
		_cuttingPlane.emplace(Hix::Application::ApplicationManager::getInstance().sceneManager().total(), 1.0f);
		_cutType = ZAxial;
		_cuttingPlane->enableDrawing(false);
		_cuttingPlane->clearPt();
		Hix::Application::ApplicationManager::getInstance().getRayCaster().setHoverEnabled(false);
		_cuttingPlane->transform().setTranslation(QVector3D(0, 0, _modelsBound.zMin() + 1 * _modelsBound.lengthZ() / 1.8));
		_cuttingPlane->enablePlane(true);
		Hix::Application::ApplicationManager::getInstance().sceneManager().setViewPreset(Hix::Application::SceneManager::ViewPreset::Center);
		slider().setValue(_modelsBound.zMin() + 1.0 * _modelsBound.lengthZ());
		slider().setVisible	(true);
	}
	else if (_cutSwitch->isChecked())
	{
		_cuttingPlane.emplace(Hix::Application::ApplicationManager::getInstance().sceneManager().total(), 0.0f);
		_cutType = Polyline;
		_cuttingPlane->enableDrawing(true);
		//want cutting plane to be over model mesh
		float zOverModel = _modelsBound.zMax() + 0.1f;
		_cuttingPlane->transform().setTranslation(QVector3D(0, 0, zOverModel));
		_cuttingPlane->enablePlane(true);
		Hix::Application::ApplicationManager::getInstance().getRayCaster().setHoverEnabled(true);
		Hix::Application::ApplicationManager::getInstance().sceneManager().setViewPreset(Hix::Application::SceneManager::ViewPreset::Down);
		slider().setVisible(false);
	}
	return;
}




void Hix::Features::ModelCut::applyButtonClicked()
{
	switch (_cutType)
	{
	case Hix::Features::ModelCut::ZAxial:
	{
		for (auto each : _models)
		{
			Hix::Application::ApplicationManager::getInstance().taskManager().enqueTask(new ZAxialCut(each, _cuttingPlane->transform().translation().z(), Hix::Features::Cut::KeepBoth));
		}
		break;
	}
	case Hix::Features::ModelCut::Polyline:
	{
		auto cuttingPts = _cuttingPlane->contour();
		if (!cuttingPts.empty())
		{
			for (auto each : _models)
			{
				Hix::Application::ApplicationManager::getInstance().taskManager().enqueTask(new PolylineCut(each, cuttingPts));
			}
		}
		break;
	}
	default:
		break;
	}
	//Hix::Application::ApplicationManager::getInstance().partManager().unselectAll();
}
