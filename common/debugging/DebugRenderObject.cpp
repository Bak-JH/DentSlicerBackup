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

void Hix::Debug::DebugRenderObject::showGLModelAabb(GLModel* target)
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

	addPaths(paths);
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
