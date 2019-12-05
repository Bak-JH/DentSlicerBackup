#include "extension.h"
#include "render/ModelMaterial.h"
#include "render/Color.h"
#include "glmodel.h"

using namespace Hix::Debug;

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


Hix::Features::Extend::Extend(const std::unordered_set<GLModel*>& selectedModels):Feature(selectedModels)
{
}

Hix::Features::Extend::~Extend()
{
}

void Hix::Features::Extend::faceSelected(GLModel* selected, const Hix::Engine3D::FaceConstItr& selectedFace, const Hix::Input::MouseEventData& mouse, const Qt3DRender::QRayCasterHit& hit)
{
	_model = selected;
	_normal = selectedFace.localFn();
	_extensionFaces = selected->getMesh()->findNearSimilarFaces(_normal, selectedFace);
	PPShaderFeature::colorFaces(selected, _extensionFaces);
}

void Hix::Features::Extend::extendMesh(double distance){

	if (_model)
	{
		std::deque<HalfEdgeConstItr> path;
		try
		{
			path = boundaryPath(_extensionFaces);
		}
		catch (...)
		{
			return;
		}
		extendAlongOutline(_model->getMeshModd(), _normal, distance, path);
		coverCap(_model, _normal, _extensionFaces, distance);
		_model->getMeshModd()->removeFaces(_extensionFaces);
		_model->updateMesh();
		_model->setZToBed();
	}
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
