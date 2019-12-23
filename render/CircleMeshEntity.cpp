#include "CircleMeshEntity.h"
#include "../feature/Shapes2D.h"
using namespace Hix;
using namespace Hix::Render;
using namespace Hix::Engine3D;
CircleMeshEntity::CircleMeshEntity(float radius, size_t segCount, const QColor& color, Qt3DCore::QEntity* parent): SceneEntity(parent)
{
	addComponent(&_transform);
	addComponent(&_material);
	_material.setAmbient(color);
	_material.setDiffuse(color);
	_material.setSpecular(color);


	auto mesh = new Mesh();
	auto circle = Hix::Shapes2D::generateCircle(radius, segCount);
	QVector3D centre(0, 0, 0);
	auto* prevPt = &circle.back();
	for (size_t i = 0; i < segCount; ++i)
	{
		auto& currPt = circle[i];
		mesh->addFace(*prevPt, currPt, centre);
		prevPt = &currPt;
	}
	setMesh(mesh);
}


Hix::Render::CircleMeshEntity::~CircleMeshEntity()
{
	removeComponent(&_material);
}