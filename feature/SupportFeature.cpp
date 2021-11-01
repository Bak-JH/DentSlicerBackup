#include "SupportFeature.h"
#include "../../input/raycastcontroller.h"
#include "render/Color.h"
#include "feature/move.h"
#include "support/RaftModel.h"
#include "application/ApplicationManager.h"
#include "../../glmodel.h"
#include "../Qml/components/Inputs.h"
#include "../Qml/components/Buttons.h"

using namespace Hix::Features;
using namespace Hix::Application;
using namespace Hix::Settings;
/////////////////////
///  Add Support  ///
/////////////////////

Hix::Features::AddSupport::AddSupport(std::unique_ptr<SupportModel> model):_model(std::move(model))
{
}

Hix::Features::AddSupport::~AddSupport()
{}

void Hix::Features::AddSupport::undoImpl()
{
	postUIthread([this]() {
		_model = Hix::Application::ApplicationManager::getInstance().supportRaftManager().removeSupport(std::get<SupportModel*>(_model));
	});
}

void Hix::Features::AddSupport::redoImpl()
{
	postUIthread([this]() {
		_model = Hix::Application::ApplicationManager::getInstance().supportRaftManager().addSupport(std::move(std::get<std::unique_ptr<SupportModel>>(_model)));
	});
}

void Hix::Features::AddSupport::runImpl()
{
	redoImpl();
}
//
//
//Hix::Features::AddInterconnect::AddInterconnect(std::array<SupportModel*, 2> pts): _pts(pts)
//{
//}
//
//Hix::Features::AddInterconnect::~AddInterconnect()
//{
//}
//
//void Hix::Features::AddInterconnect::undoImpl()
//{
//	postUIthread([this]() {
//		_model = Hix::Application::ApplicationManager::getInstance().supportRaftManager().removeSupport(std::get<SupportModel*>(_model));
//		});
//}
//
//void Hix::Features::AddInterconnect::redoImpl()
//{
//	postUIthread([this]() {
//		_model = Hix::Application::ApplicationManager::getInstance().supportRaftManager().addSupport(std::move(std::get<std::unique_ptr<SupportModel>>(_model)));
//		});
//}
//
//void Hix::Features::AddInterconnect::runImpl()
//{
//	postUIthread([this]() {
//		auto newModel = Hix::Application::ApplicationManager::getInstance().supportRaftManager().addInterconnect(_pts);
//		_model = newModel;
//		});
//}
//
//

Hix::Features::ManualSupport::ManualSupport(const OverhangDetect::Overhang& overhang):_overhang(overhang)
{
}

Hix::Features::ManualSupport::~ManualSupport()
{
}

void Hix::Features::ManualSupport::runImpl()
{
	auto& srMan = Hix::Application::ApplicationManager::getInstance().supportRaftManager();
	std::unique_ptr<SupportModel> model;
	postUIthread([&]() {
		model = srMan.createSupport(_overhang);
		});
	auto supportFeature = new AddSupport(std::move(model));
	tryRunFeature(*supportFeature);
	addFeature(supportFeature);
}


Hix::Features::AutoSupport::AutoSupport(GLModel* model): _model(model){}
Hix::Features::AutoSupport::~AutoSupport(){}

void Hix::Features::AutoSupport::runImpl()
{
	auto& srMan = Hix::Application::ApplicationManager::getInstance().supportRaftManager();
	auto move = new Move(_model, QVector3D(0, 0, Hix::Support::SupportRaftManager::supportRaftMinLength()));
	tryRunFeature(*move);
	addFeature(move);
	auto overhangs = srMan.detectOverhang(*_model);
	for (auto overhang : overhangs)
	{
		std::unique_ptr<SupportModel> model;
		postUIthread([&]() {
			model = srMan.createSupport(overhang);
		});
		auto supportFeature = new AddSupport(std::move(model));
		tryRunFeature(*supportFeature);
		addFeature(supportFeature);
	}
}

