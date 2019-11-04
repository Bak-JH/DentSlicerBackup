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
#include "feature/cut/DrawingPlane.h"
#include "feature/cut/modelCutZAxis.h"
#include "feature/labelling/labelModel.h"


#define ATTRIBUTE_SIZE_INCREMENT 200
#if defined(_DEBUG) || defined(QT_DEBUG)
#define _STRICT_GLMODEL
#endif



using namespace Utils::Math;
using namespace Hix::Engine3D;
using namespace Hix::Input;
using namespace Hix::Render;



GLModel::GLModel(QObject* mainWindow, QEntity*parent, Mesh* loadMesh, QString fname, int id)
    : SceneEntityWithMaterial(parent)
    , _filename(fname)
    , mainWindow(mainWindow)
    , cutMode(1)
    , ID(id)
{
	initHitTest();
    qDebug() << "new model made _______________________________"<<this<< "parent:"<<parent;
	qDebug() << "new model made _______________________________" << fname;
    // set shader mode and color
	setMaterialMode(Hix::Render::ShaderMode::SingleColor);
	setMaterialColor(Hix::Render::Colors::Default);

	if (_filename != "")
	{
		qmlManager->addPart(getFileName(fname.toStdString().c_str()), ID);
		if (_filename.contains(".stl") || _filename.contains(".STL")) {
			FileLoader::loadMeshSTL(loadMesh, _filename.toLocal8Bit().constData());
		}
		else if (_filename.contains(".obj") || _filename.contains(".OBJ")) {
			FileLoader::loadMeshOBJ(loadMesh, _filename.toLocal8Bit().constData());
		}
	}
	if (loadMesh)
	{
		loadMesh->centerMesh();
		setMesh(loadMesh);
	}
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

	QObject::connect(this, SIGNAL(bisectDone(Mesh*, Mesh*)), this, SLOT(generateRLModel(Mesh*, Mesh*)));

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

QString GLModel::filename() const
{
	return _filename;
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
    MeshRepair::modelRepair(this);
	emit _updateModelMesh();
}


/* copy info's from other GLModel */
void GLModel::copyModelAttributeFrom(GLModel* from){
    cutMode = from->cutMode;
    cutFillMode = from->cutFillMode;
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


void GLModel::generatePlane(int type){

	//generate drawing plane
	_cuttingPlane.reset(new Hix::Features::Cut::DrawingPlane(this)); 
	_cuttingPlane->enablePlane(true);
	//if flat cut
	if (type == 1)
	{	

	}
	else if (type == 2)
	{
		_cuttingPlane->enableDrawing(true);
		//want cutting plane to be over model mesh
		float zOverModel = _mesh->z_max() + 0.1f;
		_cuttingPlane->transform().setTranslation(QVector3D(0, 0, zOverModel));
		qmlManager->getRayCaster().setHoverEnabled(true);
	}
}

void GLModel::removePlane(){
	//freecut disable hovering
	if (cutMode == 2)
	{
		qmlManager->getRayCaster().setHoverEnabled(false);
	}
	_cuttingPlane.reset();
}

void GLModel::removeModelPartList(){
    //remove part list
    QList<QObject*> temp;
    temp.append(mainWindow);
    QObject *partList = (QEntity *)FindItemByName(temp, "partList");
    QObject *yesno_popup = (QEntity *)FindItemByName(temp, "yesno_popup");

    qDebug() <<"remove ID   " << ID;
    QMetaObject::invokeMethod(partList, "deletePartListItem", Q_ARG(QVariant, ID));
    QMetaObject::invokeMethod(yesno_popup, "deletePartListItem", Q_ARG(QVariant, ID));
}

void GLModel::modelCut(){

    qmlManager->openProgressPopUp();


    if (cutMode == 1){ // flat cut
		auto lmesh = new Mesh();
		auto rmesh = new Mesh();
        if (this->shellOffsetActive && isFlatcutEdge == true) {
            getSliderSignal(0.0);
        }
		bool fillCuttingSurface = cutFillMode == 2;
		Hix::Features::Cut::ZAxisCutTask task(_mesh, lmesh, rmesh, _cuttingPlane->transform().translation().z(), fillCuttingSurface);
        emit bisectDone(lmesh, rmesh);

    } else if (cutMode == 2){ // free cut
		auto cuttingContour = _cuttingPlane->contour();
        if (cuttingContour.size() >= 2){
			auto lmesh = new Mesh();
			auto rmesh = new Mesh();
            cutAway(lmesh, rmesh, _mesh, cuttingContour, cutFillMode);

            if (lmesh->getFaces().size() == 0 || rmesh->getFaces().size() == 0){

                qDebug() << "cutting contour selected not cutting";
                qmlManager->setProgress(1);
                cutModeSelected(2); // reset
                return;
            }
            emit bisectDone(lmesh, rmesh);
        }
    }
}

void GLModel::generateRLModel(Mesh* lmesh, Mesh* rmesh){
	GLModel* leftmodel = nullptr;
	GLModel* rightmodel = nullptr;
    qDebug() << "** generateRLModel" << this;
    if (lmesh->getFaces().size() != 0){
		leftmodel = qmlManager->createModelFile(lmesh, _filename+"_l");
        qDebug() << "leftmodel created";
    }
    // 승환 70%
    qmlManager->setProgress(0.72);
    if (rmesh->getFaces().size() != 0){
		rightmodel = qmlManager->createModelFile(rmesh, _filename +"_r");
        qDebug() << "rightmodel created";
    }


    // 승환 90%

    qDebug() << "found models : " << leftmodel << rightmodel;
    if (leftmodel != nullptr && rightmodel != nullptr){
        leftmodel->twinModel = rightmodel;
        rightmodel->twinModel = leftmodel;
    }

    qmlManager->setProgress(1);

    if (shellOffsetActive){
		if (leftmodel != nullptr)
		{
			auto offsetLeftMesh = ShellOffset::shellOffset(leftmodel->_mesh, (float)shellOffsetFactor);

			qmlManager->createModelFile(offsetLeftMesh, leftmodel->filename());

			qmlManager->deleteModelFile(leftmodel->ID);

		}
        if (rightmodel != nullptr)
            qmlManager->deleteModelFile(rightmodel->ID);
        QMetaObject::invokeMethod(qmlManager->boxUpperTab, "all_off");
    }


    //deleteLater();
    removePlane();
    // delete original model
    qmlManager->deleteModelFile(ID);

    // do auto arrange
    //qmlManager->runArrange();
	QMetaObject::invokeMethod(qmlManager->boundedBox, "hideBox");
    // 승환 100%
    qmlManager->setProgress(1);
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
	auto currentFeature = qmlManager->currentFeature();
	if (currentFeature != Hix::Features::FeatureEnum::Cut && 
		currentFeature != Hix::Features::FeatureEnum::Extend &&
		currentFeature != Hix::Features::FeatureEnum::Label &&
		currentFeature != Hix::Features::FeatureEnum::LayFlat &&
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
	if (qmlManager->currentFeature() == Hix::Features::FeatureEnum::Extend &&
		hit.localIntersection() != QVector3D(0, 0, 0)) {
		unselectMeshFaces();
		emit extensionSelect();
		selectMeshFaces();
	}

	/// Hollow Shell ///
	if (qmlManager->currentFeature() == Hix::Features::FeatureEnum::ShellOffset) {
		qDebug() << "getting handle picker clicked signal hollow shell active";
		qDebug() << "found parent meshface";
		// translate hollowShellSphere to mouse position
		QVector3D v = hit.localIntersection();
        qmlManager->hollowShellSphereTransform->setTranslation(v + _transform.translation());


	}

	/// Lay Flat ///
	if (qmlManager->currentFeature() == Hix::Features::FeatureEnum::LayFlat && 
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
    if (currentFeature != Hix::Features::FeatureEnum::Label && hit.localIntersection() != QVector3D(0, 0, 0)) {
		if (textPreview != nullptr)
			textPreview->setEnabled(false);

		textPreview = new Hix::Label::LabelModel(this);

		if (textPreview && currentFeature != Hix::Features::FeatureEnum::Label) {
			textPreview->setTranslation(hit.localIntersection());
            QMetaObject::invokeMethod(qmlManager->labelPopup, "labelUpdate");
        }
    }
}

bool GLModel::isDraggable(Hix::Input::MouseEventData& e,const Qt3DRender::QRayCasterHit&)
{
	if (e.button == Qt3DInput::QMouseEvent::Buttons::LeftButton
		&&
		qmlManager->isSelected(this) 
		&&
			(qmlManager->currentFeature() != Hix::Features::FeatureEnum::Cut ||
			qmlManager->currentFeature() != Hix::Features::FeatureEnum::ShellOffset ||
			qmlManager->currentFeature() != Hix::Features::FeatureEnum::Extend ||
			qmlManager->currentFeature() != Hix::Features::FeatureEnum::Label ||
			qmlManager->currentFeature() != Hix::Features::FeatureEnum::LayFlat ||
			qmlManager->currentFeature() != Hix::Features::FeatureEnum::LayerViewMode)
		&&
		!(qmlManager->currentFeature() != Hix::Features::FeatureEnum::Orient ||
			qmlManager->currentFeature() != Hix::Features::FeatureEnum::Rotate ||
			qmlManager->currentFeature() != Hix::Features::FeatureEnum::Save))
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

/** HELPER functions **/

bool GLModel::EndsWith(const std::string& a, const std::string& b) {
    if (b.size() > a.size()) return false;
    return std::equal(a.begin() + a.size() - b.size(), a.end(), b.begin());
}

QString GLModel::getFileName(const std::string& s){
   char sep = '/';

   size_t i = s.rfind(sep, s.length());
   if (i != std::string::npos) {
      return QString::fromStdString(s.substr(i+1, s.length() - i));
   }

   return QString::fromStdString("");
}

QVector3D GLModel::spreadPoint(QVector3D endPoint,QVector3D startPoint,int factor){
    QVector3D standardVector = endPoint-startPoint;
    QVector3D finalVector=endPoint+standardVector*(factor-1);
    return finalVector;
}


void GLModel::getTextChanged(QString text)
{
    if (text != "" && textPreview && labellingActive){
		textPreview->text = text;
    }
}

void GLModel::getFontNameChanged(QString fontName)
{
    qDebug() << "@@@@ getFontNameChanged";
    if (textPreview && labellingActive){
		textPreview->font.setFamily(fontName);
    }
}

void GLModel::getFontBoldChanged(bool isbold){
    qDebug() << "@@@@ getBoldChanged";
    if (textPreview && labellingActive){
		textPreview->font.setBold(isbold);
    }
}

void GLModel::getFontSizeChanged(int fontSize)
{
    qDebug() << "@@@@ getSizeChanged" << fontSize;
	if (textPreview && labellingActive){
		textPreview->font.setPointSize(fontSize);
    }
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
void GLModel::generateExtensionFaces(double distance){
    if (!_targetSelected)
        return;
    Hix::Features::Extension::extendMesh(_mesh, _targetMeshFace, distance);
	_targetSelected = false;
	updateMesh(true);
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
        if (qmlManager->currentFeature() != Hix::Features::FeatureEnum::LayerViewMode){
            // remove layer view components
            removeLayerViewComponents();
        }
        layerViewActive = false;
		
        break;
    case VIEW_MODE_LAYER:
        layerViewActive = true;
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


    closeCut();
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
	return faceSelectionActive() || qmlManager->currentFeature() != Hix::Features::FeatureEnum::LayerViewMode;
}
bool GLModel::faceSelectionActive() const
{
	return qmlManager->currentFeature() != Hix::Features::FeatureEnum::Extend || qmlManager->currentFeature() != Hix::Features::FeatureEnum::Layflat;
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
