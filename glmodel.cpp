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
#include <feature/generatesupport.h>
#include <Qt3DCore/qpropertyupdatedchange.h>
#include <feature/generateraft.h>

const size_t POS_SIZE = 3;
const size_t NRM_SIZE = 3;
const size_t COL_SIZE = 3;
const size_t VTX_SIZE = POS_SIZE + NRM_SIZE + COL_SIZE;

#define ATTRIBUTE_SIZE_INCREMENT 200
#if defined(_DEBUG) || defined(QT_DEBUG)
#define _STRICT_GLMODEL
#endif

using namespace Utils::Math;
using namespace Hix::Engine3D;
using namespace Hix::Input;
GLModel::GLModel(QObject* mainWindow, QNode *parent, Mesh* loadMesh, QString fname, int id)
    : QEntity(parent)
    , filename(fname)
    , mainWindow(mainWindow)
    , v_cnt(0)
    , f_cnt(0)
	, _currentVisibleMesh(nullptr)
    , cutMode(1)
    , layerMesh(nullptr)
    , layerInfillMesh(nullptr)
    , layerSupportMesh(nullptr)
    , layerRaftMesh(nullptr)
    , slicer(nullptr)
    , ID(id)
    , m_meshMaterial(nullptr)
	, m_geometryRenderer(this)
    , m_geometry(this)
    , vertexBuffer(Qt3DRender::QBuffer::VertexBuffer, this)
	, indexBuffer(Qt3DRender::QBuffer::IndexBuffer, this)
	, positionAttribute(this)
	, normalAttribute(this)
	, colorAttribute(this)
	, indexAttribute(this)


{
    connect(&futureWatcher, SIGNAL(finished()), this, SLOT(slicingDone()));
    qDebug() << "new model made _______________________________"<<this<< "parent:"<<parent;
	_layer.setRecursive(true);
	m_meshMaterial = new QPhongMaterial();
	m_meshVertexMaterial = new QPerVertexColorMaterial();
	addComponent(&_layer);
	addComponent(&m_transform);
	addComponent(m_meshMaterial);
	//addComponent(&m_geometryRenderer);

    //initialize vertex buffers etc
    vertexBuffer.setUsage(Qt3DRender::QBuffer::DynamicDraw);
	indexBuffer.setUsage(Qt3DRender::QBuffer::DynamicDraw);
	vertexBuffer.setAccessType(	Qt3DRender::QBuffer::AccessType::ReadWrite);
	indexBuffer.setAccessType(	Qt3DRender::QBuffer::AccessType::ReadWrite);

    positionAttribute.setAttributeType(QAttribute::VertexAttribute);
    positionAttribute.setBuffer(&vertexBuffer);
    positionAttribute.setDataType(QAttribute::Float);
    positionAttribute.setDataSize(POS_SIZE);
    positionAttribute.setByteOffset(0);
    positionAttribute.setByteStride(VTX_SIZE);
    positionAttribute.setCount(0);
    positionAttribute.setName(QAttribute::defaultPositionAttributeName());

    normalAttribute.setAttributeType(QAttribute::VertexAttribute);
    normalAttribute.setBuffer(&vertexBuffer);
    normalAttribute.setDataType(QAttribute::Float);
    normalAttribute.setDataSize(NRM_SIZE);
    normalAttribute.setByteOffset(POS_SIZE * sizeof(float));
    normalAttribute.setByteStride(VTX_SIZE);
    normalAttribute.setCount(0);
    normalAttribute.setName(QAttribute::defaultNormalAttributeName());

    colorAttribute.setAttributeType(QAttribute::VertexAttribute);
    colorAttribute.setBuffer(&vertexBuffer);
    colorAttribute.setDataType(QAttribute::Float);
    colorAttribute.setDataSize(COL_SIZE);
    colorAttribute.setByteOffset((NRM_SIZE + POS_SIZE) * sizeof(float));
    colorAttribute.setByteStride(VTX_SIZE);
    colorAttribute.setCount(0);
    colorAttribute.setName(QAttribute::defaultColorAttributeName());

	indexAttribute.setVertexBaseType(QAttribute::VertexBaseType::UnsignedInt);
	indexAttribute.setAttributeType(QAttribute::IndexAttribute);
	indexAttribute.setBuffer(&indexBuffer);
	indexAttribute.setDataType(QAttribute::UnsignedInt);
	indexAttribute.setDataSize(1);
	indexAttribute.setByteOffset(0);
	indexAttribute.setByteStride(0);
	indexAttribute.setCount(0);
	indexAttribute.setVertexSize(1);



	m_geometryRenderer.setInstanceCount(1);
	m_geometryRenderer.setFirstVertex(0);
	m_geometryRenderer.setFirstInstance(0);
    m_geometryRenderer.setPrimitiveType(QGeometryRenderer::Triangles);
    m_geometryRenderer.setGeometry(&m_geometry);
	m_geometryRenderer.setVertexCount(0);

	m_geometry.addAttribute(&positionAttribute);
	m_geometry.addAttribute(&normalAttribute);
	m_geometry.addAttribute(&colorAttribute);
	m_geometry.addAttribute(&indexAttribute);




    // generates shadow model for object picking
  
	// Add to Part List
	qmlManager->addPart(getFileName(fname.toStdString().c_str()), ID);



	generateLayerViewMaterial();

	this->changecolor(0);
	if (filename != "" && (filename.contains(".stl") || filename.contains(".STL"))\
		&& loadMesh == nullptr) {
		_mesh = new Mesh();
		FileLoader::loadMeshSTL(_mesh, filename.toLocal8Bit().constData());
	}
	else if (filename != "" && (filename.contains(".obj") || filename.contains(".OBJ"))\
		&& loadMesh == nullptr) {
		_mesh = new Mesh();
		FileLoader::loadMeshOBJ(_mesh, filename.toLocal8Bit().constData());
	}
	else {
		_mesh = loadMesh;
	}

	// 승환 25%
	qmlManager->setProgress(0.23);
	clearMem();
	setMesh(_mesh);
	//applyGeometry();

	// 승환 50%
	qmlManager->setProgress(0.49);
	//Qt3DExtras::QDiffuseMapMaterial* diffuseMapMaterial = new Qt3DExtras::QDiffuseMapMaterial();


	qDebug() << "created original model";

	//repairMesh(_mesh);
	//addShadowModel(_mesh);

	// 승환 75%
	qmlManager->setProgress(0.73);

	QObject::connect(this, SIGNAL(bisectDone(Mesh*, Mesh*)), this, SLOT(generateRLModel(Mesh*, Mesh*)));
	QObject::connect(this, SIGNAL(_generateSupport()), this, SLOT(generateSupport()));
	QObject::connect(this, SIGNAL(_updateModelMesh()), this, SLOT(updateModelMesh()));

	qDebug() << "created shadow model";

	ft = new featureThread(this, 0);
	qDebug() << "adding part " << fname.toStdString().c_str();


	// reserve cutting points, contours
	sphereEntity.reserve(50);
	sphereMesh.reserve(50);
	sphereMaterial.reserve(50);
	sphereTransform.reserve(50);
	sphereObjectPicker.reserve(50);
	cuttingPoints.reserve(50);
	cuttingContourCylinders.reserve(50);
}

void GLModel::changecolor(int mode){
    if (mode == -1) mode = colorMode;
    switch(mode){
    case 0: // default
        m_meshMaterial->setAmbient(QColor(130,130,140));
        //m_meshMaterial->setDiffuse(QColor(131,206,220));
        m_meshMaterial->setDiffuse(QColor(97,185,192));
        m_meshMaterial->setSpecular(QColor(150,150,150));
        m_meshMaterial->setShininess(0.0f);

        /*m_meshAlphaMaterial->setAmbient(QColor(130,130,140));;
        m_meshAlphaMaterial->setDiffuse(QColor(131,206,220));
        m_meshAlphaMaterial->setDiffuse(QColor(97,185,192));
        m_meshAlphaMaterial->setSpecular(QColor(0,0,0));
        m_meshAlphaMaterial->setShininess(0.0f);*/
        colorMode = 0;
        break;
    case 1:
        m_meshMaterial->setAmbient(QColor(230,230,230));
        //m_meshMaterial->setDiffuse(QColor(100,255,100));
        m_meshMaterial->setDiffuse(QColor(130,208,125));
        m_meshMaterial->setSpecular(QColor(150,150,150));
        m_meshMaterial->setShininess(0.0f);

        /*m_meshAlphaMaterial->setDiffuse(QColor(100,255,100));
        m_meshAlphaMaterial->setDiffuse(QColor(130,208,125));
        m_meshAlphaMaterial->setSpecular(QColor(0,0,0));
        m_meshAlphaMaterial->setShininess(0.0f);*/
        colorMode = 1;
        break;
    case 2:
        m_meshMaterial->setAmbient(QColor(0,0,0));
        //m_meshMaterial->setDiffuse(QColor(205,84,84));
        break;
    case 3:
        m_meshMaterial->setAmbient(QColor(160,160,160));;
        break;
    }
}
bool GLModel::modelSelectChangable(){
    bool result = false;
    qDebug() << cutActive << extensionActive << labellingActive << layflatActive << isMoved;
    if (!cutActive && !extensionActive && !labellingActive && !layflatActive && !manualSupportActive && !isMoved)
        result = true;

    return result;
}

void GLModel::checkPrintingArea(){
    float printing_x = scfg->bed_x;
    float printing_y = scfg->bed_y;
    float printing_z = 120;
    float printing_safegap = 1;
    // is it inside the printing area or not?
    QVector3D tmp = m_transform.translation();
    if ((tmp.x() + _mesh->x_min()) < printing_safegap - printing_x/2 |
        (tmp.x() + _mesh->x_max()) > printing_x/2 - printing_safegap|
        (tmp.y() + _mesh->y_min()) <  printing_safegap - printing_y/2|
        (tmp.y() + _mesh->y_max()) > printing_y/2 - printing_safegap|
        (tmp.z() + _mesh->z_max()) > printing_z){
        this->changecolor(2);
    } else {
        this->changecolor(-1);
        this->changecolor(3);
    }
//    qDebug() << tmp << _mesh->x_max() << _mesh->x_min() << _mesh->y_max() << _mesh->y_min() << _mesh->z_max();
}

void GLModel::saveUndoState(){
    saveUndoState_internal();
    //QFuture<void> future = QtConcurrent::run(this, &saveUndoState_internal);
}

