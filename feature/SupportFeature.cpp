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

Hix::Features::RemoveSupport::~RemoveSupport()
{
}

void Hix::Features::RemoveSupport::undo()
{
	_removedModel.get()->setEnabled(true);
	_removedModel.get()->setHitTestable(true);
	qmlManager->supportRaftManager().addSupport(std::move(_removedModel));
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
		qmlManager->featureHistoryManager().addFeature(new AddSupport(selectedFace, selected->ptToRoot(hit.localIntersection())));
	}
}

Hix::Features::FeatureContainer* Hix::Features::SupportMode::generateAutoSupport()
{
	qmlManager->supportRaftManager().setSupportType(scfg->support_type);
	Hix::Features::FeatureContainer* container = new FeatureContainer();

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

Hix::Features::FeatureContainer* Hix::Features::SupportMode::clearSupport()
{
	Hix::Features::FeatureContainer* container = new FeatureContainer();
	std::unordered_set<const GLModel*> models;

	for(auto each : qmlManager->supportRaftManager().modelAttachedSupports(_targetModels))
		container->addFeature(new RemoveSupport(each));

	for (auto model : _targetModels)
		model->setZToBed();

	if (qmlManager->supportRaftManager().supportsEmpty())
		qmlManager->supportRaftManager().clear();

	return container;
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

	if (qmlManager->supportRaftManager().supportsEmpty())
		qmlManager->supportRaftManager().clear();

	return container;
}
