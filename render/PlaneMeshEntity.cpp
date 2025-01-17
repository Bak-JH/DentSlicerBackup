#include "PlaneMeshEntity.h"
#include "../application/ApplicationManager.h"
#include "../render/ModelMaterial.h"
using namespace Hix::Render;

PlaneMeshEntity::PlaneMeshEntity(Qt3DCore::QEntity* owner, bool isDoubleSided, float alpha):
	PlaneMeshEntity(owner, Hix::Application::ApplicationManager::getInstance().settings().printerSetting.bedBound.lengthX(),
		Hix::Application::ApplicationManager::getInstance().settings().printerSetting.bedBound.lengthY(), QColor(140, 140, 140), isDoubleSided, alpha)
{
}

Hix::Render::PlaneMeshEntity::PlaneMeshEntity(Qt3DCore::QEntity* owner, float width, float height, const QColor& color, bool isDoubleSided, float alpha) : Qt3DCore::QEntity(owner)
{
	QColor actualColor = color;
	actualColor.setAlphaF(alpha);
	size_t plnCnt = 1;
	if (isDoubleSided)
		++plnCnt;
	for (size_t i = 0; i < plnCnt; ++i)
	{

		auto planeEntity = new Qt3DCore::QEntity(this);
		//qDebug() << "generatePlane---------------------==========-=-==-" << parentModel;
		auto clipPlane = new Qt3DExtras::QPlaneMesh(this);
		clipPlane->setHeight(height);
		clipPlane->setWidth(width);
		auto planeTransform = new Qt3DCore::QTransform();
		planeTransform->setRotation(QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), 90 + 180 * i));
		planeEntity->addComponent(clipPlane);
		planeEntity->addComponent(planeTransform);
		if (alpha > 0.01f)
		{
			auto planeMaterial = new Hix::Render::ModelMaterial(this);
			planeMaterial->setColor(actualColor);
			planeMaterial->setDiffuse(actualColor);
			planeMaterial->setAmbient(actualColor);
			planeMaterial->changeMode(Hix::Render::ShaderMode::SingleColor);
			planeEntity->addComponent(planeMaterial);
		}
		planeEntity->setEnabled(true);
		_meshTransformMap[planeEntity] = planeTransform;
		_entityOrder.push_back(planeEntity);
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

Qt3DCore::QTransform& Hix::Render::PlaneMeshEntity::planeTransform(size_t index)
{
	return *_meshTransformMap.at(_entityOrder[index]);
}

const Qt3DCore::QTransform& Hix::Render::PlaneMeshEntity::transform() const
{
	return _transform;
}

const Qt3DCore::QTransform& Hix::Render::PlaneMeshEntity::planeTransform(size_t index) const
{
	return *_meshTransformMap.at(_entityOrder[index]);
}



void Hix::Render::PlaneMeshEntity::setPointNormal(const Hix::Plane3D::PDPlane& plane)
{
	_transform.setTranslation(plane.point);
	_transform.setRotation(QQuaternion::fromDirection(plane.normal,QVector3D(0,0,1)));
}

Hix::Plane3D::PDPlane Hix::Render::PlaneMeshEntity::pointNormal() const
{
	return{_transform.translation(), _transform.rotation().rotatedVector(QVector3D(0,0,1))};

}
