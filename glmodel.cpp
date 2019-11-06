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
	qDebug() << "new model made _______________________________" << fname;
    // set shader mode and color
	setMaterialMode(Hix::Render::ShaderMode::SingleColor);
	setMaterialColor(Hix::Render::Colors::Default);

	if (transform)
	{
		_transform.setMatrix(transform->matrix());
	}
	else
	{
		loadMesh->centerMesh();
	}
	setMesh(loadMesh);
	//applyGeometry();
	// 승환 25%
	qmlManager->setProgress(0.23);
	// 승환 50%
	qmlManager->setProgress(0.49);
	//Qt3DExtras::QDiffuseMapMaterial* diffuseMapMaterial = new Qt3DExtras::QDiffuseMapMaterial();


	qDebug() << "created original model";

	//repairMesh(_mesh);
	//addShadowModel(_mesh);

	// 승환 75%
	qmlManager->setProgress(0.73);


	qDebug() << "created shadow model";

	qDebug() << "adding part " << fname.toStdString().c_str();


	// reserve cutting points, contours
	sphereEntity.reserve(50);
	sphereMesh.reserve(50);
	sphereMaterial.reserve(50);
	sphereTransform.reserve(50);
	sphereObjectPicker.reserve(50);

	QObject::connect(this, SIGNAL(_updateModelMesh()), this, SLOT(updateModelMesh()));

}



void GLModel::moveModel(const QVector3D& displacement) {
	auto translation = _transform.translation() + displacement;
	_transform.setTranslation(translation);
	_aabb.translate(displacement);
}
void GLModel::rotateModel(const QQuaternion& rotation) {
	auto newRot = rotation * _transform.rotation();
	_transform.setRotation(newRot);
}


void GLModel::scaleModel(const QVector3D& scales) {
	auto newScales = _transform.scale3D() * scales; //this is NOT cross product
	_aabb.translate(-_transform.translation());
	_aabb.scale(QVector3D(1.0f, 1.0f, 1.0f)/_transform.scale3D());
	_aabb.scale(newScales);
	_aabb.translate(_transform.translation());
	_transform.setScale3D(newScales);
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
	moveModel(QVector3D(0, 0, -_aabb.zMin()));
}

QString GLModel::modelName() const
{
	return _name;
}





void GLModel::changeColor(const QVector3D& color)
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


void GLModel::repairMesh()
{
    //MeshRepair::modelRepair(this);
	emit _updateModelMesh();
}


/* copy info's from other GLModel */
void GLModel::copyModelAttributeFrom(GLModel* from){

}

void GLModel::updateModelMesh(){
    QMetaObject::invokeMethod(qmlManager->boxUpperTab, "disableUppertab");
    QMetaObject::invokeMethod(qmlManager->boxLeftTab, "disableLefttab");
    QMetaObject::invokeMethod((QObject*)qmlManager->scene3d, "disableScene3D");
    qDebug() << "update Model Mesh";
	updateMesh(_mesh);
    qmlManager->sendUpdateModelInfo();
    updateLock = false;
    qDebug() << this << "released lock";
    QMetaObject::invokeMethod(qmlManager->boxUpperTab, "enableUppertab");
    QMetaObject::invokeMethod(qmlManager->boxLeftTab, "enableLefttab");
    QMetaObject::invokeMethod((QObject*)qmlManager->scene3d, "enableScene3D");
}


// hollow shell part
void GLModel::indentHollowShell(double radius){
    qDebug() << "hollow shell called" << radius;
	if (!_targetSelected)
		return;
	auto meshVertices = _targetMeshFace.meshVertices();
    QVector3D center = (
		meshVertices[0].localPosition() +
		meshVertices[1].localPosition() +
		meshVertices[2].localPosition())/3;
	HollowShell::hollowShell(_mesh, _targetMeshFace, center, radius);
}

