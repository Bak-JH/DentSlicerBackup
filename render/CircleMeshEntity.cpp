#include "CircleMeshEntity.h"
#include "application/ApplicationManager.h"
using namespace Hix;
using namespace Hix::Render;
using namespace Hix::Engine3D;

CircleMeshEntity::CircleMeshEntity(Qt3DCore::QEntity* owner, bool isDoubleSided) :
	CircleMeshEntity(owner, Hix::Application::ApplicationManager::getInstance().settings().printerSetting.bedRadius, QColor(244, 244, 244, 255), isDoubleSided)
{
}

CircleMeshEntity::CircleMeshEntity(Qt3DCore::QEntity* parent, float radius, const QColor& color, bool isDoubleSided)
	: Qt3DCore::QEntity(parent)
{
	size_t plnCnt = 1;
	if (isDoubleSided)
		++plnCnt;
	for (size_t i = 0; i < plnCnt; ++i)
	{
		auto planeMaterial = new Qt3DExtras::QPhongAlphaMaterial(this);
		planeMaterial->setAmbient(color);
		planeMaterial->setDiffuse(color);
		planeMaterial->setSpecular(color);
		auto planeEntity = new Qt3DCore::QEntity(this);
		//qDebug() << "generatePlane---------------------==========-=-==-" << parentModel;
		auto clipPlane = new Qt3DExtras::QCylinderMesh(this);
		clipPlane->setRadius(radius);
		clipPlane->setLength(0.001f);
		auto planeTransform = new Qt3DCore::QTransform();
		planeTransform->setRotation(QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), 90 + 180 * i));
		planeEntity->addComponent(clipPlane);
		planeEntity->addComponent(planeTransform); //jj
		planeEntity->addComponent(planeMaterial);
		planeEntity->setEnabled(true);

		_meshTransformMap[planeEntity] = planeTransform;
	}
	addComponent(&_transform);

	//auto circle = Hix::Shapes2D::generateCircle(radius, segCount);
	//QVector3D centre(0, 0, 0);
	//auto* prevPt = &circle.back();
	//for (size_t i = 0; i < segCount; ++i)
	//{
	//	auto& currPt = circle[i];
	//	qDebug() << currPt;
	//	mesh->addFace(*prevPt, currPt, centre);
	//	prevPt = &currPt;
	//}
	//setMesh(mesh);
}


Hix::Render::CircleMeshEntity::~CircleMeshEntity()
{
}

Qt3DCore::QTransform& CircleMeshEntity::transform()
{
	return _transform;
}

void Hix::Render::CircleMeshEntity::setPointNormal(const Hix::Plane3D::PDPlane& plane)
{
	_transform.setTranslation(plane.point);
	_transform.setRotation(QQuaternion::fromDirection(plane.normal, QVector3D(0, 0, 1)));
}
