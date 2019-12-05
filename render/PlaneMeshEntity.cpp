#include "PlaneMeshEntity.h"
#include "../../qmlmanager.h"
using namespace Hix::Render;

PlaneMeshEntity::PlaneMeshEntity(Qt3DCore::QEntity* owner): Qt3DCore::QEntity(owner)
{
	auto planeMaterial = new Qt3DExtras::QPhongAlphaMaterial(this);
	planeMaterial->setAmbient(QColor(244,244,244,255));
	planeMaterial->setDiffuse(QColor(244, 244, 244, 255));
	planeMaterial->setSpecular(QColor(244, 244, 244, 255));

	for (int i = 0; i < 1; i++) {
		auto planeEntity = new Qt3DCore::QEntity(this);
		//qDebug() << "generatePlane---------------------==========-=-==-" << parentModel;
		auto clipPlane = new Qt3DExtras::QPlaneMesh(this);
		clipPlane->setHeight(200.0);
		clipPlane->setWidth(200.0);

		auto planeTransform = new Qt3DCore::QTransform();
		planeTransform->setRotation(QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0),  90 + 180 * i));
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