GLModel::~GLModel(){
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
	auto suppMode = qmlManager->supportRaftManager().supportEditMode();
	if (qmlManager->isActive<Hix::Features::ModelCut>(qmlManager->currentFeature()) &&
		qmlManager->isActive<Hix::Features::Extend>(qmlManager->currentFeature()) &&
		qmlManager->isActive<Hix::Features::Labelling>(qmlManager->currentFeature()) &&
		qmlManager->isActive<Hix::Features::LayFlat>(qmlManager->currentFeature()) &&
		suppMode == Hix::Support::EditMode::None)// && !layerViewActive && !supportViewActive)
		qmlManager->modelSelected(ID);

	if (qmlManager->isSelected(this) && pick.button == Qt::MouseButton::RightButton) {
		qDebug() << "mttab alive";
		QMetaObject::invokeMethod(qmlManager->mttab, "tabOnOff");
	}

#ifdef _STRICT_GLMODEL
	if (hit.type() != QRayCasterHit::HitType::TriangleHit)
		throw std::runtime_error("trying to get tri idx from non tri hit");
#endif


    //triangle index section
    if (hit.primitiveIndex() >= _mesh->getFaces().size())
    {
        qDebug() << "trianglePick out of bound";
        return;
    }
    _targetSelected = true;
    _targetMeshFace = _mesh->getFaces().cbegin() + hit.primitiveIndex();


	/// Extension Feature ///
	if (qmlManager->isActive<Hix::Features::Extend>(qmlManager->currentFeature()) &&
		hit.localIntersection() != QVector3D(0, 0, 0)) {
		unselectMeshFaces();
		emit extensionSelect();
		selectMeshFaces();
	}

	/// Hollow Shell ///
	//if (qmlManager->currentFeature() == Hix::Features::FeatureEnum::ShellOffset) {
	//	qDebug() << "getting handle picker clicked signal hollow shell active";
	//	qDebug() << "found parent meshface";
	//	// translate hollowShellSphere to mouse position
	//	QVector3D v = hit.localIntersection();
 //       qmlManager->hollowShellSphereTransform->setTranslation(v + _transform.translation());


	//}

	/// Lay Flat ///
	if (qmlManager->isActive<Hix::Features::LayFlat>(qmlManager->currentFeature()) &&
		hit.localIntersection() != QVector3D(0, 0, 0)) {

		unselectMeshFaces();
		emit layFlatSelect();
		selectMeshFaces();
	}

	/// Manual Support ///
	if (suppMode == Hix::Support::EditMode::Manual && hit.localIntersection() != QVector3D(0, 0, 0)) {
		Hix::OverhangDetect::FaceOverhang newOverhang;
		newOverhang.coord = ptToRoot(hit.localIntersection());
		newOverhang.face = _targetMeshFace;
		qmlManager->supportRaftManager().addSupport(newOverhang);
	}

	/// Labeling Feature ///
    if (qmlManager->isActive<Hix::Features::Labelling>(qmlManager->currentFeature()) 
			&& hit.localIntersection() != QVector3D(0, 0, 0)) 
	{
		qmlManager->setLabelTranslation(hit.localIntersection());
    }
}

bool GLModel::isDraggable(Hix::Input::MouseEventData& e,const Qt3DRender::QRayCasterHit&)
{
	if (e.button == Qt3DInput::QMouseEvent::Buttons::LeftButton
		&&
		qmlManager->isSelected(this) 
		&&
			(qmlManager->isActive<Hix::Features::ModelCut*>(qmlManager->currentFeature())||
			//qmlManager->currentFeature() != Hix::Features::FeatureEnum::ShellOffset ||
			qmlManager->isActive<Hix::Features::Extend*>(qmlManager->currentFeature()) ||
			qmlManager->isActive<Hix::Features::Labelling*>(qmlManager->currentFeature()) ||
			qmlManager->isActive<Hix::Features::LayFlat*>(qmlManager->currentFeature())
			//qmlManager->currentFeature() != Hix::Features::FeatureEnum::LayerViewMode)
		/*&&
		!(qmlManager->currentFeature() != Hix::Features::FeatureEnum::Orient ||
			qmlManager->currentFeature() != Hix::Features::FeatureEnum::Rotate ||
			qmlManager->currentFeature() != Hix::Features::FeatureEnum::Save)*/))
	{
		return true;
	}
	return false;
}

