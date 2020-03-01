#include "SupportFeature.h"
#include "../../input/raycastcontroller.h"
#include "render/Color.h"
#include "feature/move.h"
#include "support/RaftModel.h"
#include "application/ApplicationManager.h"
#include "../../glmodel.h"

using namespace Hix::Features;
using namespace Hix::Application;
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


	//Hix::QML::Controls::Button* _generateSupportsBttn;
	//Hix::QML::Controls::Button* _clearSupportsBttn;
	//Hix::QML::Controls::ToggleSwitch* _manualEditBttn;

	//Hix::QML::Controls::DropdownBox* _suppTypeBttn;
	//Hix::QML::Controls::DropdownBox* _suppDensityBttn;
	//Hix::QML::Controls::DropdownBox* _maxRadBttn;
	//Hix::QML::Controls::DropdownBox* _minRadBttn;
	//Hix::QML::Controls::DropdownBox* _minRaftTypeBttn;
}

Hix::Features::SupportMode::~SupportMode()
{
	Hix::Application::ApplicationManager::getInstance().getRayCaster().setHoverEnabled(false);
}

void Hix::Features::SupportMode::faceSelected(GLModel* selected, const Hix::Engine3D::FaceConstItr& selectedFace,
	const Hix::Input::MouseEventData& mouse, const Qt3DRender::QRayCasterHit& hit)
{
	auto suppMode = Hix::Application::ApplicationManager::getInstance().supportRaftManager().supportEditMode();
	if (suppMode == Hix::Support::EditMode::Manual) {
		Hix::OverhangDetect::Overhang newOverhang(selectedFace, selected->ptToRoot(hit.localIntersection()));
		Hix::Application::ApplicationManager::getInstance().taskManager().enqueTask(new AddSupport(newOverhang));
	}
}

Hix::Features::FeatureContainer* Hix::Features::SupportMode::generateAutoSupport(std::unordered_set<GLModel*>& models)
{

	if (!Hix::Application::ApplicationManager::getInstance().supportRaftManager().supportsEmpty())
		return nullptr;

	//Hix::Application::ApplicationManager::getInstance().supportRaftManager().setSupportType(scfg->support_type);
	Hix::Features::FeatureContainer* container = new FeatureContainer();

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

	return container;
}

Hix::Features::FeatureContainer* Hix::Features::SupportMode::clearSupport(const std::unordered_set<GLModel*>& models)
{
	if (Hix::Application::ApplicationManager::getInstance().supportRaftManager().supportsEmpty())
		return nullptr;

	Hix::Features::FeatureContainer* container = new FeatureContainer();
	if (Hix::Application::ApplicationManager::getInstance().supportRaftManager().raftActive())
		container->addFeature(new RemoveRaft());

	for (auto each : Hix::Application::ApplicationManager::getInstance().supportRaftManager().modelAttachedSupports(models))
		container->addFeature(new RemoveSupport(each));

	for (auto model : models)
		container->addFeature(new Move(model, QVector3D(0, 0, -Hix::Support::SupportRaftManager::supportRaftMinLength())));

	return container;
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

