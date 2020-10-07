#include "CircleMeshEntity.h"
#include "../feature/Shapes2D.h"
using namespace Hix;
using namespace Hix::Render;
using namespace Hix::Engine3D;
CircleMeshEntity::CircleMeshEntity(float radius, size_t segCount, const QColor& color, Qt3DCore::QEntity* parent) 
	: SceneEntityWithMaterial(parent)
{
	setMaterialMode(ShaderMode::NoLightsMode);
	setMaterialParamter("singleColor", QVector4D(color.redF(), color.greenF(), color.blueF(), 1.0f));

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
	setMaterialMode(ShaderMode::None);
}

QVector4D Hix::Render::CircleMeshEntity::getPrimitiveColorCode(const Hix::Engine3D::Mesh* mesh, Hix::Engine3D::FaceConstItr faceItr)
{
	return QVector4D();
}
