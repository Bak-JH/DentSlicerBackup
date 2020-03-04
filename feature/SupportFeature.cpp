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
Hix::Features::AddSupport::AddSupport(const OverhangDetect::Overhang& overhang):_overhang(overhang)
{

}

Hix::Features::AddSupport::~AddSupport()
{}

void Hix::Features::AddSupport::undoImpl()
{
	_model = Hix::Application::ApplicationManager::getInstance().supportRaftManager().removeSupport(std::get<SupportModel*>(_model));
}

void Hix::Features::AddSupport::redoImpl()
{
	_model = Hix::Application::ApplicationManager::getInstance().supportRaftManager().addSupport(std::move(std::get<std::unique_ptr<SupportModel>>(_model)));
}

void Hix::Features::AddSupport::runImpl()
{
	auto newModel = Hix::Application::ApplicationManager::getInstance().supportRaftManager().addSupport(_overhang);
	_model = newModel;
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
	_model = Hix::Application::ApplicationManager::getInstance().supportRaftManager().addSupport(std::move(std::get<std::unique_ptr<SupportModel>>(_model)));
}

void Hix::Features::RemoveSupport::redoImpl()
{
	_model = Hix::Application::ApplicationManager::getInstance().supportRaftManager().removeSupport(std::get<SupportModel*>(_model));
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
	_model = Hix::Application::ApplicationManager::getInstance().supportRaftManager().removeRaft();
}

void Hix::Features::AddRaft::redoImpl()
{
	_model = Hix::Application::ApplicationManager::getInstance().supportRaftManager().addRaft(std::move(std::get<std::unique_ptr<RaftModel>>(_model)));
}

void Hix::Features::AddRaft::runImpl()
{
	_model = Hix::Application::ApplicationManager::getInstance().supportRaftManager().generateRaft();

}



/////////////////////
///  Remove Raft  ///
/////////////////////
Hix::Features::RemoveRaft::RemoveRaft()
{}

Hix::Features::RemoveRaft::~RemoveRaft()
{}


void Hix::Features::RemoveRaft::undoImpl()
{
	Hix::Application::ApplicationManager::getInstance().supportRaftManager().addRaft(std::move(std::get<std::unique_ptr<RaftModel>>(_model)));
}

void Hix::Features::RemoveRaft::redoImpl()
{
	_model = Hix::Application::ApplicationManager::getInstance().supportRaftManager().removeRaft();
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
	Hix::Application::ApplicationManager::getInstance().getRayCaster().setHoverEnabled(true);

	auto& co = controlOwner();
	co.getControl(_generateSupportsBttn, "generatesupports");
	co.getControl(_clearSupportsBttn, "clearsupports");
	co.getControl(_manualEditBttn, "editsupports");
	co.getControl(_suppTypeDrop, "supporttype");
	co.getControl(_raftTypeDrop, "rafttype");
	co.getControl(_suppDensitySpin, "supportdensity");
	co.getControl(_maxRadSpin, "maxradius");
	co.getControl(_minRadSpin, "minradius");


	auto& settings = Hix::Application::ApplicationManager::getInstance().settings().supportSetting;

	_suppTypeDrop->setEnums<SupportSetting::SupportType>(settings.supportType);
	_raftTypeDrop->setEnums<SupportSetting::RaftType>(settings.raftType);



	// bind buttons
	QObject::connect(_generateSupportsBttn, &Hix::QML::Controls::Button::clicked, [this]() {
			generateAutoSupport(Hix::Application::ApplicationManager::getInstance().partManager().selectedModels());
		});
	QObject::connect(_clearSupportsBttn, &Hix::QML::Controls::Button::clicked, [this]() {
			clearSupport(Hix::Application::ApplicationManager::getInstance().partManager().selectedModels());
		});
	QObject::connect(_manualEditBttn, &Hix::QML::Controls::ToggleSwitch::checkedChanged, [this]() {
			auto mode = _manualEditBttn->isChecked() ? Hix::Support::EditMode::Manual : Hix::Support::EditMode::None;
			Hix::Application::ApplicationManager::getInstance().supportRaftManager().setSupportEditMode(mode);
		});

	// bind dropbox
	auto& modSettings = Hix::Application::SettingsChanger::settings(Hix::Application::ApplicationManager::getInstance()).supportSetting;
	QObject::connect(_suppTypeDrop, &Hix::QML::Controls::DropdownBox::indexChanged, [this, &modSettings]() {
		_suppTypeDrop->getSelected(modSettings.supportType);
		});
	QObject::connect(_raftTypeDrop, &Hix::QML::Controls::DropdownBox::indexChanged, [this, &modSettings]() {
		_raftTypeDrop->getSelected(modSettings.raftType);
		});
}

