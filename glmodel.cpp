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

    // set shader mode and color
	_meshMaterial.changeMode(Hix::Render::ShaderMode::SingleColor);
	_meshMaterial.setColor(Hix::Render::Colors::Default);

	qmlManager->addPart(getFileName(fname.toStdString().c_str()), ID);
	if (_filename != "" && (_filename.contains(".stl") || _filename.contains(".STL"))) {
		FileLoader::loadMeshSTL(loadMesh, _filename.toLocal8Bit().constData());
	}
	else if (_filename != "" && (_filename.contains(".obj") || _filename.contains(".OBJ"))) {
		FileLoader::loadMeshOBJ(loadMesh, _filename.toLocal8Bit().constData());
	}
	loadMesh->centerMesh();
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





void GLModel::changeColor(const QVector3D& color){
	_meshMaterial.setColor(color);
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
		changeColor(Hix::Render::Colors::OutOfBound);
	}
	else
	{
		if (qmlManager->isSelected(this))
		{
			changeColor(Hix::Render::Colors::Selected);
		}
		else
		{
			changeColor(Hix::Render::Colors::Default);
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
    labellingActive = from->labellingActive;
    extensionActive = from->extensionActive;
    cutActive = from->cutActive;
    hollowShellActive = from->hollowShellActive;
    shellOffsetActive = from->shellOffsetActive;
    layflatActive = from->layflatActive;
    layerViewActive = from->layerViewActive;
    scaleActive = from->scaleActive;

    // labelling info
    if (from->textPreview) {
        qDebug() << "copyModelAttributeFrom";
        if (!textPreview)
            textPreview = new Hix::Labelling::LabelModel(this, *(from->textPreview));
    }
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

// hollow shell part
void GLModel::indentHollowShell(double radius){
    qDebug() << "hollow shell called" << radius;
	if (!_targetSelected)
		return;
	auto meshVertices = targetMeshFace.meshVertices();
    QVector3D center = (
		meshVertices[0].localPosition() +
		meshVertices[1].localPosition() +
		meshVertices[2].localPosition())/3;
	HollowShell::hollowShell(_mesh, targetMeshFace, center, radius);
}

GLModel::~GLModel(){
}
void GLModel::initHitTest()
{
	addComponent(&_layer);
	_layer.setRecursive(false);

}

void GLModel::clicked(MouseEventData& pick, const Qt3DRender::QRayCasterHit& hit)
{
	auto suppMode = qmlManager->supportRaftManager().supportEditMode();
	if (!cutActive && !extensionActive && !labellingActive && !layflatActive &&
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
    targetMeshFace = _mesh->getFaces().cbegin() + hit.primitiveIndex();


	/// Extension Feature ///
	if (extensionActive && hit.localIntersection() != QVector3D(0, 0, 0)) {
		unselectMeshFaces();
		emit extensionSelect();
		selectMeshFaces();
	}

	/// Hollow Shell ///
	if (hollowShellActive) {
		qDebug() << "getting handle picker clicked signal hollow shell active";
		qDebug() << "found parent meshface";
		// translate hollowShellSphere to mouse position
		QVector3D v = hit.localIntersection();
        qmlManager->hollowShellSphereTransform->setTranslation(v + _transform.translation());


	}

	/// Lay Flat ///
	if (layflatActive && hit.localIntersection() != QVector3D(0, 0, 0)) {

		unselectMeshFaces();
		emit layFlatSelect();
		selectMeshFaces();
	}

	/// Manual Support ///
	if (suppMode == Hix::Support::EditMode::Manual && hit.localIntersection() != QVector3D(0, 0, 0)) {
		Hix::OverhangDetect::FaceOverhang newOverhang;
		newOverhang.coord = ptToRoot(hit.localIntersection());
		newOverhang.face = targetMeshFace;
		qmlManager->supportRaftManager().addSupport(newOverhang);
	}

	/// Labeling Feature ///
    if (labellingActive && hit.localIntersection() != QVector3D(0, 0, 0)) {
		if (textPreview != nullptr)
			textPreview->setEnabled(false);

		textPreview = new Hix::Labelling::LabelModel(this);

		if (textPreview && labellingActive) {
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
		!(	scaleActive ||
			shellOffsetActive ||
			extensionActive ||
			labellingActive ||
			layflatActive ||
			layerViewActive)	
		&&
		!(qmlManager->orientationActive ||
			qmlManager->rotateActive ||
			qmlManager->saveActive))
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
    if ( !layerViewActive)
        return;

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
	_meshMaterial.setParameterValue("height", QVariant::fromValue(h));
	/*m_layerMaterialRaftHeight->setValue(QVariant::fromValue(qmlManager->getLayerViewFlags() & LAYER_INFILL != 0 ?
                _mesh->z_min() :
                _mesh->z_max() + scfg->raft_thickness - scfg->support_base_height));*/
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
    qDebug() << "@@@@ getTexyChanged";
    if (text != "" && textPreview && labellingActive){
		textPreview->text = text;
    }
}

void GLModel::openLabelling()
{
    labellingActive = true;

    qmlManager->lastModelSelected();
    if (!qmlManager->isSelected(this)) {
        labellingActive = false;
    }

}

void GLModel::closeLabelling()
{
    if (!labellingActive)
        return;

    labellingActive = false;

    if (textPreview){
		textPreview->setEnabled(false);
    }
	_targetSelected = false;
	textPreview = nullptr;
//    stateChangeLabelling();
}

void GLModel::stateChangeLabelling() {
    qmlManager->keyboardHandlerFocus();
    (qmlManager->keyboardHandler)->setFocus(true);
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

/* make a new labellingTextPreview and apply label info's */
void GLModel::applyLabelInfo(QString text, QString fontName, bool isBold, int fontSize)
{
	qDebug() << "label apply";

    if (textPreview && labellingActive){
		textPreview->generateLabel(text, _mesh, targetMeshFace.localFn(), 0.025f);
		updateModelMesh();
    }
    }


void GLModel::generateText3DMesh()
{
    qDebug() << "generateText3DMesh @@@@@" << this << this;

    if (updateLock)
        return;
    updateLock = true;

    if (!textPreview){
        qDebug() << "no labellingTextPreview";
        QMetaObject::invokeMethod(qmlManager->labelPopup, "noModel");
        return;
    }

    qmlManager->openProgressPopUp();

    qmlManager->setProgress(0.1f);

	_targetSelected = false;
	*_mesh += *textPreview->getMesh();

    qmlManager->setProgress(0.5f);

    updateModelMesh();
	textPreview = nullptr;

    qmlManager->setProgress(1.0f);
}

// for extension

void GLModel:: unselectMeshFaces(){
	selectedFaces.clear();
}
void GLModel::selectMeshFaces(){
	selectedFaces.clear();
	QVector3D normal = targetMeshFace.localFn();
	_mesh->findNearSimilarFaces(normal, targetMeshFace, selectedFaces);
	updateMesh(_mesh, true);
}
void GLModel::generateExtensionFaces(double distance){
    if (!_targetSelected)
        return;
    Hix::Features::Extension::extendMesh(_mesh, targetMeshFace, distance);
	_targetSelected = false;
	updateMesh(_mesh, true);
}

void GLModel::generateLayFlat(){
    if(!_targetSelected)
        return;
	unselectMeshFaces();
	constexpr QVector4D worldBot(0, 0, -1, 1);
	QVector3D localBotNorml(toLocalCoord(worldBot));
	auto rotationTo = QQuaternion::rotationTo(targetMeshFace.localFn(), localBotNorml);
	_transform.setRotation(_transform.rotation() * rotationTo);
	emit resetLayflat();
}


// for shell offset
void GLModel::generateShellOffset(double factor){
    //saveUndoState();
    qDebug() << "generate shell Offset";
    qmlManager->openProgressPopUp();
    QString original_filename = _filename;

    cutMode = 1;
    cutFillMode = 1;
    shellOffsetFactor = factor;

    modelCut();

}


void GLModel::openLayflat(){
    layflatActive = true;
	_meshMaterial.changeMode(Hix::Render::ShaderMode::PerPrimitiveColor);
	updateMesh(_mesh, true);
    qmlManager->lastModelSelected();
    if (!qmlManager->isSelected(this))
        layflatActive = false;
}
void GLModel::closeLayflat(){
    if (!layflatActive)
        return;
    layflatActive = false;
	_meshMaterial.changeMode(Hix::Render::ShaderMode::SingleColor);
	rotateDone();
	updateMesh(_mesh, true);
    unselectMeshFaces();
	_targetSelected = false;
}
void GLModel::openExtension(){
    extensionActive = true;
	_meshMaterial.changeMode(Hix::Render::ShaderMode::PerPrimitiveColor);
	updateMesh(_mesh, true);
    qmlManager->lastModelSelected();
    if (!qmlManager->isSelected(this))
        extensionActive = false;

}

void GLModel::closeExtension(){
    if (!extensionActive)
        return;
	_meshMaterial.changeMode(Hix::Render::ShaderMode::SingleColor);
	updateMesh(_mesh, true);
    extensionActive = false;
    unselectMeshFaces();
	_targetSelected = false;
}


void GLModel::openScale(){
    scaleActive = true;
    qmlManager->sendUpdateModelInfo();
    //QMetaObject::invokeMethod(qmlManager->scalePopup, "updateSizeInfo", Q_ARG(QVariant, _mesh->x_max()-_mesh->x_min()), Q_ARG(QVariant, _mesh->y_max()-_mesh->y_min()), Q_ARG(QVariant, _mesh->z_max()-_mesh->z_min()));
}

void GLModel::closeScale(){
    if (!scaleActive)
        return;

    scaleActive = false;
    qmlManager->sendUpdateModelInfo();
    qDebug() << "close scale";
}

void GLModel::openCut(){
    cutActive = true;
}

void GLModel::closeCut(){
    qDebug() << "closecut called";

    if (!cutActive)
        return;

    cutActive = false;
    removePlane();

}

void GLModel::openHollowShell(){
    qDebug() << "open HollowShell called";
    hollowShellActive = true;
    qmlManager->hollowShellSphereEntity->setProperty("visible", true);
}

void GLModel::closeHollowShell(){
    qDebug() << "close HollowShell called";

    if (!hollowShellActive)
        return;

    hollowShellActive = false;
    qmlManager->hollowShellSphereEntity->setProperty("visible", false);
}

void GLModel::openShellOffset(){
    qDebug() << "openShelloffset";
    shellOffsetActive = true;
    generatePlane(1);

}

void GLModel::closeShellOffset(){
    qDebug() << "closeShelloffset";

    if (!shellOffsetActive)
        return;

    shellOffsetActive = false;
    removePlane();
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
        if (layerViewActive){
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
			_meshMaterial.changeMode(Hix::Render::ShaderMode::PerPrimitiveColor);
		}
		else
		{
			_meshMaterial.changeMode(Hix::Render::ShaderMode::SingleColor);
		}
		break;
	case VIEW_MODE_LAYER:
		_meshMaterial.changeMode(Hix::Render::ShaderMode::LayerMode);
		break;
	}




}


void GLModel::inactivateFeatures(){
    /*labellingActive = false;
    extensionActive = false;
    cutActive = false;
    hollowShellActive = false;
    shellOffsetActive = false;
    layflatActive = false;
    manualSupportActive = false;
    layerViewActive = false;
    supportViewActive = false;
    scaleActive = false;*/

    closeLabelling();
    closeExtension();
    closeCut();
    closeHollowShell();
    closeShellOffset();
    closeLayflat();
    closeScale();
    //layerViewActive = false; //closeLayerView();
    //supportViewActive = false; //closeSupportView();
    //parentModel->changeViewMode(VIEW_MODE_OBJECT);
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
	return faceSelectionActive() || layerViewActive;
}
bool GLModel::faceSelectionActive() const
{
	return extensionActive || layflatActive;
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