void GLModel::dragStarted(Hix::Input::MouseEventData& e, const Qt3DRender::QRayCasterHit& hit)
{
	if (qmlManager->supportRaftManager().supportActive())
	{
		auto count = qmlManager->supportRaftManager().clear(*this);
		if (count > 0)
			setZToBed();
	}
	lastpoint = hit.localIntersection();
	prevPoint = (QVector2D)e.position;
	qmlManager->moveButton->setProperty("state", "active");
	qmlManager->setClosedHandCursor();
	isMoved = true;
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
	isMoved = false;
    qmlManager->totalMoveDone();

}



void GLModel::getLayerViewSliderSignal(int value) {
   /* if ( !layerViewActive)
        return;*/

    //float height = (_mesh->z_max() - _mesh->z_min() + scfg->raft_thickness + scfg->support_base_height) * value;
    //int layer_num = int(height/scfg->layer_height)+1;
    //if (value <= 0.002f)
    //    layer_num = 0;

    if (layerViewPlaneTextureLoader == nullptr)
    layerViewPlaneTextureLoader = new Qt3DRender::QTextureLoader();

    QDir dir(QDir::tempPath()+"_export");//(qmlManager->selectedModels[0]->filename + "_export")
    if (dir.exists()){
        QString filename = dir.path()+"/"+QString::number(value)+".svg";
        qDebug() << filename;
        layerViewPlaneTextureLoader->setSource(QUrl::fromLocalFile(filename));//"C:\\Users\\User\\Desktop\\sliced\\11111_export\\100.svg"));
    }
    //qDebug() << "layer view plane material texture format : " << layerViewPlaneTextureLoader->format();
    //layerViewPlaneTextureLoader->setFormat(QAbstractTexture::RGBA32F);
    //qDebug() << "layer view plane material texture format : " << layerViewPlaneTextureLoader->format();

    layerViewPlaneMaterial->setTexture(layerViewPlaneTextureLoader);
	float rotation_values[] = { // rotate by -90 deg
	0, -1, 0,
	1, 0, 0,
	0, 0, 1
	};
	//flip Ys,
	float flip_values[] = {
		1, 0, 0,
		0, -1, 0,
		0, 0, 1
	};

    QMatrix3x3 rotation_matrix(rotation_values);
	QMatrix3x3 flip_matrix(flip_values);
	QMatrix3x3 matrixTransform = flip_matrix * rotation_matrix;

    layerViewPlaneMaterial->setTextureTransform(matrixTransform);
    layerViewPlaneTransform->setTranslation(QVector3D(0,0, value *scfg->layer_height - scfg->raft_thickness - scfg->support_base_height));

    // change phong material of original model
    float h = scfg->layer_height* value + _mesh->z_min() - scfg->raft_thickness - scfg->support_base_height;
	//_meshMaterial.setParameterValue("height", QVariant::fromValue(h));
	/*m_layerMaterialRaftHeight->setValue(QVariant::fromValue(qmlManager->getLayerViewFlags() & LAYER_INFILL != 0 ?
                _mesh->z_min() :
                _mesh->z_max() + scfg->raft_thickness - scfg->support_base_height));*/
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
	_targetSelected = false;
	callRecursive(this, &GLModel::unselectMeshFaces);
}
void GLModel::selectMeshFaces(){
	selectedFaces.clear();
	QVector3D normal = _targetMeshFace.localFn();
	_mesh->findNearSimilarFaces(normal, _targetMeshFace, selectedFaces);
	updateMesh(true);
}

// for shell offset
void GLModel::generateShellOffset(double factor){
    //saveUndoState();
    qDebug() << "generate shell Offset";
    qmlManager->openProgressPopUp();

    //cutMode = 1;
    //cutFillMode = 1;
    shellOffsetFactor = factor;

    //modelCut();

}


void GLModel::openHollowShell(){
    qDebug() << "open HollowShell called";
    //hollowShellActive = true;
    qmlManager->hollowShellSphereEntity->setProperty("visible", true);
}

