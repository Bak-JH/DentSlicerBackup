#include "boundingbox.h"
#include <Qt3DRender>
#include <Qt3DExtras>
using namespace Qt3DCore;
using namespace Qt3DRender;
using namespace Qt3DExtras;

BoundingBox::BoundingBox(QNode* parent): QEntity(parent)
{
    auto rect = new QCuboidMesh();
    rect->setXExtent(1);
    rect->setYExtent(1);
    rect->setZExtent(1);

    rect->setXYMeshResolution({2,2});
    rect->setXZMeshResolution({2,2});
    rect->setYZMeshResolution({2,2});
	_transform = new Qt3DCore::QTransform();
	_transform->setScale(1.0f);
	this->addComponent(rect);
	this->addComponent(_transform);
}

void BoundingBox::setSize(const QVector3D& size)
{
	_transform->setScale3D(size);
}

void BoundingBox::setPos(const QVector3D& pos)
{
	_transform->setTranslation(pos);

}
