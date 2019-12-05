#ifdef _DEBUG

#include "DebugRenderObject.h"
#include "../../glmodel.h"
using namespace Qt3DCore;

void Hix::Debug::DebugRenderObject::initialize(QEntity* root)
{
	_root = root;
}

void Hix::Debug::DebugRenderObject::addLine(const std::vector<QVector3D>& vertices, const QVector4D& color)
{
	_lines.emplace_back(vertices, _root, color);
}

void Hix::Debug::DebugRenderObject::addPaths(const std::vector<std::vector<QVector3D>>& paths, const QVector4D& color)
{
	_lines.emplace_back(paths, _root, color);

}

void Hix::Debug::DebugRenderObject::outlineFace(const Hix::Engine3D::FaceConstItr& face, const QVector4D& color)
{
	std::vector<QVector3D> points;
	auto mvs = face.meshVertices();
	for (auto& each : mvs)
	{
		points.emplace_back(each.worldPosition());
	}
	addLine(points, color);
}

void Hix::Debug::DebugRenderObject::registerDebugColorFaces(GLModel* owner, const std::unordered_set<FaceConstItr>& faces)
{
	_debugFaceMap.insert(std::make_pair(owner, faces));
}

void Hix::Debug::DebugRenderObject::colorDebugFaces()
{
	for (auto& each : _debugFaceMap)
	{
		auto model = each.first;
		model->setMaterialMode(Hix::Render::ShaderMode::PerPrimitiveColor);
		for (auto& face : each.second)
		{
			model->selectedFaces.insert(face);

		}
		model->updateMesh(true);
	}
}

void Hix::Debug::DebugRenderObject::clear()
{
	_planes.clear();
	_lines.clear();
	_debugFaceMap.clear();
}

Hix::Render::PlaneMeshEntity& Hix::Debug::DebugRenderObject::displayPlane(const Hix::Plane3D::PDPlane& pdplane)
{
	auto& plane = _planes.emplace_back(_root);
	plane.setPointNormal(pdplane);
	return plane;
}





























#else
#endif