void GLModel::closeHollowShell(){
    qDebug() << "close HollowShell called";

    //if (!hollowShellActive)
    //    return;

    //hollowShellActive = false;
    qmlManager->hollowShellSphereEntity->setProperty("visible", false);
}

void GLModel::openShellOffset(){
    qDebug() << "openShelloffset";
    //shellOffsetActive = true;

}

void GLModel::closeShellOffset(){
    qDebug() << "closeShelloffset";

    //if (!shellOffsetActive)
    //    return;

    //shellOffsetActive = false;
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
        //if (qmlManager->currentFeature() != Hix::Features::FeatureEnum::LayerViewMode){
        //    // remove layer view components
        //    removeLayerViewComponents();
        //}
        //layerViewActive = false;
		
        break;
    case VIEW_MODE_LAYER:
        //layerViewActive = true;
        // generate layer view plane materials
        layerViewPlaneMaterial = new Qt3DExtras::QTextureMaterial();
        layerViewPlaneMaterial->setAlphaBlendingEnabled(false);
        layerViewPlaneEntity = new Qt3DCore::QEntity(this);
        layerViewPlane=new Qt3DExtras::QPlaneMesh(this);
        layerViewPlane->setHeight(scfg->bedX());
        layerViewPlane->setWidth(scfg->bedY());
        layerViewPlaneTransform=new Qt3DCore::QTransform();
		//layerViewPlaneTransform->setRotationX(90);
		layerViewPlaneTransform->setRotationY(-90);
		layerViewPlaneTransform->setRotationZ(-90);

        layerViewPlaneEntity->addComponent(layerViewPlane);
        layerViewPlaneEntity->addComponent(layerViewPlaneTransform); //jj
        layerViewPlaneEntity->addComponent(layerViewPlaneMaterial);
		QVariant maxLayerCount;
		QMetaObject::invokeMethod(qmlManager->layerViewSlider, "getMaxLayer", Qt::DirectConnection, Q_RETURN_ARG(QVariant, maxLayerCount));
        getLayerViewSliderSignal(maxLayerCount.toInt());
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


void GLModel::inactivateFeatures()
{


    closeHollowShell();
    closeShellOffset();
    closeLayflat();

}

void GLModel::removeLayerViewComponents(){
    layerViewPlaneEntity->removeComponent(layerViewPlane);
    layerViewPlaneEntity->removeComponent(layerViewPlaneTransform); //jj
    layerViewPlaneEntity->removeComponent(layerViewPlaneMaterial);
    layerViewPlaneEntity->deleteLater();
    layerViewPlane->deleteLater();
    layerViewPlaneTransform->deleteLater();
    layerViewPlaneMaterial->deleteLater();
    layerViewPlaneTextureLoader->deleteLater();
    layerViewPlaneTextureLoader = nullptr;
}


bool GLModel::perPrimitiveColorActive() const
{
	return true; // TODO: delete later
	//return faceSelectionActive() || qmlManager->currentFeature() != Hix::Features::FeatureEnum::LayerViewMode;
}
bool GLModel::faceSelectionActive() const
{
	return qmlManager->isActive<Hix::Features::Extend>(qmlManager->currentFeature()) ||
		qmlManager->isActive<Hix::Features::LayFlat>(qmlManager->currentFeature());
}

QVector3D GLModel::getPrimitiveColorCode(const Hix::Engine3D::Mesh* mesh, FaceConstItr itr)
{
#ifdef _STRICT_GLMODEL
	if (!faceSelectionActive())
	{
		qDebug() << "getPrimitiveColorCode when faceSelectionActive";
		throw std::runtime_error("getPrimitiveColorCode when faceSelectionActive");
	}
#endif
		//color selected stuff yellow, everything non-yellow

	if (selectedFaces.find(itr) != selectedFaces.end())
	{
		return Hix::Render::Colors::SelectedFace;
	}
	else
	{
		return Hix::Render::Colors::Selected;
	}



}
