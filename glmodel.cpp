#include <QRenderSettings>
#include "glmodel.h"
#include <QString>
#include <QtMath>
#include <cfloat>
#include <exception>

#include "qmlmanager.h"
#include "feature/text3dgeometrygenerator.h"
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
	, _supportRaftManager(this)
    , filename(fname)
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
	if (filename != "" && (filename.contains(".stl") || filename.contains(".STL"))\
		&& loadMesh == nullptr) {
		FileLoader::loadMeshSTL(_mesh, filename.toLocal8Bit().constData());
	}
	else if (filename != "" && (filename.contains(".obj") || filename.contains(".OBJ"))\
		&& loadMesh == nullptr) {
		FileLoader::loadMeshOBJ(_mesh, filename.toLocal8Bit().constData());
	}
	else {
		delete _mesh;
		_mesh = loadMesh;
	}
	//applyGeometry();
	// 승환 25%
	qmlManager->setProgress(0.23);
	setMesh(_mesh);
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



void GLModel::moveModelMesh(QVector3D direction, bool update) {
	_mesh->vertexMove(direction);
	qDebug() << "moved vertex";
	if (update)
	{
		updateModelMesh();
	}
}


void GLModel::moveDone()
{
	_mesh->vertexMove(getTransform()->translation());
	setTranslation(QVector3D(0, 0, 0));
	updatePrintable();
	updateModelMesh();
}
void GLModel::rotationDone()
{
	////auto rotCenter = _mesh->bounds().centre();
	//_mesh->vertexApplyTransformation(m_transform);
	////reset transform by setting identity matrix;
	//m_transform.setMatrix(QMatrix4x4());
	_mesh->vertexRotate(quatToMat(m_transform.rotation()).inverted());
	m_transform.setRotationX(0);
	m_transform.setRotationY(0);
	m_transform.setRotationZ(0);
	setZToBed();
	_mesh->vertexMove(m_transform.translation());
	m_transform.setTranslation(QVector3D(0, 0, 0));
	updatePrintable();
	updateModelMesh();
}



void GLModel::rotateAroundPt(QVector3D& rot_center, float X, float Y, float Z)
{
	QMatrix4x4 rot;
	rot = m_transform.rotateAround(rot_center, X, (QVector3D(1, 0, 0).toVector4D() * m_transform.matrix()).toVector3D());
	m_transform.setMatrix(m_transform.matrix() * rot);
	rot = m_transform.rotateAround(rot_center, Y, (QVector3D(0, 1, 0).toVector4D() * m_transform.matrix()).toVector3D());
	m_transform.setMatrix(m_transform.matrix() * rot);
	rot = m_transform.rotateAround(rot_center, Z, (QVector3D(0, 0, 1).toVector4D() * m_transform.matrix()).toVector3D());
	m_transform.setMatrix(m_transform.matrix() * rot);

	_mesh->vertexRotate(quatToMat(m_transform.rotation()).inverted());
	m_transform.setRotationX(0);
	m_transform.setRotationY(0);
	m_transform.setRotationZ(0);
	setZToBed();
	_mesh->vertexMove(m_transform.translation());
	m_transform.setTranslation(QVector3D(0, 0, 0));
	updateModelMesh();
}

void GLModel::rotateAroundPt(QVector3D& rot_center, const QVector3D& axis, float angle)
{
	QMatrix4x4 rot;
	rot = m_transform.rotateAround(rot_center, angle, axis);
	m_transform.setMatrix(m_transform.matrix() * rot);
	_mesh->vertexRotate(quatToMat(m_transform.rotation()).inverted());
	m_transform.setRotationX(0);
	m_transform.setRotationY(0);
	m_transform.setRotationZ(0);
	setZToBed();
	_mesh->vertexMove(m_transform.translation());
	m_transform.setTranslation(QVector3D(0, 0, 0));
	updateModelMesh();
}





void GLModel::scaleModelMesh(float scaleX, float scaleY, float scaleZ) {
	/* To fix center of the model */
	float centerX = (_mesh->x_max() + _mesh->x_min()) / 2;
	float centerY = (_mesh->y_max() + _mesh->y_min()) / 2;
	_mesh->vertexScale(scaleX, scaleY, scaleZ, centerX, centerY);
	updatePrintable();
	updateModelMesh();
}

void GLModel::setZToBed()
{
	auto translation = getTranslation();
	translation.setZ(-1.0f * getMesh()->z_min());
	setTranslation(translation);

}