Hix::Features::GenerateInterconnections::GenerateInterconnections(std::array<SupportModel*, 2> models):_models(models)
{
}

Hix::Features::GenerateInterconnections::~GenerateInterconnections()
{
}

void Hix::Features::GenerateInterconnections::runImpl()
{
	auto& srMan = Hix::Application::ApplicationManager::getInstance().supportRaftManager();
	std::vector<std::unique_ptr<SupportModel>> itcs;
	postUIthread([&]() {
		itcs = srMan.createInterconnects(_models);
		});
	for (auto& itc : itcs)
	{
		auto supportFeature = new AddSupport(std::move(itc));
		addFeature(supportFeature);
	}
	FeatureContainer::runImpl();
}


////////////////////////
///  Remove Support  ///
////////////////////////
Hix::Features::RemoveSupport::RemoveSupport(SupportModel* target): _model(target)
{}

Hix::Features::RemoveSupport::~RemoveSupport()
{
}

void Hix::Features::RemoveSupport::undoImpl()
{
	auto owning = std::move(std::get<std::unique_ptr<SupportModel>>(_model));
	if (owning)
	{
		postUIthread([this, &owning]() {
			_model = Hix::Application::ApplicationManager::getInstance().supportRaftManager().addSupport(std::move(owning));
		});
	}

}

void Hix::Features::RemoveSupport::redoImpl()
{

	postUIthread([this]() {
		_model = Hix::Application::ApplicationManager::getInstance().supportRaftManager().removeSupport(std::get<SupportModel*>(_model));
	});
}

void Hix::Features::RemoveSupport::runImpl()
{
	redoImpl();
}



//////////////////
///  Add Raft  ///
//////////////////
Hix::Features::AddRaft::AddRaft()
{}

Hix::Features::AddRaft::~AddRaft()
{}

void Hix::Features::AddRaft::undoImpl()
{
	postUIthread([this]() {
		_model = Hix::Application::ApplicationManager::getInstance().supportRaftManager().removeRaft();
	});
}

void Hix::Features::AddRaft::redoImpl()
{
	postUIthread([this]() {
		_model = Hix::Application::ApplicationManager::getInstance().supportRaftManager().addRaft(std::move(std::get<std::unique_ptr<RaftModel>>(_model)));
	});
}

void Hix::Features::AddRaft::runImpl()
{
	postUIthread([this]() {
		_model = Hix::Application::ApplicationManager::getInstance().supportRaftManager().generateRaft();
	});

}



/////////////////////
///  Remove Raft  ///
/////////////////////
Hix::Features::RemoveRaft::RemoveRaft()
{
}

Hix::Features::RemoveRaft::~RemoveRaft()
{}


void Hix::Features::RemoveRaft::undoImpl()
{
	postUIthread([this]() {
		Hix::Application::ApplicationManager::getInstance().supportRaftManager().addRaft(std::move(std::get<std::unique_ptr<RaftModel>>(_model)));
	});
}

void Hix::Features::RemoveRaft::redoImpl()
{
	postUIthread([this]() {
		_model = Hix::Application::ApplicationManager::getInstance().supportRaftManager().removeRaft();
	});
}

void Hix::Features::RemoveRaft::runImpl()
{
	redoImpl();
}



////////////////////
/// Support Mode ///
////////////////////
const QUrl SUPPORT_POPUP_URL = QUrl("qrc:/Qml/FeaturePopup/PopupSupport.qml");

