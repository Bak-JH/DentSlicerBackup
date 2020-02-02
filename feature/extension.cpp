#include "extension.h"
#include "../qml/components/Inputs.h"
#include "../qml/components/ControlOwner.h"
#include "../qmlmanager.h"
#include "render/ModelMaterial.h"
#include "render/Color.h"
#include "glmodel.h"


using namespace Hix::Debug;
const QUrl EXTEND_POPUP_URL = QUrl("qrc:/Qml/FeaturePopup/PopupExtend.qml");
Hix::Features::ExtendMode::ExtendMode() 
	:PPShaderMode(qmlManager->getSelectedModels()), DialogedMode(EXTEND_POPUP_URL)
{
	auto& co = controlOwner();
	co.getControl(_extendValue, "extendvalue");
}
std::deque<HalfEdgeConstItr> boundaryPath(const std::unordered_set<FaceConstItr>& faces)
{
	std::unordered_map<VertexConstItr, HalfEdgeConstItr> edgeMap;
	std::deque<HalfEdgeConstItr> path;
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
		return path;
	path.emplace_back(edgeMap.begin()->second);
	edgeMap.erase(edgeMap.begin());
	while (!edgeMap.empty())
	{

		auto to = path.back().to();
		auto next = edgeMap.at(to);
		path.emplace_back(next);
		edgeMap.erase(to);

	}
	return path;
}

void extendAlongOutline(Mesh* mesh, QVector3D normal, double distance, const std::deque<HalfEdgeConstItr>& path) {

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

void Hix::Features::ExtendMode::apply()
{
	if (_args.empty())
		return;

	auto distance = _extendValue->getValue();
	Hix::Features::FeatureContainerFlushSupport* container = new FeatureContainerFlushSupport();
	for (auto& each : _args)
	{
		auto& arg = each.second;
		container->addFeature(new Extend(each.first, arg.normal, arg.extensionFaces, distance));
	}
	_args.clear();
	qmlManager->taskManager().enqueTask(container);
}





Hix::Features::Extend::Extend(GLModel* targetModel, const QVector3D& targetFaceNormal,
								const std::unordered_set<FaceConstItr>& targetFaces, double distance)
	: _model(targetModel), _normal(targetFaceNormal), _extensionFaces(targetFaces), _distance(distance)
{

}

Hix::Features::Extend::~Extend()
{
	_model = nullptr;
	_prevMesh = nullptr;
	_nextMesh = nullptr;

	delete _model;
	delete _prevMesh;
	delete _nextMesh;
}

void Hix::Features::Extend::undoImpl()
{
	_model->setMesh(_prevMesh);
	_model->unselectMeshFaces();
	_model->updateMesh(true);
	_model->setZToBed();
}

void Hix::Features::Extend::redoImpl()
{
	_model->setMesh(_nextMesh);
	_model->unselectMeshFaces();
	_model->updateMesh(true);
	_model->setZToBed();
}

void Hix::Features::Extend::runImpl()
{
	_prevMesh = new Mesh(*_model->getMeshModd());
	_model->unselectMeshFaces();
	std::deque<HalfEdgeConstItr> path;
	path = boundaryPath(_extensionFaces);
	extendAlongOutline(_model->getMeshModd(), _normal, _distance, path);
	coverCap(_model, _normal, _extensionFaces, _distance);

	_model->getMeshModd()->removeFaces(_extensionFaces);
	_model->unselectMeshFaces();
	_model->updateMesh();

	_nextMesh = _model->getMeshModd();

	_model->setZToBed();
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
