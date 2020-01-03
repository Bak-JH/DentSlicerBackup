#include "PlaneMeshEntity.h"
#include "../../qmlmanager.h"
using namespace Hix::Render;

PlaneMeshEntity::PlaneMeshEntity(Qt3DCore::QEntity* owner, bool isDoubleSided):
	PlaneMeshEntity(owner, qmlManager->settings().printerSetting.bedBound.lengthX(), qmlManager->settings().printerSetting.bedBound.lengthY(), QColor(244, 244, 244, 255), isDoubleSided)
{
}

Hix::Render::PlaneMeshEntity::PlaneMeshEntity(Qt3DCore::QEntity* owner, float width, float height, const QColor& color, bool isDoubleSided) : Qt3DCore::QEntity(owner)
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
		auto clipPlane = new Qt3DExtras::QPlaneMesh(this);
		clipPlane->setHeight(height);
		clipPlane->setWidth(width);
		auto planeTransform = new Qt3DCore::QTransform();
		planeTransform->setRotation(QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), 90 + 180 * i));
		planeEntity->addComponent(clipPlane);
		planeEntity->addComponent(planeTransform); //jj
		planeEntity->addComponent(planeMaterial);
		planeEntity->setEnabled(true);

		_meshTransformMap[planeEntity] = planeTransform;
	}

	addComponent(&_transform);
}

PlaneMeshEntity::~PlaneMeshEntity()
{
}

Qt3DCore::QTransform& PlaneMeshEntity::transform()
{
	return _transform;
}

void Hix::Render::PlaneMeshEntity::setPointNormal(const Hix::Plane3D::PDPlane& plane)
{
	_transform.setTranslation(plane.point);
	_transform.setRotation(QQuaternion::fromDirection(plane.normal,QVector3D(0,0,1)));
}


