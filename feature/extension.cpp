#include "extension.h"
#include "../qml/components/Inputs.h"
#include "../qml/components/ControlOwner.h"

#include "render/ModelMaterial.h"
#include "render/Color.h"
#include "glmodel.h"
#include "application/ApplicationManager.h"
#include "../Mesh/FaceSelectionUtils.h"

using namespace Hix::Debug;
using namespace Hix::Engine3D;
const QUrl EXTEND_POPUP_URL = QUrl("qrc:/Qml/FeaturePopup/PopupExtend.qml");

Hix::Features::ExtendMode::ExtendMode() 
	:PPShaderMode(Hix::Application::ApplicationManager::getInstance().partManager().selectedModels()), DialogedMode(EXTEND_POPUP_URL)
{
	if(Hix::Application::ApplicationManager::getInstance().partManager().selectedModels().empty())
	{
		Hix::Application::ApplicationManager::getInstance().modalDialogManager().needToSelectModels();
		return;
	}
	auto& co = controlOwner();
	co.getControl(_extendValue, "extendvalue");
}



Hix::Features::ExtendMode::~ExtendMode()
{
}

void Hix::Features::ExtendMode::faceSelected(GLModel* selected, const Hix::Engine3D::FaceConstItr& selectedFace, const Hix::Input::MouseEventData& mouse, const Qt3DRender::QRayCasterHit& hit)
{
	auto& arg = _args[selected];
	arg.normal = selectedFace.localFn();
	auto& latest = arg.extensionFaces = selected->getMesh()->findNearSimilarFaces(arg.normal, selectedFace);
	PPShaderMode::colorFaces(selected, latest);
}

void Hix::Features::ExtendMode::applyButtonClicked()
{
	if (_args.empty())
		return;

	auto distance = _extendValue->getValue();
	Hix::Features::FeatureContainerFlushSupport* container = new FeatureContainerFlushSupport(Hix::Application::ApplicationManager::getInstance().partManager().selectedModels());
	for (auto& each : _args)
	{
		auto& arg = each.second;
		container->addFeature(new Extend(each.first, arg.normal, arg.extensionFaces, distance));
	}

	_args.clear();
	Hix::Application::ApplicationManager::getInstance().taskManager().enqueTask(container);
}





Hix::Features::Extend::Extend(GLModel* targetModel, const QVector3D& targetFaceNormal,
								const std::unordered_set<FaceConstItr>& targetFaces, double distance)
	: _model(targetModel), _normal(targetFaceNormal), _extensionFaces(targetFaces), _distance(distance)
{
	_progress.setDisplayText("Extending Mesh");
}

Hix::Features::Extend::~Extend()
{
}

void Hix::Features::Extend::undoImpl()
{
	postUIthread([this]() {
		auto temp = _prevMesh.release();
		_prevMesh.reset(_model->getMeshModd());
		_model->setMesh(temp);
		_model->unselectMeshFaces();
		_model->updateMesh(true);
		_model->setZToBed();
	});
}

void Hix::Features::Extend::redoImpl()
{
	postUIthread([this]() {
		auto temp = _prevMesh.release();
		_prevMesh.reset(_model->getMeshModd());
		_model->setMesh(temp);
		_model->unselectMeshFaces();
		_model->updateMesh(true);
		_model->setZToBed();
	});
}

void Hix::Features::Extend::runImpl()
{
	auto *mesh = new Mesh(*_model->getMeshModd());
	_model->unselectMeshFaces();
	_prevMesh.reset(_model->getMeshModd());

	auto paths = Hix::Engine3D::boundaryPath(_extensionFaces);
	for (auto& path : paths)
	{
		auto vtcs = extendAlongOutline(_normal, _distance, path);
		mesh->addFaces(vtcs);
	}
	postUIthread([this, &mesh]() {
		_model->setMesh(mesh);
		coverCap(_model, _normal, _extensionFaces, _distance);

		
		_model->getMeshModd()->removeFaces(_extensionFaces);
		_model->unselectMeshFaces();
		_model->updateMesh();

		_model->setZToBed();
	});
	_extensionFaces.clear();
}

void Hix::Features::Extend::coverCap(GLModel* model, QVector3D normal,const std::unordered_set<FaceConstItr>& extension_faces, double distance){
	auto* mesh = model->getMeshModd();
	auto& aabb = model->aabb();

	for (FaceConstItr mf : extension_faces){
		auto meshVertices = mf.meshVertices();
		mesh->addFace(
				meshVertices[0].localPosition() + distance*normal,
                meshVertices[1].localPosition() + distance*normal,
                meshVertices[2].localPosition() + distance*normal);
		auto added = mesh->getFaces().cend() -1;
		auto addedVtcs = added.meshVertices();
		aabb.update(addedVtcs[0].worldPosition());
		aabb.update(addedVtcs[1].worldPosition());
		aabb.update(addedVtcs[2].worldPosition());

    }
}