Hix::Features::SupportMode::SupportMode()
	: _targetModels(Hix::Application::ApplicationManager::getInstance().partManager().selectedModels()), DialogedMode(SUPPORT_POPUP_URL)
{
	if (Hix::Application::ApplicationManager::getInstance().partManager().selectedModels().empty())
	{
		Hix::Application::ApplicationManager::getInstance().modalDialogManager().needToSelectModels();
		return;
	}
	Hix::Application::ApplicationManager::getInstance().getRayCaster().setHoverEnabled(true);
	

	auto& co = controlOwner();
	co.getControl(_generateSupportsBttn, "generatesupports");
	co.getControl(_generateRaftBttn, "generateraft");
	co.getControl(_clearSupportsBttn, "clearsupports");
	co.getControl(_reconnectBttn, "reconnect");
	co.getControl(_manualEditBttn, "editsupports");
	co.getControl(_thickenFeetBttn, "thickenFeet");
	co.getControl(_suppSettBttn, "supportsettingbutton");
	co.getControl(_raftSettBttn, "raftsettingbutton");
	co.getControl(_suppTypeDrop, "supporttype");
	co.getControl(_raftTypeDrop, "rafttype");
	co.getControl(_suppDensitySpin, "supportdensity");
	co.getControl(_maxRadSpin, "maxradius");
	co.getControl(_minRadSpin, "minradius");
	co.getControl(_raftRadiusMultSpin, "raftRadiusMult");
	co.getControl(_raftMinMaxRatioSpin, "raftMinMaxRatio");
	co.getControl(_raftThickness, "raftThickness");
	co.getControl(_interconnectTypeDrop, "interconnecttype");
	co.getControl(_supportBaseHeightSpin, "supportBaseHeight");
	co.getControl(_maxConnectDistanceSpin, "maxConnectDistance");

	auto& settings = Hix::Application::ApplicationManager::getInstance().settings().supportSetting;

	_suppTypeDrop->setEnums<SupportSetting::SupportType>(settings.supportType);
	_raftTypeDrop->setEnums<SupportSetting::RaftType>(settings.raftType);
	_suppDensitySpin->setValue(settings.supportDensity);
	_maxRadSpin->setValue(settings.supportRadiusMax);
	_minRadSpin->setValue(settings.supportRadiusMin);
	_raftRadiusMultSpin->setValue(settings.raftRadiusMult);
	_raftMinMaxRatioSpin->setValue(settings.raftMinMaxRatio);
	_raftThickness->setValue(settings.raftThickness);
	_interconnectTypeDrop->setEnums<SupportSetting::InterconnectType>(settings.interconnectType);
	_supportBaseHeightSpin->setValue(settings.supportBaseHeight);
	_maxConnectDistanceSpin->setValue(settings.maxConnectDistance);
	_thickenFeetBttn->setChecked(settings.thickenFeet);

	_suppDensitySpin->setRange(5, 100);
	_maxRadSpin->setRange(0.1, 5);
	_minRadSpin->setRange(0.1, 5);
	_raftRadiusMultSpin->setRange(0.5, 100);
	_raftMinMaxRatioSpin->setRange(1, 10);
	_raftThickness->setRange(0.05, 10);
	_supportBaseHeightSpin->setRange(1, 20);
	_maxConnectDistanceSpin->setRange(0, 10000);

	// bind buttons
	QObject::connect(_generateSupportsBttn, &Hix::QML::Controls::Button::clicked, [this]() {
		generateAutoSupport(Hix::Application::ApplicationManager::getInstance().partManager().selectedModels());
		});
	QObject::connect(_generateRaftBttn, &Hix::QML::Controls::Button::clicked, [this]() {
		regenerateRaft();
		});
	QObject::connect(_suppSettBttn, &Hix::QML::Controls::Button::clicked, [this]() {
		applySupportSettings();
		});
	QObject::connect(_raftSettBttn, &Hix::QML::Controls::Button::clicked, [this]() {
		applySupportSettings();
		});
	QObject::connect(_reconnectBttn, &Hix::QML::Controls::Button::clicked, [this]() {
		interconnectSupports();
		});
	QObject::connect(_clearSupportsBttn, &Hix::QML::Controls::Button::clicked, [this]() {
		clearSupport(Hix::Application::ApplicationManager::getInstance().partManager().selectedModels());
		});
	QObject::connect(_manualEditBttn, &Hix::QML::Controls::ToggleSwitch::checkedChanged, [this]() {
		auto mode = _manualEditBttn->isChecked() ? Hix::Support::EditMode::Manual : Hix::Support::EditMode::None;
		Hix::Application::ApplicationManager::getInstance().supportRaftManager().setSupportEditMode(mode);
		});


	// bind inputs
	auto& modSettings = Hix::Application::SettingsChanger::settings(Hix::Application::ApplicationManager::getInstance()).supportSetting;

	QObject::connect(_suppTypeDrop, &Hix::QML::Controls::DropdownBox::indexChanged, [this, &modSettings]() {
		_suppTypeDrop->getSelected(modSettings.supportType);
		});
	QObject::connect(_raftTypeDrop, &Hix::QML::Controls::DropdownBox::indexChanged, [this, &modSettings]() {
		_raftTypeDrop->getSelected(modSettings.raftType);
		});
	QObject::connect(_interconnectTypeDrop, &Hix::QML::Controls::DropdownBox::indexChanged, [this, &modSettings]() {
		_interconnectTypeDrop->getSelected(modSettings.interconnectType);
		});
	QObject::connect(_supportBaseHeightSpin, &Hix::QML::Controls::InputSpinBox::valueChanged, [this, &modSettings]() {
		modSettings.supportBaseHeight = _supportBaseHeightSpin->getValue();
		});

	QObject::connect(_thickenFeetBttn, &Hix::QML::Controls::ToggleSwitch::checkedChanged, [this, &modSettings]() {
		modSettings.thickenFeet = _thickenFeetBttn->isChecked();
		});


}

