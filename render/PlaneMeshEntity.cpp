#include "PlaneMeshEntity.h"
#include "../application/ApplicationManager.h"

using namespace Hix::Render;

PlaneMeshEntity::PlaneMeshEntity(Qt3DCore::QEntity* owner, bool isDoubleSided):
	PlaneMeshEntity(owner, Hix::Application::ApplicationManager::getInstance().settings().printerSetting.bedBound.lengthX(),
		Hix::Application::ApplicationManager::getInstance().settings().printerSetting.bedBound.lengthY(), QColor(140, 140, 140, 0), isDoubleSided)
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

void Hix::Render::PlaneMeshEntity::setAlpha(const float alpha)
{
	for (auto each : _meshTransformMap)
	{
		for (auto comp : each.first->components())
		{
			if (dynamic_cast<Qt3DExtras::QPhongAlphaMaterial*>(comp))
			{
				dynamic_cast<Qt3DExtras::QPhongAlphaMaterial*>(comp)->setAlpha(alpha);
			}
		}
	}
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


