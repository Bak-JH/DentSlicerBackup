#include <QRenderSettings>
#include "glmodel.h"
#include <QString>
#include <QtMath>
#include <cfloat>
#include <exception>

#include "qmlmanager.h"
#include "feature/shelloffset.h"
//#include "feature/supportview.h"
#include "feature/stlexporter.h"
#include "utils/utils.h"
#include <QtConcurrent/QtConcurrent>
#include <QFuture>
#include <QFileDialog>
#include <iostream>
#include <QDir>
#include <QMatrix3x3>
#include <Qt3DCore/qpropertyupdatedchange.h>
#include "DentEngine/src/configuration.h"
#include "feature/Extrude.h"
#include "feature/cut/modelcut.h"
#include "feature/cut/DrawingPlane.h"
#include "feature/label/labelling.h"
#include "feature/layFlat.h"
#include "feature/interfaces/SelectFaceFeature.h"

#define ATTRIBUTE_SIZE_INCREMENT 200
#if defined(_DEBUG) || defined(QT_DEBUG)
#define _STRICT_GLMODEL
#endif



using namespace Utils::Math;
using namespace Hix::Engine3D;
using namespace Hix::Input;
using namespace Hix::Render;

GLModel::GLModel(QEntity*parent, Mesh* loadMesh, QString fname, int id, const Qt3DCore::QTransform* transform)
    : SceneEntityWithMaterial(parent)
    , _name(fname)
    , ID(id)
{

	initHitTest();
    qDebug() << "new model made _______________________________"<<this<< "parent:"<<parent;
	qDebug() << "new model made _______________________________" << ID;
    // set shader mode and color
	setMaterialMode(Hix::Render::ShaderMode::SingleColor);
	setMaterialColor(Hix::Render::Colors::Default);

	setMesh(loadMesh);
	
	if (transform)
	{
		_transform.setMatrix(transform->matrix());
	}

	// 승환 75%
	qmlManager->setProgress(0.73);
	// reserve cutting points, contours
	sphereEntity.reserve(50);
	sphereMesh.reserve(50);
	sphereMaterial.reserve(50);
	sphereTransform.reserve(50);
	sphereObjectPicker.reserve(50);

	QObject::connect(this, SIGNAL(_updateModelMesh()), this, SLOT(updateModelMesh()));

}

void GLModel::moveModel(const QVector3D& movement) {
	auto translation = _transform.translation() + movement;
	_transform.setTranslation(translation);
	updateAABBMove(movement);
}

void GLModel::rotateModel(const QQuaternion& rotation) {
	auto newRot = rotation * _transform.rotation();
	_transform.setRotation(newRot);
}


void GLModel::scaleModel(const QVector3D& scales) {
	auto newScales = _transform.scale3D() * scales; //this is NOT cross product
	_transform.setScale3D(newScales);
	updateAABBScale(scales);

}


void GLModel::updateAABBMove(const QVector3D& displacement)
{
	auto translation = _transform.translation() + displacement;
	_aabb.translate(displacement);
	callRecursive(this, &GLModel::updateAABBMove, displacement);
}
void GLModel::updateAABBScale(const QVector3D& scale)
{
	updateRecursiveAabb();
	//auto newScales = _transform.scale3D() * scale; //this is NOT cross product
	//_aabb.translate(-_transform.translation());
	//_aabb.scale(QVector3D(1.0f, 1.0f, 1.0f) / _transform.scale3D());
	//_aabb.scale(newScales);
	//_aabb.translate(_transform.translation());
	//callRecursive(this, &GLModel::updateAABBScale, scale);
}


void GLModel::moveDone()
{
	updatePrintable();
}


void GLModel::rotateDone()
{
	updateRecursiveAabb();
	setZToBed();
	updatePrintable();
}
void GLModel::scaleDone()
{

	setZToBed();
	updatePrintable();

}


void GLModel::setZToBed()
{
	auto listedModel = getRootModel();
	listedModel->moveModel(QVector3D(0, 0, -listedModel->recursiveAabb().zMin()));
}

QString GLModel::modelName() const
{
	return _name;
}