Hix::Features::SupportMode::~SupportMode()
{
	Hix::Application::ApplicationManager::getInstance().getRayCaster().setHoverEnabled(false);
}


void Hix::Features::SupportMode::applySupportSettings()
{
	_suppDensitySpin->updateValue();
	_maxRadSpin->updateValue();
	_minRadSpin->updateValue();
	_raftRadiusMultSpin->updateValue();
	_raftMinMaxRatioSpin->updateValue();
	_raftThickness->updateValue();
	_supportBaseHeightSpin->updateValue();
	_maxConnectDistanceSpin->updateValue();

#ifdef _DEBUG
	qDebug() << _suppDensitySpin->getValue();
	qDebug() << _maxRadSpin->getValue();
	qDebug() << _minRadSpin->getValue();
	qDebug() << _raftRadiusMultSpin->getValue();
	qDebug() << _raftMinMaxRatioSpin->getValue();
	qDebug() << _raftThickness->getValue();
	qDebug() << _supportBaseHeightSpin->getValue();
	qDebug() << _maxConnectDistanceSpin->getValue();
#endif

	auto& modSettings = Hix::Application::SettingsChanger::settings(Hix::Application::ApplicationManager::getInstance()).supportSetting;
	modSettings.supportDensity = _suppDensitySpin->getValue();
	modSettings.supportRadiusMax = _maxRadSpin->getValue();
	modSettings.supportRadiusMin = _minRadSpin->getValue();
	modSettings.raftRadiusMult = _raftRadiusMultSpin->getValue();
	modSettings.raftMinMaxRatio = _raftMinMaxRatioSpin->getValue();
	modSettings.raftThickness = _raftThickness->getValue();
	modSettings.supportBaseHeight = _supportBaseHeightSpin->getValue();
	modSettings.maxConnectDistance = _maxConnectDistanceSpin->getValue();
	modSettings.writeJSON();
}

void Hix::Features::SupportMode::faceSelected(GLModel* selected, const Hix::Engine3D::FaceConstItr& selectedFace,
	const Hix::Input::MouseEventData& mouse, const Qt3DRender::QRayCasterHit& hit)
{
	auto editMode = Hix::Application::ApplicationManager::getInstance().supportRaftManager().supportEditMode();
	auto suppType = Hix::Application::ApplicationManager::getInstance().settings().supportSetting.supportType;
	if (editMode == Hix::Support::EditMode::Manual && suppType != Hix::Settings::SupportSetting::SupportType::None) {
		applySupportSettings();
		Hix::OverhangDetect::Overhang newOverhang(selectedFace, selected->ptToRoot(hit.localIntersection()));
		Hix::Application::ApplicationManager::getInstance().taskManager().enqueTask(new ManualSupport(newOverhang));
	}
}