void GLModel::saveUndoState_internal(){
    // need to change to memcpy or something
    qDebug () << "save prev mesh";

    // reset slicing view since model has been updated
    slicer = nullptr;
	auto temp_prev_mesh =  _mesh->saveUndoState(m_transform);


    // for model cut, shell offset
	if (leftModel)
	{
		leftModel->_mesh->setPrevMesh(temp_prev_mesh);
	}
	if (rightModel)
	{
		rightModel->_mesh->setPrevMesh(temp_prev_mesh);
	}
}

void GLModel::loadUndoState(){
    //while (updateLock); // if it is not locked
    if (updateLock)
        return;
    updateLock = true;
    qDebug() << this << "achieved lock";

    QMetaObject::invokeMethod(qmlManager->boxUpperTab, "all_off");

    if (_mesh->getPrev() != nullptr){
        qDebug() << "load undo state";
        if (twinModel != NULL && _mesh->getPrev() == twinModel->_mesh->getPrev()){ // same parent, cut generated
            qmlManager->deleteModelFile(twinModel->ID);
        }
        if (_mesh->time.isNull() || _mesh->m_translation == QVector3D(0,0,0) || _mesh->m_matrix.isIdentity()){ // most recent job
            _mesh->time = QTime::currentTime();
            _mesh->m_translation = m_transform.translation();
            _mesh->m_matrix = m_transform.matrix();
        }

        _mesh = _mesh->getPrev();

        // move model _mesh and rotate model _mesh
        /*_mesh->vertexMove(_mesh->m_translation);
        _mesh->vertexRotate(_mesh->m_matrix);*/
        if (!_mesh->m_matrix.isIdentity()){
            _mesh->vertexRotate(_mesh->m_matrix.inverted());
            _mesh->m_matrix = QMatrix4x4();
        }
        m_transform.setTranslation(_mesh->m_translation);
        m_transform.setRotationX(0);
        m_transform.setRotationY(0);
        m_transform.setRotationZ(0);
        emit _updateModelMesh();
    } else {
        updateLock = false;
        qDebug() << "no undo state";
        int saveCnt = (_mesh->getFaces().size()>100000)? 3: 10;
        qmlManager->openResultPopUp("Undo state doesn't exist.","","Maximum "+QVariant(saveCnt).toString().toStdString()+" states are saved for this model.");
    }
}

void GLModel::loadRedoState(){
    if (_mesh->getNext() != nullptr){
        qDebug() << "load Redo State";
        _mesh = _mesh->getNext();
        // move model _mesh and rotate model _mesh
        /*_mesh->vertexMove(_mesh->m_translation);
        _mesh->vertexRotate(_mesh->m_matrix);*/
        if (!_mesh->m_matrix.isIdentity()){
            _mesh->vertexRotate(_mesh->m_matrix.inverted());
            _mesh->m_matrix = QMatrix4x4();
        }


        m_transform.setTranslation(_mesh->m_translation);
        m_transform.setRotationX(0);
        m_transform.setRotationY(0);
        m_transform.setRotationZ(0);
        emit _updateModelMesh();
    } else {
        qDebug() << "no redo status";
    }
}
void GLModel::repairMesh()
{
    MeshRepair::repairMesh(_mesh);
    emit _updateModelMesh();
}
void GLModel::moveModelMesh(QVector3D direction, bool update){
    _mesh->vertexMove(direction);
    /*if (shadowModel != NULL)
        moveModelMesh(direction);*/
    qDebug() << "moved vertex";
    if(update)
    {
        emit _updateModelMesh();
    }
}
void GLModel::rotationDone()
{
    _mesh->vertexRotate(quatToMat(m_transform.rotation()).inverted());
    m_transform.setRotationX(0);
    m_transform.setRotationY(0);
    m_transform.setRotationZ(0);

    _mesh->vertexMove(m_transform.translation());
    m_transform.setTranslation(QVector3D(0,0,0));
    emit _updateModelMesh();
}


void GLModel::rotateByNumber(QVector3D& rot_center, int X, int Y, int Z)
{
    QMatrix4x4 rot;
    rot = m_transform.rotateAround(rot_center,X,(QVector3D(1,0,0).toVector4D()*m_transform.matrix()).toVector3D());
    m_transform.setMatrix(m_transform.matrix()*rot);
    rot = m_transform.rotateAround(rot_center,Y,(QVector3D(0,1,0).toVector4D()*m_transform.matrix()).toVector3D());
    m_transform.setMatrix(m_transform.matrix()*rot);
    rot = m_transform.rotateAround(rot_center,Z,(QVector3D(0,0,1).toVector4D()*m_transform.matrix()).toVector3D());
    m_transform.setMatrix(m_transform.matrix()*rot);

    _mesh->vertexRotate(quatToMat(m_transform.rotation()).inverted());
    m_transform.setRotationX(0);
    m_transform.setRotationY(0);
    m_transform.setRotationZ(0);
    _mesh->vertexMove(m_transform.translation());
    m_transform.setTranslation(QVector3D(0,0,0));
    emit _updateModelMesh();
}

void GLModel::rotateModelMesh(QMatrix4x4 matrix, bool update){
    _mesh->vertexRotate(matrix);
    if(update)
    {
        emit _updateModelMesh();
    }
}


void GLModel::rotateModelMesh(int Axis, float Angle, bool update){
    Qt3DCore::QTransform tmp;
    switch(Axis){
    case 1:{
        tmp.setRotationX(Angle);
        break;
    }
    case 2:{
        tmp.setRotationY(Angle);
        break;
    }
    case 3:{
        tmp.setRotationZ(Angle);
        break;
    }
    }
    rotateModelMesh(tmp.matrix(),update);
}



