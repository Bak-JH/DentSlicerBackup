#ifdef _DEBUG

#include "DebugRenderObject.h"
#include "../../glmodel.h"
#include "../../render/SceneEntityWithMaterial.h"
#include "render/Bounds3D.h"

using namespace Qt3DCore;
using namespace Hix::Render;
using namespace Hix::Debug;
using namespace Hix::Engine3D;
void Hix::Debug::DebugRenderObject::initialize(QEntity* root)
{
	_root = root;
}

void Hix::Debug::DebugRenderObject::addVertex(const QVector3D vertex, const QColor& color)
{
	_points.emplace_back(vertex, _root, color);
}

void Hix::Debug::DebugRenderObject::addVertices(const std::vector<QVector3D>& vertices, const QColor& color)
{
	_points.emplace_back(vertices, _root, color);
}

void Hix::Debug::DebugRenderObject::addLinePath(const std::vector<QVector3D>& vertices, const QColor& color)
{
	_lines.emplace_back(vertices, _root, color);
}

void Hix::Debug::DebugRenderObject::addLinePaths(const std::vector<std::vector<QVector3D>>& paths, const QColor& color)
{
	_lines.emplace_back(paths, _root, color);

}

void Hix::Debug::DebugRenderObject::outlineFace(const Hix::Engine3D::FaceConstItr& face, const QColor& color)
{
	std::vector<QVector3D> points;
	auto mvs = face.meshVertices();
	for (auto& each : mvs)
	{
		points.emplace_back(each.worldPosition());
	}
	addLinePath(points, color);
}

void Hix::Debug::DebugRenderObject::registerDebugColorFaces(SceneEntityWithMaterial* owner, const std::unordered_set<FaceConstItr>& faces, const QVector4D& color)
{
	_debugFaceMap[owner] = { faces , color};
}

void Hix::Debug::DebugRenderObject::colorDebugFaces()
{
	for (auto& each : _debugFaceMap)
	{
		auto model = each.first;
		model->setMaterialMode(Hix::Render::ShaderMode::PerPrimitiveColor);
		//for (auto& face : each.second)
		//{
		//	model->selectedFaces.insert(face);

		//}
		model->updateMesh(true);
	}
}

void Hix::Debug::DebugRenderObject::showGLModelAabb(SceneEntityWithMaterial* target)
{
	auto minX = target->recursiveAabb().xMin();
	auto minY = target->recursiveAabb().yMin();
	auto minZ = target->recursiveAabb().zMin();
	auto maxX = target->recursiveAabb().xMax();
	auto maxY = target->recursiveAabb().yMax();
	auto maxZ = target->recursiveAabb().zMax();

	std::vector<std::vector<QVector3D>> paths;

	paths.push_back({ QVector3D(minX, minY, minZ), QVector3D(maxX, minY, minZ) });
	paths.push_back({ QVector3D(minX, minY, minZ), QVector3D(minX, maxY, minZ) });
	paths.push_back({ QVector3D(minX, minY, minZ), QVector3D(minX, minY, maxZ) });

	paths.push_back({ QVector3D(maxX, maxY, maxZ), QVector3D(maxX, minY, maxZ) });
	paths.push_back({ QVector3D(maxX, maxY, maxZ), QVector3D(minX, maxY, maxZ) });
	paths.push_back({ QVector3D(maxX, maxY, maxZ), QVector3D(maxX, maxY, minZ) });

	paths.push_back({ QVector3D(minX, minY, maxZ), QVector3D(maxX, minY, maxZ) });
	paths.push_back({ QVector3D(minX, minY, maxZ), QVector3D(minX, maxY, maxZ) });

	paths.push_back({ QVector3D(minX, maxY, minZ), QVector3D(minX, maxY, maxZ) });
	paths.push_back({ QVector3D(minX, maxY, minZ), QVector3D(maxX, maxY, minZ) });

	paths.push_back({ QVector3D(maxX, minY, minZ), QVector3D(maxX, maxY, minZ) });
	paths.push_back({ QVector3D(maxX, minY, minZ), QVector3D(maxX, minY, maxZ) });

	addLinePaths(paths);
}

