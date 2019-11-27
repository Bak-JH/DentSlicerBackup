#include "SupportFeature.h"
#include "../../input/raycastcontroller.h"
#include "render/Color.h"
#include "../../qmlmanager.h"
Hix::Features::SupportFeature::SupportFeature(std::unordered_set<GLModel*>& selectedModels)
{

	_prevSupports = std::unordered_set<Hix::Memory::HetUniquePtr<SupportModel>>(std::move(qmlManager->supportRaftManager().supports()));
	qmlManager->supportRaftManager().supports().clear();
	for (auto selectedModel : selectedModels)
	{
		if (scfg->support_type != SlicingConfiguration::SupportType::None)
		{
			selectedModel->setZToBed();
			selectedModel->moveModel(QVector3D(0, 0, Hix::Support::SupportRaftManager::supportRaftMinLength()));
		}
		qmlManager->supportRaftManager().autoGen(*selectedModel, scfg->support_type);
	}

}

Hix::Features::AddSupport::AddSupport(const Hix::Engine3D::FaceConstItr face, QVector3D point)
{
	qDebug() << "AddSupport";
	Hix::OverhangDetect::Overhang newOverhang(face, point);
	_addedModel = qmlManager->supportRaftManager().addSupport(newOverhang);
}

Hix::Features::AddSupport::~AddSupport()
{
}

void Hix::Features::AddSupport::undo()
{
	qDebug() << "AddSupport undo called";
	qmlManager->supportRaftManager().removeSupport(_addedModel);
}

Hix::Features::SupportFeature::~SupportFeature()
{
}

void Hix::Features::SupportFeature::undo()
{
	qDebug() << "SupportFeature undo called";
	/*for (auto& each : _prevSupports)
	{
		qmlManager->supportRaftManager().supports().emplace(std::move(each.get()));
	}*/
}





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

void Hix::Features::SupportMode::generateAutoSupport()
{
	qmlManager->addToHistory(new SupportFeature(_targetModels));
}

Hix::Features::RemoveSupport::RemoveSupport(SupportModel* target)
{
	qmlManager->supportRaftManager().removeSupport(target);
}

Hix::Features::RemoveSupport::~RemoveSupport()
{
}

void Hix::Features::RemoveSupport::undo()
{
	_removedModel->setHitTestable(true);
	qmlManager->supportRaftManager().supports().emplace(Memory::toUnique(_removedModel));
}
