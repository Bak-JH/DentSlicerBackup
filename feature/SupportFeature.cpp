#include "SupportFeature.h"
#include "../../input/raycastcontroller.h"
#include "render/Color.h"
#include "../../qmlmanager.h"
#include "feature/move.h"
#include "support/RaftModel.h"

/////////////////////
///  Add Support  ///
/////////////////////
Hix::Features::AddSupport::AddSupport(const Hix::Engine3D::FaceConstItr face, QVector3D point)
{
	Hix::OverhangDetect::Overhang newOverhang(face, point);
	_addedModel = qmlManager->supportRaftManager().addSupport(newOverhang);
	_addedModel->setHitTestable(true);
}

Hix::Features::AddSupport::AddSupport(const OverhangDetect::Overhang& overhang)
{
	_addedModel = qmlManager->supportRaftManager().addSupport(overhang);
}

Hix::Features::AddSupport::~AddSupport()
{
}

void Hix::Features::AddSupport::undoImpl()
{
	_removedModel = qmlManager->supportRaftManager().removeSupport(_addedModel).release();
}

void Hix::Features::AddSupport::redoImpl()
{
	_removedModel->setEnabled(true);
	_removedModel->setHitTestable(true);
	_addedModel = qmlManager->supportRaftManager().addSupport(std::unique_ptr<SupportModel>(_removedModel));
}



////////////////////////
///  Remove Support  ///
////////////////////////
Hix::Features::RemoveSupport::RemoveSupport(SupportModel* target)
{
	_removedModel = qmlManager->supportRaftManager().removeSupport(target).release();
}

Hix::Features::RemoveSupport::~RemoveSupport()
{
}

void Hix::Features::RemoveSupport::undoImpl()
{
	_removedModel->setEnabled(true);
	_removedModel->setHitTestable(true);
	_addedModel = qmlManager->supportRaftManager().addSupport(std::unique_ptr<SupportModel>(_removedModel));
}

void Hix::Features::RemoveSupport::redoImpl()
{
	_removedModel = qmlManager->supportRaftManager().removeSupport(_addedModel).release();
}



//////////////////
///  Add Raft  ///
//////////////////
Hix::Features::AddRaft::AddRaft()
{
	_addedRaft = qmlManager->supportRaftManager().generateRaft();
}

Hix::Features::AddRaft::~AddRaft()
{
	//delete _addedRaft;
	//delete _deletedRaft;
}

void Hix::Features::AddRaft::undoImpl()
{
	_deletedRaft = _addedRaft;
	_addedRaft->setParent((QNode*)nullptr);
	_deletedRaft = qmlManager->supportRaftManager().removeRaft();
}

void Hix::Features::AddRaft::redoImpl()
{
	_deletedRaft->setParent(qmlManager->total);
	qmlManager->supportRaftManager().addRaft(_deletedRaft);
}



/////////////////////
///  Remove Raft  ///
/////////////////////
Hix::Features::RemoveRaft::RemoveRaft()
{
	_deletedRaft = qmlManager->supportRaftManager().removeRaft();
}

Hix::Features::RemoveRaft::~RemoveRaft()
{
	//delete _addedRaft;
	//delete _deletedRaft;
}


void Hix::Features::RemoveRaft::undoImpl()
{
	_addedRaft = _deletedRaft;
	_deletedRaft->setParent(qmlManager->total);
	qmlManager->supportRaftManager().addRaft(_deletedRaft);
}

void Hix::Features::RemoveRaft::redoImpl()
{
	_deletedRaft = qmlManager->supportRaftManager().removeRaft();
	_addedRaft->setParent((QNode*)nullptr);
}



////////////////////
/// Support Mode ///
////////////////////
Hix::Features::SupportMode::SupportMode(const std::unordered_set<GLModel*>& selectedModels)
	: _targetModels(selectedModels)
{
}

Hix::Features::SupportMode::~SupportMode()
{

}

void Hix::Features::SupportMode::faceSelected(GLModel* selected, const Hix::Engine3D::FaceConstItr& selectedFace,
	const Hix::Input::MouseEventData& mouse, const Qt3DRender::QRayCasterHit& hit)
{
	auto suppMode = qmlManager->supportRaftManager().supportEditMode();
	if (suppMode == Hix::Support::EditMode::Manual) {
		qmlManager->featureHistoryManager().addFeature(new AddSupport(selectedFace, selected->ptToRoot(hit.localIntersection())));
	}
}

Hix::Features::FeatureContainer* Hix::Features::SupportMode::generateAutoSupport()
{
	if (!qmlManager->supportRaftManager().supportsEmpty())
		return nullptr;

	qmlManager->supportRaftManager().setSupportType(scfg->support_type);
	Hix::Features::FeatureContainer* container = new FeatureContainer();

	for (auto selectedModel : _targetModels)
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

Hix::Features::FeatureContainer* Hix::Features::SupportMode::clearSupport()
{
	Hix::Features::FeatureContainer* container = new FeatureContainer();
	std::unordered_set<const GLModel*> models;
	if(qmlManager->supportRaftManager().raftActive())
		container->addFeature(new RemoveRaft());

	for(auto each : qmlManager->supportRaftManager().modelAttachedSupports(_targetModels))
		container->addFeature(new RemoveSupport(each));

	for (auto model : _targetModels)
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
	qmlManager->featureHistoryManager().addFeature(new RemoveSupport(target));
}

std::unique_ptr<Hix::Features::FeatureContainer> Hix::Features::clearSupport(std::unordered_set<GLModel*>& models)
{
	auto container = std::unique_ptr<Hix::Features::FeatureContainer>();
	for (auto each : qmlManager->supportRaftManager().modelAttachedSupports(models))
	{
		container->addFeature(new RemoveSupport(each));
	}

	for (auto model : models)
		model->setZToBed();

	return container;
}