GLModel* GLModel::getRootModel()
{
	auto current = this;
	GLModel* back = current;
	while (current)
	{
		back = current;
		current = dynamic_cast<GLModel*>(current->parent());
	}
	return back;

}



void GLModel::changeColor(const QVector4D& color)
{
	setMaterialColor(color);
}

bool GLModel::isPrintable()const
{
	const auto& bedBound = scfg->bedBound();
	return bedBound.contains(_aabb);
}

void GLModel::updatePrintable() {
	// is it inside the printing area or not?
	if(!isPrintable())
	{
		setMaterialColor(Hix::Render::Colors::OutOfBound);
	}
	else
	{
		if (qmlManager->isSelected(this))
		{
			setMaterialColor(Hix::Render::Colors::Selected);
		}
		else
		{
			setMaterialColor(Hix::Render::Colors::Default);
		}
	}
}

GLModel::~GLModel(){
}

 void GLModel::getChildrenModels(std::unordered_set<const GLModel*>& results)const
{
	 
	for (auto child : childNodes())
	{
		auto model = dynamic_cast<GLModel*>(child);
		if (model)
		{
			results.insert(model);
		}
	}
	callRecursive(this, &GLModel::getChildrenModels, results);

}

void GLModel::initHitTest()
{
	addComponent(&_layer);
	_layer.setRecursive(false);

}

void GLModel::setHitTestable(bool isEnable)
{
	if (_hitEnabled != isEnable)
	{
		_hitEnabled = isEnable;
		if (_hitEnabled)
		{
			
			qmlManager->getRayCaster().addInputLayer(&_layer);
		}
		else
		{
			qmlManager->getRayCaster().removeInputLayer(&_layer);
		}
	}
	callRecursive(this, &GLModel::setHitTestable, isEnable);
}

void GLModel::clicked(MouseEventData& pick, const Qt3DRender::QRayCasterHit& hit)
{
	auto listed = getRootModel();
	if (!qmlManager->isFeatureActive())
	{
		if (pick.button == Qt::MouseButton::LeftButton)
		{
			qmlManager->modelSelected(listed->ID);
		}
		else if (pick.button == Qt::MouseButton::RightButton && qmlManager->isSelected(listed))
		{
			qDebug() << "mttab alive";
			QMetaObject::invokeMethod(qmlManager->mttab, "tabOnOff");
		}
		return;
	}

    //triangle index section
    if (hit.primitiveIndex() >= _mesh->getFaces().size() || hit.localIntersection() == QVector3D(0, 0, 0) || !qmlManager->isSelected(listed))
    {
    }
	else
	{
		auto selectFaceFeature = dynamic_cast<Hix::Features::SelectFaceFeature*>(qmlManager->currentFeature());
		if (selectFaceFeature)
		{
			auto selectedFace = _mesh->getFaces().cbegin() + hit.primitiveIndex();
			selectFaceFeature->faceSelected(this, selectedFace, pick, hit);
		}
	}
}
void GLModel::updateModelMesh() {
	QMetaObject::invokeMethod(qmlManager->boxUpperTab, "disableUppertab");
	QMetaObject::invokeMethod(qmlManager->boxLeftTab, "disableLefttab");
	QMetaObject::invokeMethod((QObject*)qmlManager->scene3d, "disableScene3D");
	updateMesh(_mesh);
	qmlManager->sendUpdateModelInfo();
	updateLock = false;
	QMetaObject::invokeMethod(qmlManager->boxUpperTab, "enableUppertab");
	QMetaObject::invokeMethod(qmlManager->boxLeftTab, "enableLefttab");
	QMetaObject::invokeMethod((QObject*)qmlManager->scene3d, "enableScene3D");
}


bool GLModel::isDraggable(Hix::Input::MouseEventData& e,const Qt3DRender::QRayCasterHit&)
{
	auto listed = getRootModel();
	if (e.button == Qt3DInput::QMouseEvent::Buttons::LeftButton && qmlManager->isSelected(listed) && !qmlManager->isFeatureActive())
	{
		return true;
	}
	return false;
}

