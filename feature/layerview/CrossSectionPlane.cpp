#include "CrossSectionPlane.h"
#include "../../qmlmanager.h"
using namespace Hix::Input;
using namespace Hix::Features;


Hix::Features::CrossSectionPlane::CrossSectionPlane(Qt3DCore::QEntity* owner): QEntity(owner)
{
	_planeMaterial = new Qt3DExtras::QTextureMaterial(this);
	_planeMaterial->setAlphaBlendingEnabled(false);
	auto clipPlane = new Qt3DExtras::QPlaneMesh(this);
	clipPlane->setHeight(qmlManager->settings().printerSetting.bedX);
	clipPlane->setWidth(qmlManager->settings().printerSetting.bedY);
	clipPlane->setEnabled(true);
	_transform.setRotationY(-90);
	_transform.setRotationZ(-90);
	addComponent(clipPlane);
	addComponent(&_transform);
	addComponent(_planeMaterial);
	addComponent(&_transform);
	setEnabled(true);

	_textureLoader = new  Qt3DRender::QTextureLoader(this);
	_planeMaterial->setTexture(_textureLoader);
}


void CrossSectionPlane::loadTexture(QString filename)
{

	_textureLoader->setSource(QUrl::fromLocalFile(filename));//"C:\\Users\\User\\Desktop\\sliced\\11111_export\\100.svg"));


	//layerViewPlaneMaterial->setTexture(layerViewPlaneTextureLoader);
	const float rotation_values[] = { // rotate by -90 deg
	0, -1, 0,
	1, 0, 0,
	0, 0, 1
	};
	//flip Ys,
	const float flip_values[] = {
		1, 0, 0,
		0, -1, 0,
		0, 0, 1
	};

	QMatrix3x3 rotation_matrix(rotation_values);
	QMatrix3x3 flip_matrix(flip_values);
	QMatrix3x3 matrixTransform = flip_matrix * rotation_matrix;

	_planeMaterial->setTextureTransform(matrixTransform);



}


Hix::Features::CrossSectionPlane::~CrossSectionPlane()
{
}

void Hix::Features::CrossSectionPlane::enablePlane(bool isEnable)
{
}

Qt3DCore::QTransform& Hix::Features::CrossSectionPlane::transform()
{
	return _transform;
}
