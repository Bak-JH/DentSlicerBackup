#include "boundingbox.h"
#include <Qt3DRender>
#include <Qt3DExtras>
using namespace Qt3DCore;
using namespace Qt3DRender;
using namespace Qt3DExtras;

BoundingBox::BoundingBox(QNode* parent): QEntity(parent)
{
	auto mat = new QPhongMaterial();
	mat->setAmbient(QColor(240, 249, 70, 255));
	mat->setDiffuse(QColor(240, 249, 70, 255));
	mat->setSpecular(QColor(240, 249, 70, 255));
	mat->setShininess(0);
	auto mesh = new QMesh();
	mesh->setSource(QUrl("qrc:/Resource/mesh/boundedBox3.stl"));
	_transform = new Qt3DCore::QTransform();
	_transform->setScale(1.0f);
	this->addComponent(mesh);
	this->addComponent(_transform);
	this->addComponent(mat);

}

void BoundingBox::setSize(const QVector3D& size)
{
	_transform->setScale3D(size);

}

void BoundingBox::setPos(const QVector3D& pos)
{
	_transform->setTranslation(pos);

}