void GLModel::dragStarted(Hix::Input::MouseEventData& e, const Qt3DRender::QRayCasterHit& hit)
{
	auto listed = getRootModel();
	if (qmlManager->supportRaftManager().supportActive())
	{
		size_t prevCount = qmlManager->supportRaftManager().supportCount();
		qmlManager->supportRaftManager().clear(*listed);
		if (qmlManager->supportRaftManager().supportCount() != prevCount)
		{
			listed->setZToBed();
		}
	}
	lastpoint = hit.localIntersection();
	prevPoint = (QVector2D)e.position;
	qmlManager->moveButton->setProperty("state", "active");
	qmlManager->setClosedHandCursor();
}

void GLModel::doDrag(Hix::Input::MouseEventData& v)
{
	QVector2D currentPoint = QVector2D(v.position.x(), v.position.y());
	//auto pt = qmlManager->world2Screen(QVector3D(0, 0, 0));
	//auto pt2 = qmlManager->world2Screen(lastpoint);

	//qDebug()<< currentPoint << pt << pt2;
	QVector3D xAxis3D = QVector3D(1, 0, 0);
	QVector3D yAxis3D = QVector3D(0, 1, 0);
	QVector2D xAxis2D = (qmlManager->world2Screen(lastpoint + xAxis3D) - qmlManager->world2Screen(lastpoint));
	QVector2D yAxis2D = (qmlManager->world2Screen(lastpoint + yAxis3D) - qmlManager->world2Screen(lastpoint));
	QVector2D target = currentPoint - prevPoint;

	float b = (target.y() * xAxis2D.x() - target.x() * xAxis2D.y()) /
		(xAxis2D.x() * yAxis2D.y() - xAxis2D.y() * yAxis2D.x());
	float a = (target.x() - b * yAxis2D.x()) / xAxis2D.x();

	// move ax + by amount
	qmlManager->modelMove(QVector3D(a, b, 0));
	prevPoint = currentPoint;
}

void GLModel::dragEnded(Hix::Input::MouseEventData&)
{
    qmlManager->totalMoveDone();
}


/** HELPER functions **/

bool GLModel::EndsWith(const std::string& a, const std::string& b) {
    if (b.size() > a.size()) return false;
    return std::equal(a.begin() + a.size() - b.size(), a.end(), b.begin());
}


QVector3D GLModel::spreadPoint(QVector3D endPoint, QVector3D startPoint, int factor) {
	QVector3D standardVector = endPoint - startPoint;
	QVector3D finalVector = endPoint + standardVector * (factor - 1);
	return finalVector;
}
// for extension

void GLModel:: unselectMeshFaces(){
	selectedFaces.clear();
	callRecursive(this, &GLModel::unselectMeshFaces);
}

void GLModel::changeViewMode(int viewMode) {
    if( this->viewMode == viewMode ) {
        return;
    }

    this->viewMode = viewMode;
    qDebug() << "changeViewMode" << viewMode;
    QMetaObject::invokeMethod(qmlManager->boxUpperTab, "all_off");

    switch( viewMode ) {
    case VIEW_MODE_OBJECT:
        break;
    case VIEW_MODE_LAYER:
        break;
    }
	updateShader(viewMode);


    emit _updateModelMesh();
}


void GLModel::updateShader(int viewMode)
{

	switch (viewMode) {
	case VIEW_MODE_OBJECT:
		if (faceSelectionActive())
		{
			setMaterialMode(Hix::Render::ShaderMode::PerPrimitiveColor);
		}
		else
		{
			setMaterialMode(Hix::Render::ShaderMode::SingleColor);
		}
		break;
	case VIEW_MODE_LAYER:
		setMaterialMode(Hix::Render::ShaderMode::LayerMode);
		break;
	}




}

bool GLModel::perPrimitiveColorActive() const
{
	return faceSelectionActive();
}
bool GLModel::faceSelectionActive() const
{
	return qmlManager->isActive<Hix::Features::Extend>() ||
		qmlManager->isActive<Hix::Features::LayFlat>();
}

QVector4D GLModel::getPrimitiveColorCode(const Hix::Engine3D::Mesh* mesh, FaceConstItr itr)
{
	if (selectedFaces.find(itr) != selectedFaces.end())
	{
		return Hix::Render::Colors::SelectedFace;
	}
	else
	{
		return Hix::Render::Colors::Selected;
	}

}