Hix::Features::SupportMode::~SupportMode()
{
	Hix::Application::ApplicationManager::getInstance().getRayCaster().setHoverEnabled(false);
}


void Hix::Features::SupportMode::applySupportSettings()
{
	auto& modSettings = Hix::Application::SettingsChanger::settings(Hix::Application::ApplicationManager::getInstance()).supportSetting;
	_suppTypeDrop->getSelected(modSettings.supportType);
	_raftTypeDrop->getSelected(modSettings.raftType);
	modSettings.supportDensity = _suppDensitySpin->getValue();
	modSettings.supportRadiusMax = _maxRadSpin->getValue();
	modSettings.supportRadiusMin = _minRadSpin->getValue();
}

void Hix::Features::SupportMode::faceSelected(GLModel* selected, const Hix::Engine3D::FaceConstItr& selectedFace,
	const Hix::Input::MouseEventData& mouse, const Qt3DRender::QRayCasterHit& hit)
{
	auto suppMode = Hix::Application::ApplicationManager::getInstance().supportRaftManager().supportEditMode();
	if (suppMode == Hix::Support::EditMode::Manual) {
		applySupportSettings();
		Hix::OverhangDetect::Overhang newOverhang(selectedFace, selected->ptToRoot(hit.localIntersection()));
		Hix::Application::ApplicationManager::getInstance().taskManager().enqueTask(new AddSupport(newOverhang));
	}
}

void Hix::Features::SupportMode::generateAutoSupport(std::unordered_set<GLModel*> models)
{ 

	if (!Hix::Application::ApplicationManager::getInstance().supportRaftManager().supportsEmpty())
		return;

	//Hix::Application::ApplicationManager::getInstance().supportRaftManager().setSupportType(scfg->support_type);
	Hix::Features::FeatureContainer* container = new FeatureContainer();
	applySupportSettings();

	for (auto selectedModel : models)
	{
		if (Hix::Application::ApplicationManager::getInstance().settings().supportSetting.supportType != Hix::Settings::SupportSetting::SupportType::None)
		{
			container->addFeature(new Move(selectedModel, QVector3D(0, 0, Hix::Support::SupportRaftManager::supportRaftMinLength())));
		}
		auto overhangs = Hix::Application::ApplicationManager::getInstance().supportRaftManager().detectOverhang(*selectedModel);
		for (auto overhang : overhangs)
		{
			container->addFeature(new AddSupport(overhang));
		}
	}

	Hix::Application::ApplicationManager::getInstance().taskManager().enqueTask(container);
}

void Hix::Features::SupportMode::clearSupport(const std::unordered_set<GLModel*> models)
{
	if (Hix::Application::ApplicationManager::getInstance().supportRaftManager().supportsEmpty())
		return;

	Hix::Features::FeatureContainer* container = new FeatureContainer();
	applySupportSettings();

	if (Hix::Application::ApplicationManager::getInstance().supportRaftManager().raftActive())
		container->addFeature(new RemoveRaft());

	for (auto each : Hix::Application::ApplicationManager::getInstance().supportRaftManager().modelAttachedSupports(models))
		container->addFeature(new RemoveSupport(each));

	for (auto model : models)
		container->addFeature(new Move(model, QVector3D(0, 0, -Hix::Support::SupportRaftManager::supportRaftMinLength())));

	Hix::Application::ApplicationManager::getInstance().taskManager().enqueTask(container);
}

void Hix::Features::SupportMode::regenerateRaft()
{
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
	//do nothing
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