void GLModel::scaleModelMesh(float scaleX, float scaleY, float scaleZ){
    /* To fix center of the model */
    float centerX = (_mesh->x_max() + _mesh->x_min())/2;
    float centerY = (_mesh->y_max() + _mesh->y_min())/2;
    _mesh->vertexScale(scaleX, scaleY, scaleZ, centerX, centerY);
    /*if (shadowModel != NULL)
        scaleModelMesh(scale);*/

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
    manualSupportActive = from->manualSupportActive;
    layerViewActive = from->layerViewActive;
    supportViewActive = from->supportViewActive;
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

QTime GLModel::getPrevTime()
{
	return _mesh->getPrevTime();
}

QTime GLModel::getNextTime()
{
	return _mesh->getNextTime();
}

void GLModel::updateModelMesh(){
    // shadowUpdate updates shadow model of current Model
    QMetaObject::invokeMethod(qmlManager->boxUpperTab, "disableUppertab");
    QMetaObject::invokeMethod(qmlManager->boxLeftTab, "disableLefttab");
    QMetaObject::invokeMethod((QObject*)qmlManager->scene3d, "disableScene3D");
    qDebug() << "update Model Mesh";
    // reinitialize with updated mesh

    int viewMode = qmlManager->getViewMode();
    switch( viewMode ) {
    case VIEW_MODE_OBJECT:
		updateMesh(_mesh);

        m_transform.setTranslation(QVector3D(m_transform.translation().x(),m_transform.translation().y(),-_mesh->z_min()));
        break;
    case VIEW_MODE_SUPPORT:
		updateMesh(_mesh);
        if (supportMesh != nullptr)
        {
			appendMesh(supportMesh);
            qDebug() << "ADDED support mesh";
        }
        if (raftMesh != nullptr)
        {
            appendMesh(raftMesh);
            qDebug() << "ADDED raft mesh";
        }

        m_transform.setTranslation(QVector3D(m_transform.translation().x(),m_transform.translation().y(),-_mesh->z_min() + scfg->raft_thickness + scfg->support_base_height));

        break;
    case VIEW_MODE_LAYER:
        qDebug() << "in the glmodel view mode layer" << qmlManager->getLayerViewFlags() << " " << (qmlManager->getLayerViewFlags() & LAYER_SUPPORTERS);
        if( layerMesh != nullptr ) {
            int faces = layerMesh->getFaces().size()*2 +
                    (qmlManager->getLayerViewFlags() & LAYER_INFILL != 0 ? layerInfillMesh->getFaces().size()*2 : 0) +
                    (qmlManager->getLayerViewFlags() & LAYER_SUPPORTERS != 0 ? supportMesh->getFaces().size()*2 : 0) +
                    (qmlManager->getLayerViewFlags() & LAYER_RAFT != 0 ? layerRaftMesh->getFaces().size()*2 : 0);
			updateMesh(layerMesh);
            if( qmlManager->getLayerViewFlags() & LAYER_INFILL ) {
				appendMesh(layerInfillMesh);
            }
            if( qmlManager->getLayerViewFlags() & LAYER_SUPPORTERS ) {
				appendMesh(supportMesh);
            }
            if( qmlManager->getLayerViewFlags() & LAYER_RAFT) {
                appendMesh(raftMesh);
            }
        } else {
            int faces = _mesh->getFaces().size()*2 + ((supportMesh!=nullptr) ? supportMesh->getFaces().size()*2:0);
			updateMesh(_mesh);
            if (supportMesh != nullptr){
				appendMesh(supportMesh);
                qDebug() << "ADDED support mesh";
        }
            if (raftMesh != nullptr){
                appendMesh(raftMesh);
                qDebug() << "ADDED raft mesh";
            }
        }

        m_transform.setTranslation(QVector3D(m_transform.translation().x(),m_transform.translation().y(),-_mesh->z_min() + scfg->raft_thickness + scfg->support_base_height));
        break;
    }

    qmlManager->sendUpdateModelInfo();
    checkPrintingArea();
    //QMetaObject::invokeMethod(qmlManager->scalePopup, "updateSizeInfo", Q_ARG(QVariant, _mesh->x_max()-_mesh->x_min()), Q_ARG(QVariant, _mesh->y_max()-_mesh->y_min()), Q_ARG(QVariant, _mesh->z_max()-_mesh->z_min()));
    qDebug() << "model transform :" <<m_transform.translation() << _mesh->x_max() << _mesh->x_min() << _mesh->y_max() << _mesh->y_min() << _mesh->z_max() << _mesh->z_min();


    // create new object picker, shadowmodel, remove prev shadowmodel
    //if (shadowUpdate){
    //    switch( viewMode ) {
    //        case VIEW_MODE_OBJECT:
    //            updateShadowModelImpl();
    //            break;
    //        case VIEW_MODE_SUPPORT:
    //            updateShadowModelImpl();
    //            m_transform.setTranslation(m_transform.translation()+QVector3D(0,0,scfg->raft_thickness));
    //            break;
    //        case VIEW_MODE_LAYER:
    //            updateShadowModelImpl();
    //            //addShadowModel(_mesh);
    //            break;
    //    }
    //}
    updateLock = false;
    qDebug() << this << "released lock";
    QMetaObject::invokeMethod(qmlManager->boxUpperTab, "enableUppertab");
    QMetaObject::invokeMethod(qmlManager->boxLeftTab, "enableLefttab");
    QMetaObject::invokeMethod((QObject*)qmlManager->scene3d, "enableScene3D");
}

void GLModel::slicingDone(){
    QString result = futureWatcher.result()->slicingInfo;
    slicingInfo = result;
    qmlManager->slicingData->setProperty("visible", true);

    slicer = futureWatcher.result();
    emit _generateSupport();
}

featureThread::featureThread(GLModel* glmodel, int type){
    qDebug() << "feature thread created" << type;
    m_glmodel = glmodel;
    optype = type;

    // enable features
    ot = new autoorientation();
    ct = new modelcut();
    ar = new autoarrange();
    ste = new STLexporter();
    se = new SlicingEngine();

    QObject::connect(se, SIGNAL(updateModelInfo(int,int,QString,float)), qmlManager, SLOT(sendUpdateModelInfo(int,int,QString,float)));

}

void featureThread::setTypeAndRun(int type){
    optype = type;
    run();
}

void featureThread::setTypeAndRun(int type, double value1=1, double value2=1, double value3=1){
    optype = type;
    arg1 = value1;
    arg2 = value2;
    arg3 = value3;
    qDebug() << "settypeandrun 4";
    run();
}

void featureThread::setTypeAndRun(int type, QVariant map){
    optype = type;
    data = map;
    run();
}

void featureThread::setTypeAndStart(int type){
    optype = type;
    start();
}

void featureThread::run(){
    qDebug() << "run()";
    switch (optype){
        case ftrOpen:
            {
                break;
            }
        case ftrSave:
            {
                break;
                /*qDebug() << "file save called";
                if (qmlManager->selectedModels.size() == 1 && qmlManager->selectedModels[0] == nullptr) {
                    qDebug() << "need popup";
                    return;
                }
                QString fileName = QFileDialog::getSaveFileName(nullptr, tr("Save to STL file"), "", tr("3D Model file (*.stl)"));
                if(fileName == "")
                    return;
                qmlManager->openProgressPopUp();
                QFuture<void> future = QtConcurrent::run(ste, &STLexporter::exportSTL, m_glmodel->_mesh,fileName);
                break;
                */
            }
        case ftrExport: // support view & layer view & export // on export, temporary variable = true;
            {

                if (m_glmodel->updateLock){
                    qDebug() << "still updating";
                    return;
                }
                m_glmodel->updateLock = true;
                // look for data if it is temporary
                QVariantMap config = data.toMap();
                bool isTemporary = false;
                for (QVariantMap::const_iterator iter = config.begin(); iter != config.end(); ++iter){
                    if (!strcmp(iter.key().toStdString().c_str(), "temporary")){
                        qDebug() << "iter value : " << iter.value().toString();
                        if (iter.value().toString() == "true"){
                            isTemporary = true;
                        }
                    }
                }

                QString fileName;
                if (! isTemporary){ // export view
                    qDebug() << "export to file";
                    fileName = QFileDialog::getSaveFileName(nullptr, tr("Export sliced file"), "");
                    //ste->exportSTL(m_glmodel->_mesh, fileName);
                    if(fileName == "")
                        return;
                } else { // support view & layerview
                    qDebug() << "export to temp file";
                    fileName =  QDir::tempPath();
                    qDebug() << fileName;
                }

                // slice file
                qmlManager->openProgressPopUp();
                QFuture<Slicer*> future = QtConcurrent::run(
					se, &SlicingEngine::slice, data, m_glmodel->_mesh, m_glmodel->supportMesh, 
					m_glmodel->raftMesh, fileName + "/" + m_glmodel->filename.split("/").last() );
                m_glmodel->futureWatcher.setFuture(future);
                m_glmodel->updateLock = false;
                break;
            }
        case ftrMove:
            {
                break;
            }
        case ftrRotate:
            {
                break;
            }
        case ftrLayFlat:
            {
                break;
            }
        case ftrArrange:
            {
                //emit (m_glmodel->arsignal)->runArrange();
                //unused, signal from qml goes right into QmlManager.runArrange
                break;
            }
        case ftrOrient:
            {
                qDebug() << "ftr orientation run";
                //while (m_glmodel->updateLock); // if it is not locked
                if (m_glmodel->updateLock)
                    return;
                m_glmodel->updateLock = true;

                m_glmodel->saveUndoState();
                qmlManager->openProgressPopUp();
                qDebug() << "tweak start";
                rotateResult* rotateres= ot->Tweak(m_glmodel->_mesh,true,45,&m_glmodel->appropriately_rotated);
                qDebug() << "got rotate result";
                if (rotateres == NULL)
                    break;
                else
                    m_glmodel->rotateModelMesh(rotateres->R);
                break;
            }
        case ftrScale:
            {
                m_glmodel->saveUndoState();
                float scaleX = arg1;
                float scaleY = arg2;
                float scaleZ = arg3;
                m_glmodel->scaleModelMesh(scaleX, scaleY, scaleZ);
                break;
            }
        case ftrRepair:
            {
                m_glmodel->saveUndoState();
                qmlManager->openProgressPopUp();
                MeshRepair::repairMesh(m_glmodel->_mesh);

                emit m_glmodel->_updateModelMesh();
                break;
            }
        case ftrCut:
            {
                m_glmodel->modelCut();
                break;
            }
        case ftrShellOffset:
            {
                //m_glmodel->saveUndoState();
                auto offsetMesh = ShellOffset::shellOffset(m_glmodel->_mesh, -0.5);
                qmlManager->deleteModelFile(m_glmodel->ID);
                qmlManager->createModelFile(offsetMesh, m_glmodel->filename);
                break;
            }
        case ftrExtend:
            {

                break;
            }
        case ftrManualSupport:
            {
                break;
            }
        case ftrLabel:
            {
                break;
            }
    }
}

arrangeSignalSender::arrangeSignalSender(){

}

void GLModel::clearMem(){
    QByteArray newVertexArray;
	QByteArray newIdxArray;

    vertexBuffer.setData(newVertexArray);
	indexBuffer.setData(newIdxArray);

	positionAttribute.setCount(0);
	normalAttribute.setCount(0);
	colorAttribute.setCount(0);
	indexAttribute.setCount(0);

}

//****************************** Mesh -> QGeometry helper inlines *******************//
inline void eraseBufferData(QAttribute& attr, Qt3DRender::QBuffer& buffer, size_t amount, size_t count)
{
	QByteArray copy = buffer.data();
	copy.remove(copy.size() - amount, amount);
	buffer.setData(copy);
	attr.setCount(attr.count() - count);
	return;
}
void inline attrBufferResize(QAttribute & attr, Qt3DRender::QBuffer & attrBuffer, size_t dataUnitSize, size_t appendByteSize) {

	size_t currentUsedSize = attr.count() * dataUnitSize;
	size_t currentAllocSize = attrBuffer.data().size();
	if ((currentUsedSize + appendByteSize) > currentAllocSize) {
		size_t countPowerOf2 = 1;
		while (countPowerOf2 < currentUsedSize + appendByteSize) {
			countPowerOf2 <<= 1;
		}
		auto copy = attrBuffer.data();
		copy.resize(countPowerOf2);
		attrBuffer.setData(copy);
	}
}

void GLModel::setMesh(Mesh* mesh)
{

	//flush datas
	auto faceHistory = mesh->getFacesNonConst().flushChanges();
	auto verticesHistory = mesh->getVerticesNonConst().flushChanges();
	auto hEdgesHistory = mesh->getHalfEdgesNonConst().flushChanges();//not used...for now
	removeComponent(&m_geometryRenderer);
	appendMesh(mesh);
	addComponent(&m_geometryRenderer);

}

void GLModel::appendMesh(Mesh* mesh)
{
	_currentVisibleMesh = mesh;
	//if the QGeo hasn't been cleared, ie) combining two meshes, set current mesh pointer to nullptr
	if (positionAttribute.count() != 0)
		_currentVisibleMesh = nullptr;

	//indexAttribute.buffer()->setAccessType(Qt3DRender::QBuffer::AccessType::);
	//indexAttribute.buffer()->setAccessType(Qt3DRender::QBuffer::AccessType::Read);
	//m_geometry.addAttribute(&indexAttribute);
	auto &faces = mesh->getFaces();
	auto &vtxs = mesh->getVertices();
	//since renderer access data from background thread
	size_t prevMaxIndex =  appendMeshVertex(mesh, vtxs.cbegin(), vtxs.cend());
	appendMeshFace(mesh, faces.cbegin(), faces.cend(), prevMaxIndex);

}

size_t GLModel::appendMeshVertex(const Mesh* mesh,
	Hix::Engine3D::VertexConstItr begin, Hix::Engine3D::VertexConstItr end)
{
	//we use position attribute, but can be either normal, color etc since they have same count
	size_t oldCount = positionAttribute.count();
	size_t oldSize = oldCount * VTX_SIZE;
	//append count
	size_t count = end - begin;
	size_t appendByteSize = count * VTX_SIZE;
	//resize attr buffer for new append
	//attrBufferResize(positionAttribute, vertexBuffer, VTX_SIZE, appendByteSize);
	//data to be appended
	QByteArray appendData;
	appendData.resize(appendByteSize);
	//add data to the append data
	QVector<QVector3D> vertices;

	for (auto itr = begin; itr != end; ++itr)
	{
		auto& vtx = *itr;
		vertices << vtx.position << vtx.vn << vtx.color;

	}
	float* rawVertexArray = reinterpret_cast<float*>(appendData.data());
	size_t idx = 0;
	for (const QVector3D& v : vertices) {
		rawVertexArray[idx++] = v.x();
		rawVertexArray[idx++] = v.y();
		rawVertexArray[idx++] = v.z();
	}
	//update data/count
	QByteArray totalData = vertexBuffer.data() + appendData;

	vertexBuffer.setData(totalData);
	positionAttribute.setCount(oldCount + count);
	normalAttribute.setCount(oldCount + count);
	colorAttribute.setCount(oldCount + count);
	return oldCount;
}

void GLModel::appendMeshFace(const Mesh* mesh, Hix::Engine3D::FaceConstItr begin, Hix::Engine3D::FaceConstItr end, size_t prevMaxIndex)
{	
	//we need to get maximum index of the current rendered mesh when we are appending to non-empty mesh.
	size_t oldCount = indexAttribute.count()/3; //see below
	size_t oldSize = oldCount * FACE_SIZE;

	size_t count = end - begin;
	size_t appendByteSize = count * FACE_SIZE;

	//resize attr buffer for new append
	//attrBufferResize(indexAttribute, indexBuffer, FACE_SIZE, appendByteSize);
	QByteArray indexBufferData;
	indexBufferData.resize(appendByteSize);
	uint* rawIndexArray = reinterpret_cast<uint*>(indexBufferData.data());

	size_t indexIndex = 0;

	for (auto itr = begin; itr != end; ++itr)
	{
		auto& face = *itr;
		auto faceVertices = face.getVerticeIndices(mesh);
		for (auto faceVtx : faceVertices)
		{
			rawIndexArray[indexIndex] = faceVtx + prevMaxIndex;
			++indexIndex;
		}
	}
	QByteArray totalData = indexBuffer.data() + indexBufferData;
	indexBuffer.setData(totalData);
	indexAttribute.setCount((oldCount + count) *3);//3 indicies per face
	m_geometryRenderer.setVertexCount(indexAttribute.count());


}



void GLModel::updateMesh(Mesh* mesh)
{
	//flush datas
	auto faceHistory = mesh->getFacesNonConst().flushChanges();
	auto verticesHistory = mesh->getVerticesNonConst().flushChanges();
	auto hEdgesHistory = mesh->getHalfEdgesNonConst().flushChanges();//not used...for now

	bool tooManyChanges = false;
	std::unordered_set<size_t> faceChangeSet;
	std::unordered_set<size_t> vtxChangeSet;
	//check allChanged flag and skip to updateAll OR...
	//if the mesh being updated is not the same as the visible one, we need to redraw everything
	if (_currentVisibleMesh != mesh || faceHistory.index() == 0 || verticesHistory.index() == 0)
	{
		tooManyChanges = true;
	}
	else
	{
		faceChangeSet = std::get<1>(faceHistory);
		vtxChangeSet = std::get<1>(verticesHistory);
		if (faceChangeSet.size() > mesh->getFaces().size() * 0.7 || vtxChangeSet.size() > mesh->getVertices().size() * 0.7)
		{
			tooManyChanges = true;
		}
	}
	//if there are too many individual changes...
	tooManyChanges = true;
	if (tooManyChanges)
	{
		clearMem();
		setMesh(mesh);
		return;
	}


	removeComponent(&m_geometryRenderer);
	updateVertices(vtxChangeSet, *mesh);
	updateFaces(faceChangeSet, *mesh);
	addComponent(&m_geometryRenderer);

}




void GLModel::updateFaces(const std::unordered_set<size_t>& faceIndicies, const Hix::Engine3D::Mesh& mesh)
{
	auto& faces = mesh.getFaces();
	size_t oldFaceCount = indexAttribute.count()/3;
	size_t newFaceCount = faces.size();
	size_t smallerCount = std::min(oldFaceCount, newFaceCount);
	size_t largerCount = std::max(oldFaceCount, newFaceCount);
	size_t difference = largerCount - smallerCount;
	//update existing old values if they were changed
	QByteArray updateArray;
	updateArray.resize(FACE_SIZE);
	uint* rawIndexArray = reinterpret_cast<uint*>(updateArray.data());
	for (auto faceIdx : faceIndicies)
	{
		if (faceIdx < smallerCount)
		{
			auto face = faces[faceIdx];
			size_t offset = faceIdx * FACE_SIZE;
			auto faceVertices = face.getVerticeIndices(&mesh);
			//for each indices
			for (size_t i = 0; i < 3; ++i)
			{
				rawIndexArray[i] = faceVertices[i];
			}
			indexBuffer.updateData(offset, updateArray);
		}
	}
	indexBuffer.setData(indexBuffer.data());

	//if new size is smaller, delete elements from back
	if (newFaceCount < oldFaceCount)
	{
		eraseBufferData(indexAttribute, indexBuffer, difference * FACE_SIZE, difference*3);
		m_geometryRenderer.setVertexCount(indexAttribute.count());
	}
	else if (newFaceCount > oldFaceCount)
	{
		appendMeshFace(&mesh, faces.cend() - difference, faces.cend(), 0);
	}

	indexAttribute.setCount(newFaceCount * 3);//3 indicies per face
	m_geometryRenderer.setVertexCount(indexAttribute.count());

}


void GLModel::updateVertices(const std::unordered_set<size_t>& vtxIndicies, const Hix::Engine3D::Mesh& mesh)
{
	auto& vtcs = mesh.getVertices();
	size_t oldVtxCount = positionAttribute.count();
	size_t newVtxCount = vtcs.size();
	size_t smallerCount = std::min(oldVtxCount, newVtxCount);
	size_t largerCount = std::max(oldVtxCount, newVtxCount);
	size_t difference = largerCount - smallerCount;
	//update existing old values if they were changed
	QByteArray updateArray;
	updateArray.resize(VTX_SIZE);
	float* rawVertexArray = reinterpret_cast<float*>(updateArray.data());

	//vertexBuffer.blockSignals(true);
	//vertexBuffer.blockNotifications(true);
	for (auto vtxIdx : vtxIndicies)
	{
		if (vtxIdx < smallerCount)
		{
			auto vtx = vtcs[vtxIdx];
			QVector<QVector3D> vtxData;
			vtxData << vtx.position  << vtx.vn << vtx.color;
			size_t idx = 0;
			for (const QVector3D& v : vtxData) {
				rawVertexArray[idx++] = v.x();
				rawVertexArray[idx++] = v.y();
				rawVertexArray[idx++] = v.z();
			}
			size_t offset = vtxIdx * VTX_SIZE;
			vertexBuffer.updateData(offset, updateArray);
		}
	}
	//vertexBuffer.blockSignals(false);
	//vertexBuffer.blockNotifications(false);
	//vertexBuffer.setData(vertexBuffer.data());
	emit vertexBuffer.dataChanged(vertexBuffer.data());
	//if new size is smaller, delete elements from back
	if (newVtxCount < oldVtxCount)
	{
		eraseBufferData(positionAttribute, vertexBuffer, difference * VTX_SIZE, difference);
		//set other attribute count as well
		normalAttribute.setCount(positionAttribute.count());
		colorAttribute.setCount(positionAttribute.count());
	}
	else if (newVtxCount > oldVtxCount)
	{
		appendMeshVertex(&mesh, vtcs.cend() - difference, vtcs.cend());
	}
	positionAttribute.setCount(newVtxCount);
	normalAttribute.setCount(positionAttribute.count());
	colorAttribute.setCount(positionAttribute.count());
}


const Mesh* GLModel::getMesh()
{
    return _mesh;
}


void swapByteOrder(unsigned int& ui)
{
	ui = (ui >> 24) |
		((ui << 8) & 0x00FF0000) |
		((ui >> 8) & 0x0000FF00) |
		(ui << 24);
}


void GLModel::mouseEnteredFreeCutSphere()
{
    QCursor cursorEraser = QCursor(QPixmap(":/Resource/cursor_eraser.png"));
    QApplication::setOverrideCursor(cursorEraser);

}

void GLModel::mouseExitedFreeCutSphere()
{
    qmlManager->resetCursor();
}
void GLModel::mouseClickedFreeCutSphere(Qt3DRender::QPickEvent* pick)
{
    qmlManager->resetCursor();

    if(cuttingPoints.size() < 1)
        return;

    QVector2D pickPosition = (QVector2D)pick->position();

    int minIdx = 0;
    float min = qmlManager->world2Screen(cuttingPoints[0]).distanceToPoint(pickPosition);
    for (int i=0; i<cuttingPoints.size(); i++){
        if (qmlManager->world2Screen(cuttingPoints[i]).distanceToPoint(pickPosition) < min){
            minIdx = i;
            min = qmlManager->world2Screen(cuttingPoints[i]).distanceToPoint(pickPosition);
        }
    }

    qDebug() << "min idx  " << minIdx;
    removeCuttingPoint(minIdx);
    removeCuttingContour();
    if (cuttingPoints.size() >=2){
        generateCuttingContour(cuttingPoints);
        regenerateCuttingPoint(cuttingPoints);
    }
}

void GLModel::mouseClickedFreeCut(Qt3DRender::QPickEvent* pick)
{
    /*if ((pick->position().x() < 260 && pick->position().y() < 330)|| cutMode == 1 || pick->button() != Qt3DRender::QPickEvent::Buttons::LeftButton) // cut panel and if cut mode isn't freecut
        return;

    QVector3D v = pick->localIntersection();
    QVector3D result_v = QVector3D(v.x(), -v.z(), v.y());
    result_v = result_v*2;

    qDebug()<< "moiuse clicked freecut : " << v << result_v;

    bool found_nearby_v = false;


    for (int i=0; i< cuttingPoints.size(); i++){
        if (result_v.distanceToPoint(cuttingPoints[i]) <0.5f){;
            removeCuttingPoint(i);
            found_nearby_v = true;
            break;
        }
    }

    if (!found_nearby_v){
        addCuttingPoint(result_v);
    }

    // remove cutting contour and redraw cutting contour
    removeCuttingContour();
    if (cuttingPoints.size() >=2){
        qDebug() << "tt";
        generateCuttingContour(cuttingPoints);
        regenerateCuttingPoint(cuttingPoints);
    }


    if (cuttingPoints.size() >= 3)
        QMetaObject::invokeMethod(qmlManager->cutPopup, "colorApplyFinishButton", Q_ARG(QVariant, 2));
    else
        QMetaObject::invokeMethod(qmlManager->cutPopup, "colorApplyFinishButton", Q_ARG(QVariant, 0));
    */
}


void GLModel::mouseClickedLayflat(MeshFace shadow_meshface){
    /*
    qDebug() << "layflat picker!";
    QPickTriangleEvent *trianglePick = static_cast<QPickTriangleEvent*>(pick);
    MeshFace shadow_meshface = _mesh->faces[trianglePick->triangleIndex()];
    */
    uncolorExtensionFaces();
    QVector3D tmp_fn = shadow_meshface.fn;
    Qt3DCore::QTransform tmp;
    float x= tmp_fn.x();
    float y= tmp_fn.y();
    float z=tmp_fn.z();
    float angleX = qAtan2(y,z)*180/M_PI;
    if (z>0) angleX+=180;
    float angleY = qAtan2(x,z)*180/M_PI;
    tmp.setRotationX(angleX);
    tmp.setRotationY(angleY);
    rotateModelMesh(tmp.matrix());
    //QObject::disconnect(m_objectPicker,SIGNAL(released(Qt3DRender::QPickEvent*)), this, SLOT(mouseClickedLayflat(Qt3DRender::QPickEvent*)));
    //delete m_objectPicker;
    ////m_objectPicker->setEnabled(true);
    //closeLayflat();
    emit resetLayflat();
}

		
void GLModel::generateClickablePlane(){
    generatePlane();
    for (int i=0;i<2;i++){
       qDebug() << "generated clickable plane";
    }
    return;
}

void GLModel::removeCuttingContour(){
    qDebug() << "in the remove cutting contour " << cuttingContourCylinders.size();

    for (int i=0; i<cuttingContourCylinders.size(); i++){
        cuttingContourCylinders[i]->deleteLater();
    }
    cuttingContourCylinders.clear();
    if (cuttingPoints.size() < 3)
        QMetaObject::invokeMethod(qmlManager->cutPopup, "colorApplyFinishButton", Q_ARG(QVariant, 0));
}
void GLModel::regenerateCuttingPoint(std::vector<QVector3D> points){
    return;
    int temp = points.size();
    /* This function is working with strange logic(make 3 times more point and delete ),  fix it later */
  \

    for (int i=0; i<temp; i++){
        QVector3D temp = points[i] + QVector3D(0,0,110);

        addCuttingPoint(temp);
        //addCuttingPoint((QVector3D)sphereTransform[i]->translation());

        qDebug() << "tttt    " << points[i] << (QVector3D)sphereTransform[i]->translation();
    }

    for (int i=0; i<temp; i++){
        qDebug() << "remove";
        removeCuttingPoint(0);
    }

}

void GLModel::generateCuttingContour(std::vector<QVector3D> cuttingContour){
    for (int cvi=0; cvi<cuttingContour.size(); cvi++){
        qDebug() << "pos   " << cvi;
    }

    for (int cvi=0; cvi<cuttingContour.size()-1; cvi++){
        QVector3D to = cuttingContour[cvi];
        to = QVector3D(to.x(), to.y(), 100.0f);
        QVector3D from = cuttingContour[(cvi+1)%cuttingContour.size()];
        from = QVector3D(from.x(), from.y(), 100.0f);
        double w, h, d, l, tx, ty, tz, lxz, anglex, angley;
        w = to.x() - from.x();
        h = to.y() - from.y();
        d = to.z() - from.z();
        l = sqrt(w*w + h*h + d*d);
        lxz = sqrt(w*w + d*d);
        tx = from.x() + w/2;
        ty = from.y() + h/2;
        tz = from.z() + d/2;
        anglex = (l==0)? 90:acos(h/l)*180/3.14159265;
        angley = (lxz==0)? 90:acos(d/lxz)*180/3.14159265;

        QVector3D QVx(static_cast<float>(1), static_cast<float>(0), static_cast<float>(0));
        QVector3D QVy(static_cast<float>(0), static_cast<float>(((w<0)?-1:1)), static_cast<float>(0));

        // Cylinder shape data
        Qt3DExtras::QCylinderMesh *cylinder = new Qt3DExtras::QCylinderMesh();
        cylinder->setRadius(.1f);

        cylinder->setLength(static_cast<float>(l));
        cylinder->setRings(100);
        cylinder->setSlices(20);

        // CylinderMesh Transform
        Qt3DCore::QTransform *cylinderTransform = new Qt3DCore::QTransform();
        cylinderTransform->setScale(1.0f);
        cylinderTransform->setRotation(Qt3DCore::QTransform::fromAxesAndAngles(QVx,static_cast<float>(anglex), QVy, static_cast<float>(angley)));
        //cylinderTransform->setTranslation(QVector3D(static_cast<float>(tx), static_cast<float>(ty), static_cast<float>(tz)));
        cylinderTransform->setTranslation(QVector3D(static_cast<float>(tx), static_cast<float>(ty), 100));

        Qt3DExtras::QPhongMaterial *cylinderMaterial = new Qt3DExtras::QPhongMaterial();

        cylinderMaterial->setAmbient(QColor(QRgb(0x0086AA)));
        cylinderMaterial->setDiffuse(QColor(QRgb(0x0086AA)));
        cylinderMaterial->setSpecular(QColor(QRgb(0x0086AA)));
        /*
        cylinderMaterial->setAmbient(QColor(QRgb(0x004F6B)));
        cylinderMaterial->setDiffuse(QColor(QRgb(0x004F6B)));
        cylinderMaterial->setSpecular(QColor(QRgb(0x004F6B)));
        */
        cylinderMaterial->setShininess(0.0f);
        // Cylinder
        QEntity* m_cylinderEntity = new Qt3DCore::QEntity(this);
        m_cylinderEntity->addComponent(cylinder);
        m_cylinderEntity->addComponent(cylinderMaterial);
        m_cylinderEntity->addComponent(cylinderTransform);
        cuttingContourCylinders.push_back(m_cylinderEntity);
    }

    if (cuttingPoints.size() >= 3)
        QMetaObject::invokeMethod(qmlManager->cutPopup, "colorApplyFinishButton", Q_ARG(QVariant, 2));
}

void GLModel::generatePlane(){
    qDebug() << "generate plane called";
    removePlane();
    if (cuttingPoints.size()<3){
        qDebug()<<"Error: There is not enough vectors to render a plane.";
        return;
    }

    QVector3D v1;
    QVector3D v2;
    QVector3D v3;
    v1=cuttingPoints[cuttingPoints.size()-3];
    v2=cuttingPoints[cuttingPoints.size()-2];
    v3=cuttingPoints[cuttingPoints.size()-1];
    planeMaterial = new Qt3DExtras::QPhongAlphaMaterial();
    planeMaterial->setAmbient(QColor(QRgb(0xF4F4F4)));
    planeMaterial->setDiffuse(QColor(QRgb(0xF4F4F4)));
    planeMaterial->setSpecular(QColor(QRgb(0xF4F4F4)));
    planeMaterial->setAlpha(0.5f);

    // set cutting plane
    cuttingPlane.clear();
    cuttingPlane.push_back(v1);
    cuttingPlane.push_back(v2);
    cuttingPlane.push_back(v3);

    QVector3D world_origin(0,0,0);
    QVector3D original_normal(0,1,0);
    QVector3D desire_normal(QVector3D::normal(v1,v2,v3)); //size=1
    float angle = qAcos(QVector3D::dotProduct(original_normal,desire_normal))*180/M_PI;
    QVector3D crossproduct_vector(QVector3D::crossProduct(original_normal,desire_normal));

    for (int i=0;i<2;i++){
        planeEntity[i] = new Qt3DCore::QEntity(this);
        //qDebug() << "generatePlane---------------------==========-=-==-" << parentModel;
        clipPlane[i]=new Qt3DExtras::QPlaneMesh(this);
        clipPlane[i]->setHeight(200.0);
        clipPlane[i]->setWidth(200.0);

        planeTransform[i]=new Qt3DCore::QTransform();
        planeTransform[i]->setRotation(QQuaternion::fromAxisAndAngle(crossproduct_vector, angle+180*i));
        planeTransform[i]->setTranslation(desire_normal*(-world_origin.distanceToPlane(v1,v2,v3))
                                          + m_transform.translation());

        planeObjectPicker[i] = new Qt3DRender::QObjectPicker;//planeEntity[i]);

        planeObjectPicker[i]->setHoverEnabled(true);
        planeObjectPicker[i]->setEnabled(true);
        //QObject::connect(planeObjectPicker[i], SIGNAL(clicked(Qt3DRender::QPickEvent*)), this, SLOT(mouseClicked(Qt3DRender::QPickEvent*)));
        QObject::connect(planeObjectPicker[i], SIGNAL(released(Qt3DRender::QPickEvent*)), this, SLOT(mouseClickedFreeCut(Qt3DRender::QPickEvent*)));


        planeEntity[i]->addComponent(planeObjectPicker[i]);

        planeEntity[i]->addComponent(clipPlane[i]);
        planeEntity[i]->addComponent(planeTransform[i]); //jj
        planeEntity[i]->addComponent(planeMaterial);
    }

    removeCuttingPoints();
    //modelCut();
}

void GLModel::generateSupport(){

    // copy _mesh data from original _mesh
    layerMesh = new Mesh;
	for (const MeshFace& mf : _mesh->getFaces()) {
		auto meshVertices = mf.meshVertices();
		layerMesh->addFace(meshVertices[0]->position, meshVertices[1]->position, meshVertices[2]->position);
    }

    float x_length = _mesh->x_max() - _mesh->x_min();
    float y_length = _mesh->y_max() - _mesh->y_min();
    float z_length = _mesh->z_max() - _mesh->z_min();
    size_t xy_reserve = x_length * y_length;
    size_t xyz_reserve = xy_reserve * z_length;
    qDebug() << "********************xy_reserve = " << xy_reserve;
    qDebug() << "********************faces_reserve = " << _mesh->getFaces().size();
    qDebug() << "********************vertices_reserve = " << _mesh->getVertices().size();

    layerInfillMesh = new Mesh;
    layerSupportMesh = new Mesh;
    layerRaftMesh = new Mesh;

    QVector3D t = m_transform.translation();

    t.setZ(_mesh->z_min() * -1.0f);
    layerInfillMesh->vertexMove(t);
    layerSupportMesh->vertexMove(t);
    layerRaftMesh->vertexMove(t);

    t.setZ(scfg->raft_thickness);
    layerMesh->vertexMove(t);
    t.setZ(_mesh->z_min() + scfg->raft_thickness + scfg->support_base_height);
    layerInfillMesh->vertexMove(t);
    layerSupportMesh->vertexMove(t);
    layerRaftMesh->vertexMove(t);

    layerInfillMesh->connectFaces();
    layerSupportMesh->connectFaces();
    layerRaftMesh->connectFaces();

	layerInfillMesh->setVerticesColor(COLOR_INFILL);
	layerRaftMesh->setVerticesColor(COLOR_RAFT);

    emit _updateModelMesh();
}

void GLModel::removePlane(){
    //qDebug() << "--remove plane called--" << this << parentModel;
    if (planeMaterial == nullptr){
        qDebug() << "## parentModel have no planeMaterial";
        return;
    }
    delete planeMaterial;
    planeMaterial = nullptr;
    for (int i=0;i<2;i++){
        if (clipPlane[i] != nullptr){
            delete clipPlane[i];
            clipPlane[i] = nullptr;
        }
        if (planeTransform[i] != nullptr){
            delete planeTransform[i];
            planeTransform[i] = nullptr;
        }
        if (planeObjectPicker[i] != nullptr){
            //QObject::disconnect(planeObjectPicker[i], SIGNAL(clicked(Qt3DRender::QPickEvent*)), this, SLOT(mouseClickedFreeCut(Qt3DRender::QPickEvent*)));
            delete planeObjectPicker[i];
            planeObjectPicker[i] = nullptr;
        }
        if (planeEntity[i] != nullptr){
            delete planeEntity[i];
            planeEntity[i] = nullptr;
        }
    }

}

void GLModel::addCuttingPoint(QVector3D v){
    QVector3D tmp = m_transform.translation();
    float zlength = _mesh->z_max() - _mesh->z_min();
    cuttingPoints.push_back(v);

    sphereMesh.push_back(new Qt3DExtras::QSphereMesh);
    sphereMesh[sphereMesh.size()-1]->setRadius(0.4);

    sphereTransform.push_back(new Qt3DCore::QTransform);
    //sphereTransform[sphereTransform.size()-1]->setTranslation(v + QVector3D(tmp.x(),tmp.y(), -_mesh->z_min()));
    //v = QVector3D(v.x(),v.y(),0);
    sphereTransform[sphereTransform.size()-1]->setTranslation(v + QVector3D(tmp.x(),tmp.y(), 200));

    sphereMaterial.push_back(new Qt3DExtras::QPhongMaterial());
    sphereMaterial[sphereMaterial.size()-1]->setAmbient(QColor(QRgb(0x000000)));
    sphereMaterial[sphereMaterial.size()-1]->setDiffuse(QColor(QRgb(0x000000)));
    sphereMaterial[sphereMaterial.size()-1]->setSpecular(QColor(QRgb(0x000000)));
    sphereMaterial[sphereMaterial.size()-1]->setShininess(0.0f);

    sphereObjectPicker.push_back(new Qt3DRender::QObjectPicker);
    sphereObjectPicker[sphereObjectPicker.size()-1]->setEnabled(true);
    sphereObjectPicker[sphereObjectPicker.size()-1]->setHoverEnabled(true);

    sphereEntity.push_back(new Qt3DCore::QEntity(this));
    sphereEntity[sphereEntity.size()-1]->addComponent(sphereMesh[sphereMesh.size()-1]);
    sphereEntity[sphereEntity.size()-1]->addComponent(sphereTransform[sphereTransform.size()-1]);
    sphereEntity[sphereEntity.size()-1]->addComponent(sphereMaterial[sphereMaterial.size()-1]);

    QObject::connect(sphereObjectPicker[sphereObjectPicker.size()-1], SIGNAL(entered()), this, SLOT(mouseEnteredFreeCutSphere()));
    QObject::connect(sphereObjectPicker[sphereObjectPicker.size()-1], SIGNAL(exited()), this, SLOT(mouseExitedFreeCutSphere()));
    QObject::connect(sphereObjectPicker[sphereObjectPicker.size()-1], SIGNAL(clicked(Qt3DRender::QPickEvent*)), this, SLOT(mouseClickedFreeCutSphere(Qt3DRender::QPickEvent*)));
    sphereEntity[sphereEntity.size()-1]->addComponent(sphereObjectPicker[sphereObjectPicker.size()-1]);

}

void GLModel::removeCuttingPoint(int idx){
    qDebug()<< "cutting points : " << cuttingPoints.size();

    sphereEntity[idx]->removeComponent(sphereMesh[idx]);
    sphereEntity[idx]->removeComponent(sphereTransform[idx]);
    sphereEntity[idx]->removeComponent(sphereMaterial[idx]);
    sphereEntity[idx]->removeComponent(sphereObjectPicker[idx]);
    sphereEntity[idx]->deleteLater();
    sphereMesh[idx]->deleteLater();
    sphereTransform[idx]->deleteLater();
    sphereMaterial[idx]->deleteLater();
    sphereObjectPicker[idx]->deleteLater();
    sphereEntity.erase(sphereEntity.begin()+idx);
    sphereMesh.erase(sphereMesh.begin()+idx);
    sphereTransform.erase(sphereTransform.begin()+idx);
    sphereMaterial.erase(sphereMaterial.begin()+idx);
    sphereObjectPicker.erase(sphereObjectPicker.begin()+idx);
    cuttingPoints.erase(cuttingPoints.begin()+idx);
}

void GLModel::removeCuttingPoints(){
    for (int i=0; i<sphereEntity.size(); i++){
        sphereEntity[i]->removeComponent(sphereMesh[i]);
        sphereEntity[i]->removeComponent(sphereTransform[i]);
        sphereEntity[i]->removeComponent(sphereMaterial[i]);
        sphereEntity[i]->removeComponent(sphereObjectPicker[i]);
        sphereEntity[i]->deleteLater();
        sphereMesh[i]->deleteLater();
        sphereTransform[i]->deleteLater();
        sphereMaterial[i]->deleteLater();
        sphereObjectPicker[i]->deleteLater();
    }
    sphereEntity.clear();
    sphereMesh.clear();
    sphereTransform.clear();
    sphereMaterial.clear();
    sphereObjectPicker.clear();
    cuttingPoints.clear();
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
    qDebug() << "modelcut called" << cutMode;
    if(cutMode == 0)
        return ;
    if(cutMode == 2 && cuttingPoints.size() < 3)
        return ;

    saveUndoState();
    qmlManager->openProgressPopUp();

	auto lmesh = new Mesh();
	auto rmesh = new Mesh();
    if (cutMode == 1){ // flat cut
        if (cuttingPlane.size() != 3){
            return;
        }
        if (this->shellOffsetActive && isFlatcutEdge == true) {
            getSliderSignal(0.0);
        }

        /*
        // create left, rightmesh
        Mesh* leftMesh = lmesh;
        Mesh* rightMesh = rmesh;
        // do bisecting mesh
        leftMesh->faces.reserve(mesh->faces.size()*3);
        leftMesh->vertices.reserve(mesh->faces.size()*9);
        rightMesh->faces.reserve(mesh->faces.size()*3);
        rightMesh->vertices.reserve(mesh->faces.size()*9);*/

        bisectModelByPlane(lmesh, rmesh, _mesh, cuttingPlane, cutFillMode);

        emit bisectDone(lmesh, rmesh);

    } else if (cutMode == 2){ // free cut
        if (cuttingPoints.size() >= 3){
            // do bisecting _mesh

            /*
            // create left, rightmesh
            Mesh* leftMesh = lmesh;
            Mesh* rightMesh = rmesh;
            // do bisecting mesh
            leftMesh->faces.reserve(_mesh->getFaces().size()*10);
            leftMesh->vertices.reserve(_mesh->getFaces().size()*30);
            rightMesh->faces.reserve(_mesh->getFaces().size()*10);
            rightMesh->vertices.reserve(_mesh->getFaces().size()*30);*/

            cutAway(lmesh, rmesh, _mesh, cuttingPoints, cutFillMode);

            if (lmesh->getFaces().size() == 0 || rmesh->getFaces().size() == 0){
            // diridiri ######
            //modelcut::cutAway(lmesh, rmesh,_mesh,cuttingPoints,cutFillMode);
            //if (lmesh->getFaces()->size() == 0 || rmesh->getFaces()->size() == 0){

                qDebug() << "cutting contour selected not cutting";
                qmlManager->setProgress(1);
                //cutModeSelected(9999);
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

    removeCuttingPoints();
    removeCuttingContour();

    // delete original model
    qmlManager->deleteModelFile(ID);

    // do auto arrange
    qmlManager->runArrange();
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




//when ray casting is not needed, ie) right click

void GLModel::clicked(MouseEventData& pick, const Qt3DRender::QRayCasterHit& hit)
{

	if (!cutActive && !extensionActive && !labellingActive && !layflatActive && !manualSupportActive)// && !layerViewActive && !supportViewActive)
		qmlManager->modelSelected(ID);

	if (qmlManager->isSelected(this) && pick.button == Qt::MouseButton::RightButton) {
		qDebug() << "mttab alive";
		QMetaObject::invokeMethod(qmlManager->mttab, "tabOnOff");
	}

#ifdef _STRICT_GLMODEL
	if (hit.type() != QRayCasterHit::HitType::TriangleHit)
		throw std::runtime_error("trying to get tri idx from non tri hit");
#endif



	if (cutActive) {
		if (cutMode == 1) {
			qDebug() << "cut mode 1";

		}
		else if (cutMode == 2) {// && numPoints< sizeof(sphereEntity)/4) {
			qDebug() << hit.localIntersection() << "pick" << cuttingPoints.size() << cuttingPoints.size();
			QVector3D v = hit.localIntersection();
			qDebug() << v.distanceToPoint(cuttingPoints[cuttingPoints.size() - 1]);

            if(this != hit.entity())
            {
                //plane clicked
                v = {v.x(), -v.z(), v.y()};
            }

			bool found_nearby_v = false;
			for (int i = 0; i < cuttingPoints.size(); i++) {
				if (v.distanceToPoint(cuttingPoints[i]) < 0.5f) {
					removeCuttingPoint(i);
					found_nearby_v = true;
					break;
				}
			}

			if (!found_nearby_v) {
				qDebug() << "Check 0 " << cuttingPoints.size();
				addCuttingPoint(v + m_transform.translation());
				qDebug() << "Check 1 " << cuttingPoints.size();
			}

			removeCuttingContour();
			if (cuttingPoints.size() >= 2) {
				generateCuttingContour(cuttingPoints);
				regenerateCuttingPoint(cuttingPoints);
			}
			//generatePlane();
			//ft->ct->addCuttingPoint(parentModel, v);
		}
		else if (cutMode == 9999) {
			qDebug() << "current cut mode :" << cutMode;
			qmlManager->setProgress(1);
			//return;
		}
	}

    //triangle index section
    if (hit.primitiveIndex() >= _mesh->getFaces().size())
    {
        qDebug() << "trianglePick out of bound";
        return;
    }
    _targetSelected = true;
    targetMeshFace = _mesh->getFaces().cbegin() + hit.primitiveIndex();


	if (extensionActive && hit.localIntersection() != QVector3D(0, 0, 0)) {
		uncolorExtensionFaces();
		emit extensionSelect();
		generateColorAttributes();

		colorExtensionFaces();
	}

	if (hollowShellActive) {
		qDebug() << "getting handle picker clicked signal hollow shell active";
		qDebug() << "found parent meshface";
		// translate hollowShellSphere to mouse position
		QVector3D v = hit.localIntersection();
        qmlManager->hollowShellSphereTransform->setTranslation(v + m_transform.translation());


	}

	if (layflatActive && hit.localIntersection() != QVector3D(0, 0, 0)) {

		uncolorExtensionFaces();
		emit layFlatSelect();
		generateColorAttributes();
		colorExtensionFaces();
	}

	if (manualSupportActive && hit.localIntersection() != QVector3D(0, 0, 0)) {
		qDebug() << "manual support handle picker clicked";
		uncolorExtensionFaces();
		emit extensionSelect();
		generateColorAttributes();
		colorExtensionFaces();
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
			layerViewActive ||
			manualSupportActive ||
			supportViewActive)	
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
	lastpoint = hit.localIntersection();
	prevPoint = (QVector2D)e.position;
	qmlManager->moveButton->setProperty("state", "active");
	qmlManager->setClosedHandCursor();
	saveUndoState();
	qmlManager->hideMoveArrow();
	qDebug() << "hiding move arrow";
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
	qmlManager->modelMoveDone();

}

void GLModel::cutModeSelected(int type){

    qDebug() << "cut mode selected1" << type;
    removeCuttingPoints();
    //removePlane();
    removeCuttingContour();
    //removePlane();
    cutMode = type;
    if (cutMode == 1){
        addCuttingPoint(QVector3D(1,0,0));
        addCuttingPoint(QVector3D(1,1,0));
        addCuttingPoint(QVector3D(2,0,0));
        generatePlane();
    } else if (cutMode == 2){
        qDebug() << QCursor::pos();
        addCuttingPoint(QVector3D(1,0,0));
        addCuttingPoint(QVector3D(1,1,0));
        addCuttingPoint(QVector3D(2,0,0));
        generateClickablePlane(); // plane is also clickable
    } else {

    }
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
        QVector3D v1(1,0, _mesh->z_min() + value*zlength/1.8);
        QVector3D v2(1,1, _mesh->z_min() + value*zlength/1.8);
        QVector3D v3(2,0, _mesh->z_min() + value*zlength/1.8);

        QVector3D world_origin(0,0,0);
        QVector3D original_normal(0,1,0);
        QVector3D desire_normal(QVector3D::normal(v1,v2,v3)); //size=1
        float angle = qAcos(QVector3D::dotProduct(original_normal,desire_normal))*180/M_PI+(value-1)*30;
        QVector3D crossproduct_vector(QVector3D::crossProduct(original_normal,desire_normal));

        for (int i=0;i<2;i++){
            planeTransform[i]->setTranslation(desire_normal*(-world_origin.distanceToPlane(v1,v2,v3))
                                              +  m_transform.translation());
            planeEntity[i]->addComponent(planeTransform[i]);
        }

        cuttingPlane[0] = v1;
        cuttingPlane[1] = v2;
        cuttingPlane[2] = v3;
    } else if (hollowShellActive){
        // change radius of hollowShellSphere
        hollowShellRadius = value;
        qmlManager->hollowShellSphereMesh->setRadius(hollowShellRadius);

        qDebug() << "getting slider signal: current radius is " << value;
    }
}

void GLModel::getLayerViewSliderSignal(double value) {
    if ( !layerViewActive)
        return;

    float height = (_mesh->z_max() - _mesh->z_min() + scfg->raft_thickness + scfg->support_base_height) * value;
    int layer_num = int(height/scfg->layer_height)+1;
    if (value <= 0.002f)
        layer_num = 0;

    if (layerViewPlaneTextureLoader == nullptr)
    layerViewPlaneTextureLoader = new Qt3DRender::QTextureLoader();

    QDir dir(QDir::tempPath()+"_export");//(qmlManager->selectedModels[0]->filename + "_export")
    if (dir.exists()){
        QString filename = dir.path()+"/"+QString::number(layer_num)+".svg";
        qDebug() << filename;
        layerViewPlaneTextureLoader->setSource(QUrl::fromLocalFile(filename));//"C:\\Users\\User\\Desktop\\sliced\\11111_export\\100.svg"));
    }
    //qDebug() << "layer view plane material texture format : " << layerViewPlaneTextureLoader->format();
    //layerViewPlaneTextureLoader->setFormat(QAbstractTexture::RGBA32F);
    //qDebug() << "layer view plane material texture format : " << layerViewPlaneTextureLoader->format();

    layerViewPlaneMaterial->setTexture(layerViewPlaneTextureLoader);

    float rotation_values[] = { // rotate by 90 deg
        0, 1, 0,
        -1, 0, 0,
        0, 0, 1
    };

    QMatrix3x3 rotation_matrix(rotation_values);

    layerViewPlaneMaterial->setTextureTransform(rotation_matrix);
    layerViewPlaneTransform[0]->setTranslation(QVector3D(0,0,layer_num*scfg->layer_height - scfg->raft_thickness - scfg->support_base_height));

    // change phong material of original model
    float h = (_mesh->z_max() - _mesh->z_min() + scfg->raft_thickness + scfg->support_base_height) * value + _mesh->z_min() - scfg->raft_thickness - scfg->support_base_height;
    m_layerMaterialHeight->setValue(QVariant::fromValue(h));

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

    saveUndoState();

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

   _mesh->connectFaces();

    updateModelMesh();

    qmlManager->setProgress(1);
}

// for extension

void GLModel::colorExtensionFaces(){
    removeComponent(m_meshMaterial);
    addComponent(m_meshVertexMaterial);
}

void GLModel:: uncolorExtensionFaces(){
    resetColorMesh(_mesh, &vertexBuffer, extendFaces);
    removeComponent(m_meshVertexMaterial);
    addComponent(m_meshMaterial);
	updateModelMesh();

}
void GLModel::generateColorAttributes(){
    extendColorMesh(_mesh,targetMeshFace,&vertexBuffer,extendFaces);
	updateModelMesh();
}
void GLModel::generateExtensionFaces(double distance){
    if (!_targetSelected)
        return;

    saveUndoState();
    extendMesh(_mesh, targetMeshFace, distance);
	_targetSelected = false;
	emit _updateModelMesh();
}

void GLModel::generateLayFlat(){
    //MeshFace shadow_meshface = *targetMeshFace;

    if(!_targetSelected)
        return;

    saveUndoState();

    QVector3D tmp_fn = targetMeshFace->fn;
    qDebug() << "target 2 " << tmp_fn;

    float x= tmp_fn.x();
    float y= tmp_fn.y();
    float z=tmp_fn.z();

    float angleX = 0;
    float angleY = 0;
    qDebug() << "aqtan2" << qAtan2(z,y)*180/M_PI << "aqtan2" << qAtan2(z,x)*180/M_PI;
    angleX = (qAtan2(z,y)*180/M_PI);
    angleX = 90 + angleX;
    z = (-1)*qSqrt(qPow(z,2) + qPow(y,2));
    angleY = (qAtan2(z,x)*180/M_PI);
    angleY = -90 - angleY;
    //qDebug() << "angle" << angleX << angleY;
    Qt3DCore::QTransform tmp1;
    Qt3DCore::QTransform tmp2;
    tmp1.setRotationX(angleX);
    tmp1.setRotationY(0);
    tmp1.setRotationZ(0);
    //qDebug() << "lay flat 0      ";
    //rotateModelMesh(tmp->matrix());
    tmp2.setRotationX(0);
    tmp2.setRotationY(angleY);
    tmp2.setRotationZ(0);
    rotateModelMesh(tmp1.matrix() * tmp2.matrix());
    //qDebug() << "lay flat 1      ";
    uncolorExtensionFaces();
    //closeLayflat();
    emit resetLayflat();
}


void GLModel::generateManualSupport(){
    qDebug() << "generateManual support called";
    if (!_targetSelected)
        return;
    QVector3D t = m_transform.translation();
    t.setZ(_mesh->z_min()+scfg->raft_thickness + scfg->support_base_height);
    QVector3D targetPosition = targetMeshFace->meshVertices()[0]->position- t;
    /*OverhangPoint* targetOverhangPosition = new OverhangPoint(targetPosition.x()*scfg->resolution,
    generateSupporter(layerSupportMesh, targetOverhangPosition, nullptr, nullptr, layerSupportMesh->z_min());*/
	_targetSelected = false;
	emit _updateModelMesh();
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
    //qDebug() << "open layflat called" << this << this;
    layflatActive = true;

    qmlManager->lastModelSelected();
    if (!qmlManager->isSelected(this))
        layflatActive = false;
    //qDebug() << "open layflat called end  " << layflatActive;

    /*
    QApplication::setOverrideCursor(QCursor(Qt::UpArrowCursor));
    //m_objectPicker->setEnabled(false);
    //m_objectPicker = new Qt3DRender::QObjectPicker(this);
    QObject::connect(//m_objectPicker,SIGNAL(clicked(Qt3DRender::QPickEvent*)), this, SLOT(mouseClickedLayflat(Qt3DRender::QPickEvent*)));
    this->addComponent(//m_objectPicker);
    this->layflatActive = true;
    */
}

void GLModel::closeLayflat(){

    if (!layflatActive)
        return;

    layflatActive = false;
    uncolorExtensionFaces();
	_targetSelected = false;
}
void GLModel::openExtension(){
    extensionActive = true;

    qmlManager->lastModelSelected();
    if (!qmlManager->isSelected(this))
        extensionActive = false;

}

void GLModel::closeExtension(){
    if (!extensionActive)
        return;

    extensionActive = false;
    uncolorExtensionFaces();
	_targetSelected = false;
}

void GLModel::openManualSupport(){
    manualSupportActive = true;
    qDebug() << "open manual support";
}

void GLModel::closeManualSupport(){

    if (!manualSupportActive)
        return;

    manualSupportActive = false;
    uncolorExtensionFaces();
	_targetSelected = false;
	qDebug() << "close manual support";
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

    cutModeSelected(0);
}

void GLModel::closeCut(){
    qDebug() << "closecut called";

    if (!cutActive)
        return;

    cutActive = false;
    removePlane();
    removeCuttingPoints();
    removeCuttingContour();
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
    addCuttingPoint(QVector3D(1,0,0));
    addCuttingPoint(QVector3D(1,1,0));
    addCuttingPoint(QVector3D(2,0,0));

    generatePlane();
    //m_objectPicker->setHoverEnabled(false); // to reduce drag load
    //m_objectPicker->setDragEnabled(false);
}

void GLModel::closeShellOffset(){
    qDebug() << "closeShelloffset";

    if (!shellOffsetActive)
        return;

    shellOffsetActive = false;
    removePlane();
    removeCuttingPoints();
    removeCuttingContour();
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
        supportViewActive = false;
        addComponent(m_meshMaterial);
        removeComponent(m_layerMaterial);
        break;
    case VIEW_MODE_SUPPORT:
        if (layerViewActive){
            // remove layer view components
            removeLayerViewComponents();
        }
        layerViewActive = false;
        supportViewActive = true;
        addComponent(m_meshMaterial);
        removeComponent(m_layerMaterial);

        break;
    case VIEW_MODE_LAYER:
        layerViewActive = true;
        supportViewActive = false;
        addComponent(m_layerMaterial);

        // generate layer view plane materials
        layerViewPlaneMaterial = new Qt3DExtras::QTextureMaterial();
        layerViewPlaneMaterial->setAlphaBlendingEnabled(false);
        layerViewPlaneEntity[0] = new Qt3DCore::QEntity(this);
        layerViewPlane[0]=new Qt3DExtras::QPlaneMesh(this);
        layerViewPlane[0]->setHeight(scfg->bed_x);
        layerViewPlane[0]->setWidth(scfg->bed_y);
        layerViewPlaneTransform[0]=new Qt3DCore::QTransform();
        layerViewPlaneTransform[0]->setRotationZ(-90.0f);
        layerViewPlaneTransform[0]->setRotationY(90.0f);
        layerViewPlaneTransform[0]->setScale(1.0f);
        layerViewPlaneEntity[0]->addComponent(layerViewPlane[0]);
        layerViewPlaneEntity[0]->addComponent(layerViewPlaneTransform[0]); //jj
        layerViewPlaneEntity[0]->addComponent(layerViewPlaneMaterial);
        getLayerViewSliderSignal(1);

        removeComponent(m_meshMaterial);
        break;
    }

    emit _updateModelMesh();
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
    closeManualSupport();
    closeScale();
    //layerViewActive = false; //closeLayerView();
    //supportViewActive = false; //closeSupportView();
    //parentModel->changeViewMode(VIEW_MODE_OBJECT);
}

void GLModel::removeLayerViewComponents(){
    layerViewPlaneEntity[0]->removeComponent(layerViewPlane[0]);
    layerViewPlaneEntity[0]->removeComponent(layerViewPlaneTransform[0]); //jj
    layerViewPlaneEntity[0]->removeComponent(layerViewPlaneMaterial);
    layerViewPlaneEntity[0]->deleteLater();
    layerViewPlane[0]->deleteLater();
    layerViewPlaneTransform[0]->deleteLater();
    layerViewPlaneMaterial->deleteLater();
    layerViewPlaneTextureLoader->deleteLater();


    layerViewPlaneTextureLoader = nullptr;
}

void GLModel::generateLayerViewMaterial() {

    m_layerMaterial = new QMaterial();

    // setup material for layer view
    QEffect *effect = new QEffect();
    QDepthTest *depthTest;
    QCullFace *cullFace;
    QStencilMask *stencilMask;
    QStencilTest *stencilTest;
    QStencilOperation *stencilOp;

    QRenderPass* gl3Pass;
    QShaderProgram *glShader;
    QOpenGLShader *vert;
    QOpenGLShader *frag;

    // Create technique
    QTechnique *gl3Technique = new QTechnique();

    //====================== outer rendering ======================//
    gl3Pass = new QRenderPass();
    glShader = new QShaderProgram();

    // Set the shader on the render pass
    vert = new QOpenGLShader(QOpenGLShader::Vertex);
    if( vert->compileSourceFile(":/shaders/layerview_bottom.vert") ) {
        qDebug() << "vert compiled" << vert->sourceCode();
        glShader->setVertexShaderCode(vert->sourceCode());
    } else {
        qDebug() << "vert compile failed";
    }
    frag = new QOpenGLShader(QOpenGLShader::Fragment);
    if( frag->compileSourceFile(":/shaders/layerview_bottom.frag") ) {
        qDebug() << "frag compiled" << frag->sourceCode();
        glShader->setFragmentShaderCode(frag->sourceCode());
    } else {
        qDebug() << "frag compile failed";
    }
    gl3Pass->setShaderProgram(glShader);

    cullFace = new QCullFace();
    cullFace->setMode(QCullFace::CullingMode::Back);
    gl3Pass->addRenderState(cullFace);

    depthTest = new QDepthTest();
    depthTest->setDepthFunction(QDepthTest::DepthFunction::Less);
    gl3Pass->addRenderState(depthTest);

    // Add the pass to the technique
    gl3Technique->addRenderPass(gl3Pass);

    //====================== top rendering ======================//
    /*
    // Create technique, render pass and shader
    gl3Pass = new QRenderPass();
    glShader = new QShaderProgram();

    // Set the shader on the render pass
    vert = new QOpenGLShader(QOpenGLShader::Vertex);
    if( vert->compileSourceFile(":/shaders/layerview_top.vert") ) {
        qDebug() << "vert compiled" << vert->sourceCode();
        glShader->setVertexShaderCode(vert->sourceCode());
    } else {
        qDebug() << "vert compile failed";
    }
    frag = new QOpenGLShader(QOpenGLShader::Fragment);
    if( frag->compileSourceFile(":/shaders/layerview_top.frag") ) {
        qDebug() << "frag compiled" << frag->sourceCode();
        glShader->setFragmentShaderCode(frag->sourceCode());
    } else {
        qDebug() << "frag compile failed";
    }
    gl3Pass->setShaderProgram(glShader);

    cullFace = new QCullFace();
    cullFace->setMode(QCullFace::CullingMode::Back);
    gl3Pass->addRenderState(cullFace);

    depthTest = new QDepthTest();
    depthTest->setDepthFunction(QDepthTest::DepthFunction::Less);
    gl3Pass->addRenderState(depthTest);

    QBlendEquationArguments* blendState = new QBlendEquationArguments();
    QBlendEquation* blendEquation = new QBlendEquation();
    blendState->setSourceRgba(QBlendEquationArguments::SourceAlpha);
    blendState->setDestinationRgba(QBlendEquationArguments::OneMinusSourceAlpha);
    //blendState->setBufferIndex(2);
    blendEquation->setBlendFunction(QBlendEquation::Add);
    gl3Pass->addRenderState(blendState);
    gl3Pass->addRenderState(blendEquation);

    // Add the pass to the technique
    gl3Technique->addRenderPass(gl3Pass);*/

    //====================== infill rendering ======================//
    // Create technique, render pass and shader
    gl3Pass = new QRenderPass();
    glShader = new QShaderProgram();

    // Set the shader on the render pass
    vert = new QOpenGLShader(QOpenGLShader::Vertex);
    if( vert->compileSourceFile(":/shaders/layerview_infill.vert") ) {
        qDebug() << "vert compiled" << vert->sourceCode();
        glShader->setVertexShaderCode(vert->sourceCode());
    } else {
        qDebug() << "vert compile failed";
    }
    frag = new QOpenGLShader(QOpenGLShader::Fragment);
    if( frag->compileSourceFile(":/shaders/layerview_infill.frag") ) {
        qDebug() << "frag compiled" << frag->sourceCode();
        glShader->setFragmentShaderCode(frag->sourceCode());
    } else {
        qDebug() << "frag compile failed";
    }
    gl3Pass->setShaderProgram(glShader);

    cullFace = new QCullFace();
    cullFace->setMode(QCullFace::CullingMode::Front);
    gl3Pass->addRenderState(cullFace);

    depthTest = new QDepthTest();
    depthTest->setDepthFunction(QDepthTest::DepthFunction::Always);
    gl3Pass->addRenderState(depthTest);

    // Add the pass to the technique
    gl3Technique->addRenderPass(gl3Pass);

    // Set the targeted GL version for the technique
    gl3Technique->graphicsApiFilter()->setApi(QGraphicsApiFilter::OpenGL);
    gl3Technique->graphicsApiFilter()->setMajorVersion(3);
    gl3Technique->graphicsApiFilter()->setMinorVersion(1);
    gl3Technique->graphicsApiFilter()->setProfile(QGraphicsApiFilter::CoreProfile);

    QFilterKey *filterKey = new QFilterKey();
    filterKey->setName(QStringLiteral("renderingStyle"));
    filterKey->setValue(QStringLiteral("forward"));
    gl3Technique->addFilterKey(filterKey);

    // Add the technique to the effect
    effect->addTechnique(gl3Technique);

    m_layerMaterial->setEffect(effect);

    //====================== add parameters ======================//
    m_layerMaterialHeight = new QParameter(QStringLiteral("height"), QVariant::fromValue(0.0f));
    m_layerMaterial->addParameter(m_layerMaterialHeight);

    m_layerMaterialRaftHeight = new QParameter(QStringLiteral("raftHeight"), QVariant::fromValue(scfg->raft_thickness));
    m_layerMaterial->addParameter(m_layerMaterialRaftHeight);

    m_layerMaterial->addParameter(new QParameter(QStringLiteral("ambient"), QColor(130, 130, 140)));
    m_layerMaterial->addParameter(new QParameter(QStringLiteral("diffuse"), QColor(131, 206, 220)));
    m_layerMaterial->addParameter(new QParameter(QStringLiteral("diffuse"), QColor(97, 185, 192)));
    m_layerMaterial->addParameter(new QParameter(QStringLiteral("specular"), QColor(0, 0, 0)));
    //m_layerMaterial->addParameter(new QParameter(QStringLiteral("alpha"), 0.0f));
}



const Mesh* GLModel::getSupport()
{
    return supportMesh;
}



void GLModel::setHitTestable(bool isEnable)
{
	if (_hitEnabled != isEnable)
	{
		_hitEnabled = isEnable;
		if (_hitEnabled)
		{
			addComponent(&_layer);
		}
		else
		{
			removeComponent(&_layer);
		}
	}
}

bool GLModel::isHitTestable()
{
	return _hitEnabled;

}
void GLModel::setSupportAndRaft()
{
	GenerateSupport generatesupport;
	supportMesh = generatesupport.generateSupport(_mesh);
	GenerateRaft generateraft;
	raftMesh = generateraft.generateRaft(_mesh, generatesupport.overhangPoints);
}

const Qt3DCore::QTransform* GLModel::getTransform() const
{
	return &m_transform;
}

void GLModel::setTranslation(const QVector3D& t)
{
	m_transform.setTranslation(t);
}

QVector3D GLModel::getTranslation(){
    return m_transform.translation();
}

const Mesh* GLModel::getRaft()
{
    return raftMesh;
}


void GLModel::setMatrix(const QMatrix4x4& matrix)
{
	m_transform.setMatrix(matrix);
}

Qt3DRender::QLayer* GLModel::getLayer()
{
	return &_layer;
}

