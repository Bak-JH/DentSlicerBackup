#include "SupportFeature.h"
#include "../../input/raycastcontroller.h"
#include "render/Color.h"
#include "../../qmlmanager.h"

/////////////////////
///  Add Support  ///
/////////////////////
Hix::Features::AddSupport::AddSupport(const Hix::Engine3D::FaceConstItr face, QVector3D point)
{
	qDebug() << "AddSupport";
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

void Hix::Features::AddSupport::undo()
{
	qDebug() << "AddSupport undo called";
	qmlManager->supportRaftManager().removeSupport(_addedModel);
}



////////////////////////
///  Remove Support  ///
////////////////////////
Hix::Features::RemoveSupport::RemoveSupport(SupportModel* target)
{
	_removedModel = std::move(qmlManager->supportRaftManager().removeSupport(target));
}

Hix::Features::RemoveSupport::RemoveSupport(std::unique_ptr<SupportModel>&& target)
{
	_removedModel = std::move(qmlManager->supportRaftManager().removeSupport(target.release()));
}

Hix::Features::RemoveSupport::~RemoveSupport()
{
}

void Hix::Features::RemoveSupport::undo()
{
	_removedModel.get()->setEnabled(true);
	_removedModel.get()->setHitTestable(true);
	qmlManager->supportRaftManager().supports().insert(std::make_pair(_removedModel.get(), std::move(_removedModel)));
	qDebug() << "RemoveSupport undo called";
}



////////////////////
/// Support Mode ///
////////////////////
Hix::Features::SupportMode::SupportMode(const std::unordered_set<GLModel*>& selectedModels, QEntity* parent)
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
		qmlManager->addToHistory(new AddSupport(selectedFace, selected->ptToRoot(hit.localIntersection())));
	}
}

std::unique_ptr<Hix::Features::FeatureContainer> Hix::Features::SupportMode::generateAutoSupport()
{
	qmlManager->supportRaftManager().setSupportType(scfg->support_type);
	std::unique_ptr<Hix::Features::FeatureContainer> container = std::make_unique<FeatureContainer>();
	
	for (auto selectedModel : _targetModels)
	{
		if (scfg->support_type != SlicingConfiguration::SupportType::None)
		{
			selectedModel->setZToBed();
			selectedModel->moveModel(QVector3D(0, 0, Hix::Support::SupportRaftManager::supportRaftMinLength()));
		}
		auto overhangs = qmlManager->supportRaftManager().detectOverhang(*selectedModel);
		for (auto overhang : overhangs)
		{
			container->addFeature(new AddSupport(overhang));
		}
	}

	return container;
}

std::unique_ptr<Hix::Features::FeatureContainer> Hix::Features::SupportMode::clearSupport()
{
	std::unique_ptr<Hix::Features::FeatureContainer> container = std::make_unique<FeatureContainer>();
	std::unordered_set<const GLModel*> models;

	for (auto model : _targetModels)
	{
		model->getChildrenModels(models);
		models.insert(model);
	}
	
	auto& supps = qmlManager->supportRaftManager().supports();
	for (auto curr = supps.begin(); curr != supps.end();)
	{
		auto deleted = curr;
		++curr;
		auto attachedSupport = dynamic_cast<ModelAttachedSupport*>(deleted->first);
		if (attachedSupport)
		{
			auto ptr = &attachedSupport->getAttachedModel();
			if (models.find(ptr) != models.end())
				container->addFeature(new RemoveSupport(std::move(deleted->second)));
		}
	}

	for (auto model : _targetModels)
		model->setZToBed();

	if (qmlManager->supportRaftManager().supports().size() == 0)
		qmlManager->supportRaftManager().clear();

	return container;
}

void Hix::Features::SupportMode::removeSupport(SupportModel* target)
{
	qmlManager->addToHistory(new RemoveSupport(target));
}