void Hix::Debug::DebugRenderObject::showGLModelFaces(SceneEntityWithMaterial* target)
{
	std::vector<std::vector<QVector3D>> paths;
	auto aabb = target->aabb();
	aabb.localBoundUpdate(*target->getMesh());
	qDebug() << aabb.zMin() << aabb.zMax() << aabb.lengthZ();

	for (auto face = target->getMesh()->getFaces().begin(); face != target->getMesh()->getFaces().end(); ++face)
	{
		auto v1 = face.meshVertices().at(0).localPosition();
		auto v2 = face.meshVertices().at(1).localPosition();
		auto v3 = face.meshVertices().at(2).localPosition();

		paths.push_back({ v1, v2 });
		paths.push_back({ v2, v3 });
		paths.push_back({ v3, v1 });
	}

	addLinePaths(paths);
}

void Hix::Debug::DebugRenderObject::showAabb(const Bounds3D& aabb)
{
	auto minX = aabb.xMin();
	auto minY = aabb.yMin();
	auto minZ = aabb.zMin();
	auto maxX = aabb.xMax();
	auto maxY = aabb.yMax();
	auto maxZ = aabb.zMax();

	std::vector<std::vector<QVector3D>> paths;

	paths.push_back({ QVector3D(minX, minY, minZ), QVector3D(maxX, minY, minZ) });
	paths.push_back({ QVector3D(minX, minY, minZ), QVector3D(minX, maxY, minZ) });
	paths.push_back({ QVector3D(minX, minY, minZ), QVector3D(minX, minY, maxZ) });

	paths.push_back({ QVector3D(maxX, maxY, maxZ), QVector3D(maxX, minY, maxZ) });
	paths.push_back({ QVector3D(maxX, maxY, maxZ), QVector3D(minX, maxY, maxZ) });
	paths.push_back({ QVector3D(maxX, maxY, maxZ), QVector3D(maxX, maxY, minZ) });

	paths.push_back({ QVector3D(minX, minY, maxZ), QVector3D(maxX, minY, maxZ) });
	paths.push_back({ QVector3D(minX, minY, maxZ), QVector3D(minX, maxY, maxZ) });

	paths.push_back({ QVector3D(minX, maxY, minZ), QVector3D(minX, maxY, maxZ) });
	paths.push_back({ QVector3D(minX, maxY, minZ), QVector3D(maxX, maxY, minZ) });

	paths.push_back({ QVector3D(maxX, minY, minZ), QVector3D(maxX, maxY, minZ) });
	paths.push_back({ QVector3D(maxX, minY, minZ), QVector3D(maxX, minY, maxZ) });

	addLinePaths(paths);
}

void Hix::Debug::DebugRenderObject::clear()
{
	_planes.clear();
	_lines.clear();
	//for (auto& e : _debugFaceMap)
	//{
	//	auto model = e.first;
	//	model->selectedFaces.clear();
	//}
	_debugFaceMap.clear();
}

std::optional<std::reference_wrapper<const FaceColorSet>> Hix::Debug::DebugRenderObject::getOverrideColors(SceneEntityWithMaterial* owner) const
{
	const auto& set = _debugFaceMap.find(owner);
	if (set != _debugFaceMap.end())
	{
		return std::ref(set->second);
	}
	return std::optional<std::reference_wrapper<const FaceColorSet>>();
}

Hix::Render::PlaneMeshEntity& Hix::Debug::DebugRenderObject::displayPlane(const Hix::Plane3D::PDPlane& pdplane)
{
	auto& plane = _planes.emplace_back(_root);
	plane.setPointNormal(pdplane);
	return plane;
}





























#else
#endif