void Hix::Features::SupportMode::generateAutoSupport(std::unordered_set<GLModel*> models)
{ 
	applySupportSettings();

	if (Hix::Application::ApplicationManager::getInstance().settings().supportSetting.supportType != Hix::Settings::SupportSetting::SupportType::None)
	{
		Hix::Features::FeatureContainer* container = new FeatureContainer();
		
		if (ApplicationManager::getInstance().supportRaftManager().supportActive())
		{
			for (auto each : Hix::Application::ApplicationManager::getInstance().supportRaftManager().modelAttachedSupports(models))
				container->addFeature(new RemoveSupport(each));

			for (auto model : models)
				container->addFeature(new Move::ZToBed(model));
		}
		
		for (auto selectedModel : models)
		{
			container->addFeature(new AutoSupport(selectedModel));
		}
		Hix::Application::ApplicationManager::getInstance().taskManager().enqueTask(container);
	}
}

void Hix::Features::SupportMode::clearSupport(const std::unordered_set<GLModel*> models)
{
	auto& srMan = Hix::Application::ApplicationManager::getInstance().supportRaftManager();
	if (srMan.supportsEmpty())
		return;

	std::unordered_set<const GLModel*> cModels;
	for (auto e : models) cModels.insert(e);

	Hix::Features::FeatureContainer* container = new FeatureContainer();

	if (srMan.raftActive())
		container->addFeature(new RemoveRaft());

	auto prevConns = srMan.interconnects();
	for (auto each : prevConns)
	{
		if (dynamic_cast<Interconnect*>(each)->isConnectedTo(cModels))
		{
		container->addFeature(new RemoveSupport(each));
		}
	}
	for (auto each : srMan.modelAttachedSupports(models))
		container->addFeature(new RemoveSupport(each));

	for (auto model : models)
		container->addFeature(new Move::ZToBed(model));

	Hix::Application::ApplicationManager::getInstance().taskManager().enqueTask(container);
}


void Hix::Features::SupportMode::interconnectSupports()
{
	applySupportSettings();
	auto& srMan = Hix::Application::ApplicationManager::getInstance().supportRaftManager();

	Hix::Features::FeatureContainer* container = new FeatureContainer();
	if (srMan.supportsEmpty())
	{
		return;
	}
	auto prevConns = srMan.interconnects();
	for (auto each : prevConns)
	{
		container->addFeature(new RemoveSupport(each));
	}
	//add raycaster
	srMan.prepareRaycasterSelected();

	for (auto connPair : srMan.interconnectPairs())
	{
		container->addFeature(new GenerateInterconnections(connPair));
	}
	Hix::Application::ApplicationManager::getInstance().taskManager().enqueTask(container);

}

void Hix::Features::SupportMode::regenerateRaft()
{
	applySupportSettings();

	Hix::Features::FeatureContainer* container = new FeatureContainer();
	if (ApplicationManager::getInstance().supportRaftManager().raftActive())
	{
		//delete
		container->addFeature(removeRaft());
	}
	//add
	container->addFeature(generateRaft());
	Hix::Application::ApplicationManager::getInstance().taskManager().enqueTask(container);

}

void Hix::Features::SupportMode::applyButtonClicked()
{
}


Hix::Features::Feature* Hix::Features::SupportMode::generateRaft()
{
	return new AddRaft();
}

Hix::Features::Feature* Hix::Features::SupportMode::removeRaft()
{
	return new RemoveRaft();
}

void Hix::Features::SupportMode::removeSupport(SupportModel* target)
{
	Hix::Application::ApplicationManager::getInstance().taskManager().enqueTask(new RemoveSupport(target));
}

