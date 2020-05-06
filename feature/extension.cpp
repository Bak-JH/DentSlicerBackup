#include "extension.h"
#include "../qml/components/Inputs.h"
#include "../qml/components/ControlOwner.h"

#include "render/ModelMaterial.h"
#include "render/Color.h"
#include "glmodel.h"
#include "application/ApplicationManager.h"

using namespace Hix::Debug;
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
std::deque<std::deque<HalfEdgeConstItr>> boundaryPath(const std::unordered_set<FaceConstItr>& faces)
{
	std::unordered_map<VertexConstItr, HalfEdgeConstItr> edgeMap;
	std::deque<std::deque<HalfEdgeConstItr>> paths;
	for (auto& f : faces)
	{
		auto e = f.edge();
		for (size_t i = 0; i < 3; ++i, e.moveNext())
		{
			auto nfs = e.nonOwningFaces();
			bool isBoundary = true;
			for (auto& nf : nfs)
			{
				if (faces.find(nf) != faces.cend())
				{
					isBoundary = false;
					break;
				}
			}
			if (isBoundary)
			{
				edgeMap.insert(std::make_pair(e.from(), e));
			}
		}
	}
	if (edgeMap.empty())
		return paths;
	

	while (!edgeMap.empty())
	{
		std::deque<HalfEdgeConstItr> path;
		path.emplace_back(edgeMap.begin()->second);
		edgeMap.erase(edgeMap.begin());
		bool pathIncomplete = true;
		while (pathIncomplete)
		{

			auto to = path.back().to();
			auto found = edgeMap.find(to);
			if (found != edgeMap.end())
			{
				path.emplace_back(found->second);
			}
			else
			{
				paths.emplace_back(std::move(path));
				pathIncomplete = false;
			}
			edgeMap.erase(to);
		}
	}
	return paths;
}

void extendAlongOutline(Mesh* mesh, QVector3D normal, double distance, const std::deque<HalfEdgeConstItr>& path) 
{
	for (auto& e : path)
	{
		auto v0 = e.from().localPosition();
		auto v1 = e.to().localPosition();
		auto e0 = v0 + distance * normal;
		auto e1 = v1 + distance * normal;
		mesh->addFace(e1, v0, v1);
		mesh->addFace(e1, e0, v0);
	}
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

	auto paths = boundaryPath(_extensionFaces);
	for (auto& path : paths)
	{
		extendAlongOutline(mesh, _normal, _distance, path);
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