void GLModel::changeColor(const QVector3D& color){
	_meshMaterial.setColor(color);
}

bool GLModel::isPrintable()const
{
	const auto& bedBound = scfg->bedBound();
	return bedBound.contains(_mesh->bounds());
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
    MeshRepair::repairMesh(_mesh);
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
    if (from->labellingTextPreview) {
        qDebug() << "copyModelAttributeFrom";
        if (!labellingTextPreview)
            labellingTextPreview = new LabellingTextPreview(this);
        labellingTextPreview->setFontName(from->labellingTextPreview->fontName);
        labellingTextPreview->setFontBold((from->labellingTextPreview->fontWeight == QFont::Bold)? true:false);
        labellingTextPreview->setFontSize(from->labellingTextPreview->fontSize);
        labellingTextPreview->setText(from->labellingTextPreview->text, from->labellingTextPreview->contentWidth);
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
    qDebug() << "** generateRLModel" << this;
    if (lmesh->getFaces().size() != 0){
        qmlManager->createModelFile(lmesh, filename+"_l");
        qDebug() << "leftmodel created";
    }
    // 승환 70%
    qmlManager->setProgress(0.72);
    if (rmesh->getFaces().size() != 0){
        qmlManager->createModelFile(rmesh, filename+"_r");
        qDebug() << "rightmodel created";
    }


    // 승환 90%
    GLModel* leftmodel = qmlManager->findGLModelByName(filename+"_l");
    GLModel* rightmodel = qmlManager->findGLModelByName(filename+"_r");
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

			qmlManager->createModelFile(offsetLeftMesh, leftmodel->filename);

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
	auto meshVertices = targetMeshFace->meshVertices();
    QVector3D center = (
		meshVertices[0]->position +
		meshVertices[1]->position + 
		meshVertices[2]->position)/3;
	HollowShell::hollowShell(_mesh, &*targetMeshFace, center, radius);
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

	if (!cutActive && !extensionActive && !labellingActive && !layflatActive && _supportRaftManager.supportEditMode() == Hix::Support::EditMode::None)// && !layerViewActive && !supportViewActive)
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


	if (extensionActive && hit.localIntersection() != QVector3D(0, 0, 0)) {
		unselectMeshFaces();
		emit extensionSelect();
		selectMeshFaces();
	}

	if (hollowShellActive) {
		qDebug() << "getting handle picker clicked signal hollow shell active";
		qDebug() << "found parent meshface";
		// translate hollowShellSphere to mouse position
		QVector3D v = hit.localIntersection();
        qmlManager->hollowShellSphereTransform->setTranslation(v + m_transform.translation());


	}

	if (layflatActive && hit.localIntersection() != QVector3D(0, 0, 0)) {

		unselectMeshFaces();
		emit layFlatSelect();
		selectMeshFaces();
	}

	if (_supportRaftManager.supportEditMode() == Hix::Support::EditMode::Manual && hit.localIntersection() != QVector3D(0, 0, 0)) {
		Hix::OverhangDetect::FaceOverhang newOverhang;
		newOverhang.first = hit.localIntersection();
		newOverhang.second = targetMeshFace;
		_supportRaftManager.addSupport(newOverhang);
	}

    if (labellingActive && hit.localIntersection() != QVector3D(0, 0, 0)) {
        if (labellingTextPreview == nullptr)
            labellingTextPreview = new LabellingTextPreview(this);


        if (labellingTextPreview && labellingTextPreview->isEnabled()) {
            labellingTextPreview->setTranslation(hit.localIntersection() + targetMeshFace->fn);
            labellingTextPreview->setNormal(targetMeshFace->fn);
            labellingTextPreview->planeSelected = true;
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
			cutActive ||
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
	_supportRaftManager.clear();
	setZToBed();
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

void GLModel::cutModeSelected(int type){

    qDebug() << "cut mode selected1" << type;
    cutMode = type;
	generatePlane(cutMode);
    return;
}

void GLModel::cutFillModeSelected(int type){
    cutFillMode = type;
    return;
}

void GLModel::getSliderSignal(double value){
    if (cutActive||shellOffsetActive){
        if (value == 0.0 || value == 1.8){
            isFlatcutEdge = true;
        }
        else {
            isFlatcutEdge = false;
        }
        float zlength = _mesh->z_max() - _mesh->z_min();
       
		_cuttingPlane->transform().setTranslation(QVector3D(0,0, _mesh->z_min() + value*zlength/1.8));

    } else if (hollowShellActive){
        // change radius of hollowShellSphere
        hollowShellRadius = value;
        qmlManager->hollowShellSphereMesh->setRadius(hollowShellRadius);

        qDebug() << "getting slider signal: current radius is " << value;
    }
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


void GLModel::getTextChanged(QString text, int contentWidth)
{
    qDebug() << "@@@@ getTexyChanged";
    if (labellingTextPreview && labellingTextPreview->isEnabled()){
        applyLabelInfo(text, contentWidth, labellingTextPreview->fontName, (labellingTextPreview->fontWeight==QFont::Bold)? true:false, labellingTextPreview->fontSize);
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

    if (labellingTextPreview){
        labellingTextPreview->planeSelected = false;
        labellingTextPreview->deleteLabel();
        labellingTextPreview->deleteLater();
        labellingTextPreview = nullptr;
    }
	_targetSelected = false;

//    stateChangeLabelling();
}

void GLModel::stateChangeLabelling() {
    qmlManager->keyboardHandlerFocus();
    (qmlManager->keyboardHandler)->setFocus(true);
}

void GLModel::getFontNameChanged(QString fontName)
{
    qDebug() << "@@@@ getFontNameChanged";
    if (labellingTextPreview && labellingTextPreview->isEnabled()){
        applyLabelInfo(labellingTextPreview->text, labellingTextPreview->contentWidth, fontName, (labellingTextPreview->fontWeight==QFont::Bold)? true:false, labellingTextPreview->fontSize);
    }
}

void GLModel::getFontBoldChanged(bool isbold){
    qDebug() << "@@@@ getBoldChanged";
    if (labellingTextPreview && labellingTextPreview->isEnabled()){
        applyLabelInfo(labellingTextPreview->text, labellingTextPreview->contentWidth, labellingTextPreview->fontName, isbold, labellingTextPreview->fontSize);

    }
}

void GLModel::getFontSizeChanged(int fontSize)
{
    qDebug() << "@@@@ getSizeChanged" << fontSize;
    if (labellingTextPreview && labellingTextPreview->isEnabled()){
        applyLabelInfo(labellingTextPreview->text, labellingTextPreview->contentWidth, labellingTextPreview->fontName, (labellingTextPreview->fontWeight==QFont::Bold)? true:false, fontSize);
    }
}

/* make a new labellingTextPreview and apply label info's */
void GLModel::applyLabelInfo(QString text, int contentWidth, QString fontName, bool isBold, int fontSize){
    QVector3D translation;
    bool selected = false;

    qDebug() << "applyLabelInfo +++++++++++++++++++++++++ " << text  << contentWidth << this;

    if (labellingTextPreview && labellingTextPreview->isEnabled()){
        translation = labellingTextPreview->translation;
        selected = labellingTextPreview->planeSelected;
        labellingTextPreview->deleteLabel();
        labellingTextPreview->deleteLater();
        labellingTextPreview = nullptr;
    }
    labellingTextPreview = new LabellingTextPreview(this);
    labellingTextPreview->setEnabled(true);

    labellingTextPreview->planeSelected = selected;

    qDebug() << "applyLabelInfo";
    if (_targetSelected) {
        qDebug() << "applyLabelInfo : target Selected";
        labellingTextPreview->updateChange(text, contentWidth, fontName, isBold, fontSize, translation,targetMeshFace->fn);
        //labellingTextPreview->setTranslation(translation);
        //labellingTextPreview->setNormal(parentModel->targetMeshFace->fn);
    }
    else {
        qDebug() << "applyLabelInfo : target unSelected";
        labellingTextPreview->updateChange(text, contentWidth, fontName, isBold, fontSize, labellingTextPreview->translation, QVector3D(0,0,0));
        }



}


void GLModel::generateText3DMesh()
{
    //qDebug() << "generateText3DMesh @@@@@" << this << this;
    if (updateLock)
        return;
    updateLock = true;


    if (!labellingTextPreview){
        qDebug() << "no labellingTextPreview";
        QMetaObject::invokeMethod(qmlManager->labelPopup, "noModel");
        return;
    }

    if (!labellingTextPreview->planeSelected) {
        qDebug() << "no planeSelected" <<labellingTextPreview->translation;
        QMetaObject::invokeMethod(qmlManager->labelPopup, "noModel");
        return;
    }

    qmlManager->openProgressPopUp();

    labellingTextPreview->planeSelected = false;


    qmlManager->setProgress(0.1);

    QVector3D* originalVertices = reinterpret_cast<QVector3D*>(vertexBuffer.data().data());
    int originalVerticesSize = vertexBuffer.data().size() / sizeof(float) / 3;

    QVector3D* vertices;
    int verticesSize;
    unsigned int* indices;
    int indicesSize;
    float depth = 0.5f;
    float scale = labellingTextPreview->ratioY * labellingTextPreview->scaleY;
    QVector3D translation = labellingTextPreview->translation;// + QVector3D(0,-0.3,0);

    Qt3DCore::QTransform transform, normalTransform;

    QVector3D normal = labellingTextPreview->normal;

    QVector3D ref = QVector3D(0, 0, 1);
    QVector3D tangent;
    if (normal == QVector3D(0,0,1) || normal == QVector3D(0,0,-1)){
        tangent = QVector3D(1,0,0);
    } else {
        tangent = QVector3D::crossProduct(normal, ref);
    }
    tangent.normalize();

    QVector3D binormal;
    if (normal == QVector3D(0,0,1) || normal == QVector3D(0,0,-1)){
        binormal = QVector3D(0,1,0);
    } else {
        binormal = QVector3D::crossProduct(tangent, normal);
    }
    binormal.normalize();

    QQuaternion quat = QQuaternion::fromAxes(tangent, normal, binormal) * QQuaternion::fromAxisAndAngle(QVector3D(0, 0, 1), 180)* QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), 90);

    transform.setScale(scale);
    transform.setRotation(quat);
    transform.setTranslation(translation);
    qmlManager->setProgress(0.3);

    QFont targetFont = QFont(labellingTextPreview->fontName, labellingTextPreview->fontSize, labellingTextPreview->fontWeight, false);
    QString targetText = labellingTextPreview->text;
    QVector3D targetNormal = labellingTextPreview->normal;

    if (labellingTextPreview){
        labellingTextPreview->planeSelected = false;
        labellingTextPreview->deleteLabel();
        labellingTextPreview->deleteLater();
        labellingTextPreview = nullptr;
    }
	_targetSelected = false;

    generateText3DGeometry(&vertices, &verticesSize,
                           &indices, &indicesSize,
                           targetFont,
                           targetText,
                           depth,
                           _mesh,
                           -targetNormal,
                           transform.matrix(),
                           normalTransform.matrix());


    qmlManager->setProgress(0.9);

    std::vector<QVector3D> outVertices;
    for (int i = 0; i < indicesSize / 3; ++i) {
        // Insert vertices in CCW order
        outVertices.push_back(vertices[2 * indices[3*i + 2] + 0]);
        outVertices.push_back(vertices[2 * indices[3*i + 1] + 0]);
        outVertices.push_back(vertices[2 * indices[3*i + 0] + 0]);
        _mesh->addFace(vertices[2 * indices[3*i + 2] + 0], vertices[2 * indices[3*i + 1] + 0], vertices[2 * indices[3*i + 0] + 0]);

    }

    updateModelMesh();

    qmlManager->setProgress(1);
}

// for extension

void GLModel:: unselectMeshFaces(){
	selectedFaces.clear();
}
void GLModel::selectMeshFaces(){
	selectedFaces.clear();
	QVector3D normal = targetMeshFace->fn;
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
	constexpr QVector3D toBottNormal(0, 0, -1);
	auto rotationTo = QQuaternion::rotationTo(targetMeshFace->fn, toBottNormal);
	auto rotCenter = _mesh->bounds().centre();
	QVector3D rotAxis;
	float angle;
	rotationTo.getAxisAndAngle(&rotAxis, &angle);
	rotateAroundPt(rotCenter, rotAxis, angle);
	setZToBed();
	emit resetLayflat();
}


// for shell offset
void GLModel::generateShellOffset(double factor){
    //saveUndoState();
    qDebug() << "generate shell Offset";
    qmlManager->openProgressPopUp();
    QString original_filename = filename;

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
	updatePrintable();
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
	return extensionActive || layflatActive || _supportRaftManager.supportEditMode() != Hix::Support::EditMode::None;
}
bool GLModel::raftSupportGenerated() const
{
	return _supportRaftManager.raftActive() || _supportRaftManager.supportActive();
}
Hix::Support::SupportRaftManager& GLModel::supportRaftManager()
{
	return _supportRaftManager;
}

const Hix::Support::SupportRaftManager& GLModel::supportRaftManager()const
{
	return _supportRaftManager;
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
