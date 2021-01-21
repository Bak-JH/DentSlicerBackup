#pragma once
#include <Qt3DCore>


class BoundingBox: public Qt3DCore::QEntity
{
	Q_OBJECT
public:
    BoundingBox(QNode* parent);

    void setSize(const QVector3D& size);
	void setPos(const QVector3D& pos);

protected:
    Qt3DCore::QTransform* _transform;
};

