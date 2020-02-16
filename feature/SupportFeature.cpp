#include "SupportFeature.h"
#include "../../input/raycastcontroller.h"
#include "render/Color.h"
#include "../../qmlmanager.h"
#include "feature/move.h"
#include "support/RaftModel.h"
#include "application/ApplicationManager.h"

/////////////////////
///  Add Support  ///
/////////////////////
Hix::Features::AddSupport::AddSupport(const OverhangDetect::Overhang& overhang):_overhang(overhang)
{}

Hix::Features::AddSupport::~AddSupport()
{}

void Hix::Features::AddSupport::undoImpl()
{
	_model = qmlManager->supportRaftManager().removeSupport(std::get<SupportModel*>(_model));
}

void Hix::Features::AddSupport::redoImpl()
{
	_model = qmlManager->supportRaftManager().addSupport(std::move(std::get<std::unique_ptr<SupportModel>>(_model)));
}

void Hix::Features::AddSupport::runImpl()
{
	auto newModel = qmlManager->supportRaftManager().addSupport(_overhang);
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
	_model = qmlManager->supportRaftManager().addSupport(std::move(std::get<std::unique_ptr<SupportModel>>(_model)));
}

void Hix::Features::RemoveSupport::redoImpl()
{
	_model = qmlManager->supportRaftManager().removeSupport(std::get<SupportModel*>(_model));
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
	_model = qmlManager->supportRaftManager().removeRaft();
}

void Hix::Features::AddRaft::redoImpl()
{
	_model = qmlManager->supportRaftManager().addRaft(std::move(std::get<std::unique_ptr<RaftModel>>(_model)));
}

void Hix::Features::AddRaft::runImpl()
{
	_model = qmlManager->supportRaftManager().generateRaft();

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
	qmlManager->supportRaftManager().addRaft(std::move(std::get<std::unique_ptr<RaftModel>>(_model)));
}

void Hix::Features::RemoveRaft::redoImpl()
{
	_model = qmlManager->supportRaftManager().removeRaft();
}

void Hix::Features::RemoveRaft::runImpl()
{
	redoImpl();
}



////////////////////
/// Support Mode ///
////////////////////
Hix::Features::SupportMode::SupportMode()
	: _targetModels(Hix::Application::ApplicationManager::getInstance().partManager().selectedModels())
{
	qmlManager->getRayCaster().setHoverEnabled(true);
}

Hix::Features::SupportMode::~SupportMode()
{
	qmlManager->getRayCaster().setHoverEnabled(false);
}

void Hix::Features::SupportMode::faceSelected(GLModel* selected, const Hix::Engine3D::FaceConstItr& selectedFace,
	const Hix::Input::MouseEventData& mouse, const Qt3DRender::QRayCasterHit& hit)
{
	auto suppMode = qmlManager->supportRaftManager().supportEditMode();
	if (suppMode == Hix::Support::EditMode::Manual) {
		Hix::OverhangDetect::Overhang newOverhang(selectedFace, selected->ptToRoot(hit.localIntersection()));
		qmlManager->taskManager().enqueTask(new AddSupport(newOverhang));
	}
}

Hix::Features::FeatureContainer* Hix::Features::SupportMode::generateAutoSupport(std::unordered_set<GLModel*>& models)
{

	if (!qmlManager->supportRaftManager().supportsEmpty())
		return nullptr;

	qmlManager->supportRaftManager().setSupportType(scfg->support_type);
	Hix::Features::FeatureContainer* container = new FeatureContainer();

	for (auto selectedModel : models)
	{
		if (scfg->support_type != SlicingConfiguration::SupportType::None)
		{
			container->addFeature(new Move(selectedModel, QVector3D(0, 0, Hix::Support::SupportRaftManager::supportRaftMinLength())));
		}
		auto overhangs = qmlManager->supportRaftManager().detectOverhang(*selectedModel);
		for (auto overhang : overhangs)
		{
			container->addFeature(new AddSupport(overhang));
		}
	}

	return container;
}

Hix::Features::FeatureContainer* Hix::Features::SupportMode::clearSupport(const std::unordered_set<GLModel*>& models)
{
	if (qmlManager->supportRaftManager().supportsEmpty())
		return nullptr;

	Hix::Features::FeatureContainer* container = new FeatureContainer();
	if (qmlManager->supportRaftManager().raftActive())
		container->addFeature(new RemoveRaft());

	for (auto each : qmlManager->supportRaftManager().modelAttachedSupports(models))
		container->addFeature(new RemoveSupport(each));

	for (auto model : models)
		container->addFeature(new Move(model, QVector3D(0, 0, -Hix::Support::SupportRaftManager::supportRaftMinLength())));

	return container;
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
	qmlManager->taskManager().enqueTask(new RemoveSupport(target));
}

