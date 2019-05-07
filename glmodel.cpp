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

#define ATTRIBUTE_SIZE_INCREMENT 200
#if defined(_DEBUG) || defined(QT_DEBUG)
#define _STRICT_GLMODEL
#endif

using namespace Utils::Math;


const QVector3D GLModel::COLOR_DEFAULT_MESH = QVector3D(0.278f, 0.670f, 0.706f);
const QVector3D GLModel::COLOR_INFILL = QVector3D(1.0f, 1.0f, 0.0f);
const QVector3D GLModel::COLOR_RAFT = QVector3D(0.0f, 0.0f, 0.0f);

GLModel::GLModel(QObject* mainWindow, QNode *parent, Mesh* loadMesh, QString fname, bool isShadow, int id)
    : QEntity(parent)
	, _parent(parent)
    , filename(fname)
    , mainWindow(mainWindow)
    , v_cnt(0)
    , f_cnt(0)
	, _currentVisibleMesh(nullptr)
    , dragMesh(new Qt3DExtras::QSphereMesh())
    , parentModel((GLModel*)(parent))
    , cutMode(1)
    , layerMesh(nullptr)
    , layerInfillMesh(nullptr)
    , layerSupportMesh(nullptr)
    , layerRaftMesh(nullptr)
    , slicer(nullptr)
    , ID(id)
    , m_meshMaterial(nullptr)
    , m_geometry(&m_geometryRenderer)
    ,vertexBuffer(Qt3DRender::QBuffer::VertexBuffer, &m_geometry)
    ,vertexNormalBuffer(Qt3DRender::QBuffer::VertexBuffer, &m_geometry)
    ,vertexColorBuffer(Qt3DRender::QBuffer::VertexBuffer, &m_geometry)
	, _boundingBox(this)
{
    connect(&futureWatcher, SIGNAL(finished()), this, SLOT(slicingDone()));
    qDebug() << "new model made _______________________________"<<this<< "parent:"<<this->parentModel<<"shadow:"<<this->shadowModel;

    //initialize vertex buffers etc
    vertexBuffer.setUsage(Qt3DRender::QBuffer::DynamicDraw);
    vertexBuffer.setData(vertexArray);
    vertexNormalBuffer.setUsage(Qt3DRender::QBuffer::DynamicDraw);
    vertexNormalBuffer.setData(vertexNormalArray);
    vertexColorBuffer.setUsage(Qt3DRender::QBuffer::DynamicDraw);
    vertexColorBuffer.setData(vertexColorArray);

    positionAttribute.setAttributeType(QAttribute::VertexAttribute);
    positionAttribute.setBuffer(&vertexBuffer);
    positionAttribute.setDataType(QAttribute::Float);
    positionAttribute.setDataSize(3);
    positionAttribute.setByteOffset(0);
    positionAttribute.setByteStride(3 * sizeof(float));
    positionAttribute.setCount(0);
    positionAttribute.setName(QAttribute::defaultPositionAttributeName());

    normalAttribute.setAttributeType(QAttribute::VertexAttribute);
    normalAttribute.setBuffer(&vertexNormalBuffer);
    normalAttribute.setDataType(QAttribute::Float);
    normalAttribute.setDataSize(3);
    normalAttribute.setByteOffset(0);
    normalAttribute.setByteStride(3 * sizeof(float));
    normalAttribute.setCount(0);
    normalAttribute.setName(QAttribute::defaultNormalAttributeName());

    colorAttribute.setAttributeType(QAttribute::VertexAttribute);
    colorAttribute.setBuffer(&vertexColorBuffer);
    colorAttribute.setDataType(QAttribute::Float);
    colorAttribute.setDataSize(3);
    colorAttribute.setByteOffset(0);
    colorAttribute.setByteStride(3 * sizeof(float));
    colorAttribute.setCount(0);
    colorAttribute.setName(QAttribute::defaultColorAttributeName());

    m_geometry.addAttribute(&positionAttribute);
    m_geometry.addAttribute(&normalAttribute);
    m_geometry.addAttribute(&colorAttribute);
    m_geometryRenderer.setPrimitiveType(QGeometryRenderer::Triangles);
    m_geometryRenderer.setGeometry(&m_geometry);
    addComponent(&m_geometryRenderer);
    m_geometryRenderer.setInstanceCount(1);
    m_geometryRenderer.setFirstVertex(0);
    m_geometryRenderer.setFirstInstance(0);
	addComponent(&m_transform);

    // generates shadow model for object picking
    if (isShadow){
		//addMouseHandlers();

        updateShadowModelImpl();

        QObject::connect(this, SIGNAL(_generateSupport()), this, SLOT(generateSupport()));
        QObject::connect(this, SIGNAL(_updateModelMesh(bool)), this, SLOT(updateModelMesh(bool)));

        labellingTextPreview = new LabellingTextPreview(this);
    }
	else
	{
		// Add to Part List
		qmlManager->addPart(getFileName(fname.toStdString().c_str()), ID);

		m_meshMaterial = new QPhongMaterial();
		m_meshVertexMaterial = new QPerVertexColorMaterial();

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
		updateAllVertices(_mesh);
		//applyGeometry();

		// 승환 50%
		qmlManager->setProgress(0.49);
		Qt3DExtras::QDiffuseMapMaterial* diffuseMapMaterial = new Qt3DExtras::QDiffuseMapMaterial();

		addComponent(m_meshMaterial);

		qDebug() << "created original model";

		//repairMesh(_mesh);
		addShadowModel(_mesh);

		// 승환 75%
		qmlManager->setProgress(0.73);

		QObject::connect(this, SIGNAL(bisectDone(Mesh*, Mesh*)), this, SLOT(generateRLModel(Mesh*, Mesh*)));
		QObject::connect(this, SIGNAL(_generateSupport()), this, SLOT(generateSupport()));
		QObject::connect(this, SIGNAL(_updateModelMesh(bool)), this, SLOT(updateModelMesh(bool)));

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
        emit _updateModelMesh(true);
    } else {
        updateLock = false;
        qDebug() << "no undo state";
        int saveCnt = (_mesh->getFaces()->size()>100000)? 3: 10;
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
        emit _updateModelMesh(true);
    } else {
        qDebug() << "no redo status";
    }
}
void GLModel::repairMesh()
{
    MeshRepair::repairMesh(_mesh);
    emit _updateModelMesh(true);
}
void GLModel::moveModelMesh(QVector3D direction, bool update){
    _mesh->vertexMove(direction);
    /*if (shadowModel != NULL)
        shadowModel->moveModelMesh(direction);*/
    qDebug() << "moved vertex";
    if(update)
    {
        emit _updateModelMesh(true);
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
    emit _updateModelMesh(true);
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
    emit _updateModelMesh(true);
}

void GLModel::rotateModelMesh(QMatrix4x4 matrix, bool update){
    _mesh->vertexRotate(matrix);
    if(update)
    {
        emit _updateModelMesh(true);
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
        shadowModel->scaleModelMesh(scale);*/

    emit _updateModelMesh(true);
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

void GLModel::updateModelMesh(bool shadowUpdate){
    // shadowUpdate updates shadow model of current Model
    QMetaObject::invokeMethod(qmlManager->boxUpperTab, "disableUppertab");
    QMetaObject::invokeMethod(qmlManager->boxLeftTab, "disableLefttab");
    qDebug() << "update Model Mesh";
    // reinitialize with updated mesh

    int viewMode = qmlManager->getViewMode();
    switch( viewMode ) {
    case VIEW_MODE_OBJECT:
		updateVertices(_mesh);
        break;
    case VIEW_MODE_SUPPORT:
        if (supportMesh != nullptr) {
			updateVertices(supportMesh);
        }
        else
        {
			updateVertices(_mesh);
        }
        break;
    case VIEW_MODE_LAYER:
        qDebug() << "in the glmodel view mode layer" << qmlManager->getLayerViewFlags() << " " << (qmlManager->getLayerViewFlags() & LAYER_SUPPORTERS);
        if( layerMesh != nullptr ) {
            int faces = layerMesh->getFaces()->size()*2 +
                    (qmlManager->getLayerViewFlags() & LAYER_INFILL != 0 ? layerInfillMesh->getFaces()->size()*2 : 0) +
                    (qmlManager->getLayerViewFlags() & LAYER_SUPPORTERS != 0 ? supportMesh->getFaces()->size()*2 : 0) +
                    (qmlManager->getLayerViewFlags() & LAYER_RAFT != 0 ? layerRaftMesh->getFaces()->size()*2 : 0);
			updateVertices(layerMesh);
            if( qmlManager->getLayerViewFlags() & LAYER_INFILL ) {
				updateVertices(layerInfillMesh, QVector3D(1.0f, 1.0f, 0.0f));
            }
            if( qmlManager->getLayerViewFlags() & LAYER_SUPPORTERS ) {
				updateVertices(supportMesh);
            }
            if( qmlManager->getLayerViewFlags() & LAYER_RAFT) {
				updateVertices(layerRaftMesh, QVector3D(0.0f, 0.0f, 0.0f));
            }
        } else {
            int faces = _mesh->getFaces()->size()*2 + ((supportMesh!=nullptr) ? supportMesh->getFaces()->size()*2:0);
			updateVertices(_mesh);
            if (supportMesh != nullptr)
				updateVertices(supportMesh);
        }
        break;
    }

    QVector3D tmp = m_transform.translation();
    float zlength = _mesh->z_max() - _mesh->z_min();
    m_transform.setTranslation(QVector3D(tmp.x(),tmp.y(),-_mesh->z_min()));
    qmlManager->sendUpdateModelInfo();
    checkPrintingArea();
    qDebug() << "model transform :" <<m_transform.translation() << _mesh->x_max() << _mesh->x_min() << _mesh->y_max() << _mesh->y_min() << _mesh->z_max() << _mesh->z_min();


    // create new object picker, shadowmodel, remove prev shadowmodel
    if (shadowUpdate){
        switch( viewMode ) {
            case VIEW_MODE_OBJECT:
                shadowModel->updateShadowModelImpl();
                break;
            case VIEW_MODE_SUPPORT:
                shadowModel->updateShadowModelImpl();
                shadowModel->m_transform.setTranslation(shadowModel->m_transform.translation()+QVector3D(0,0,scfg->raft_thickness));
                break;
            case VIEW_MODE_LAYER:
                shadowModel->updateShadowModelImpl();
                //addShadowModel(_mesh);
                break;
        }
    }
    updateLock = false;
    qDebug() << this << "released lock";
    QMetaObject::invokeMethod(qmlManager->boxUpperTab, "enableUppertab");
    QMetaObject::invokeMethod(qmlManager->boxLeftTab, "enableLefttab");
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
                QFuture<Slicer*> future = QtConcurrent::run(se, &SlicingEngine::slice, data, m_glmodel->_mesh, m_glmodel->supportMesh, m_glmodel->raftMesh, fileName + "/" + m_glmodel->filename.split("/").last() );
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

                emit m_glmodel->_updateModelMesh(true);
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
    QByteArray newVertexArray(vertexBuffer.data().size(), 0);
    QByteArray newVertexNormalArray(vertexNormalBuffer.data().size(), 0);
    QByteArray newVertexColorArray(vertexColorBuffer.data().size(), 0);
    vertexBuffer.updateData(0, newVertexArray);
    vertexNormalBuffer.updateData(0, newVertexNormalArray);
    vertexColorBuffer.updateData(0, newVertexColorArray);
	positionAttribute.setCount(0);
	normalAttribute.setCount(0);
	colorAttribute.setCount(0);
}


void GLModel::clearVertices(){
    //uint vertexCount = positionAttribute->count();
    //vertexBuffer->
    //positionAttribute->setCount(vertexCount+vertexCount);
    return;
}

void GLModel::updateAllVertices(Mesh* mesh, QVector3D vertexColor)
{
	mesh->flushChanges();
	_currentVisibleMesh = mesh;
    int face_size = mesh->getFaces()->size();
    int face_idx = 0;
    for (const MeshFace& mf : *mesh->getFaces()){
        face_idx ++;
        if (face_idx %100 ==0)
            QCoreApplication::processEvents();
        std::vector<QVector3D> result_vs;
		std::vector<QVector3D> result_vns;
		std::vector<QVector3D> result_vcs;
		for (int fn = 0; fn <= 2; fn++) {
			result_vcs.push_back(vertexColor);
			result_vs.push_back(mf.mesh_vertex[fn]->position);
			result_vns.push_back(mf.mesh_vertex[fn]->vn);
			if (mf.mesh_vertex[fn]->vn[0] == 0 && \
				mf.mesh_vertex[fn]->vn[1] == 0 && \
				mf.mesh_vertex[fn]->vn[2] == 0)
			{
				result_vns.pop_back();
				result_vns.push_back(QVector3D(1, 1, 1));
			}
		}
        appendVertices(result_vs);
		appendNormalVertices(result_vns);
		appendColorVertices(result_vcs);
    }
	updateBoundingBox();
}


void GLModel::updateVertices(Mesh* mesh, QVector3D vertexColor)
{
	std::vector<Mesh::MeshOp> operations = mesh->flushChanges();
	bool tooManyChanges = (operations.size() / (mesh->getFaces()->size() * 3)) > 0.8f;
	if (_currentVisibleMesh != mesh  || tooManyChanges)
	{
		//if the mesh being updated is not the same as the visible one, we need to redraw everything
		//also update all if there are too many changes.
		clearMem();
		updateAllVertices(mesh, vertexColor);
		return;
	}

	for (auto& operation : operations)
	{
#ifdef _STRICT_GLMODEL
		if (operation.Type != Mesh::MeshOpType::Modify && operation.Operand == Mesh::MeshOpOperand::VertexSingle)
			throw std::runtime_error("invalid mesh operation vertex arument on non-Modify");
#endif
		switch (operation.Type)
		{
		case Mesh::MeshOpType::Append:
		{
			if (operation.Operand == Mesh::MeshOpOperand::FaceSingle)
			{
				auto facePtr = std::get< const MeshFace*>(operation.Data);
				for (size_t i = 0; i < 3; ++i)
				{
					const auto& vtx = facePtr->mesh_vertex[i];
					//sigh
					addVertex(vtx->position, vtx->vn, vertexColor);

				}
			}
			else
			{
#ifdef _STRICT_GLMODEL
				throw std::runtime_error("range for append operation not implemented");
#endif
			}
			break;
		}
		case Mesh::MeshOpType::Modify:
		{
			if (operation.Operand == Mesh::MeshOpOperand::FaceRange)
			{
				auto range = std::get< std::pair<size_t, size_t> >(operation.Data);
				for (size_t i = range.first; i != range.second; ++i)
				{
					auto facePtr = (*mesh->getRenderOrderFaces())[i];
					updateFace(facePtr);
				}
			}

			else if (operation.Operand == Mesh::MeshOpOperand::FaceSingle)
			{
				auto facePtr = std::get< const MeshFace*  >(operation.Data);
				updateFace(facePtr);

			}
			else
			{
				auto vtxPtr = std::get< const MeshVertex*  >(operation.Data);
				auto faces = vtxPtr->connected_faces;
				for (auto each : faces)
				{
					updateFace(each);
				}
			}
			break;
		}
		case Mesh::MeshOpType::Delete:
		{
			if (operation.Operand == Mesh::MeshOpOperand::FaceSingle)
			{
				auto deleteIdx = std::get<size_t>(operation.Data);

				deleteAndShiftFaces(deleteIdx, 1);
			}
			else if (operation.Operand == Mesh::MeshOpOperand::FaceRange)
			{
				auto deleteRange = std::get<std::pair<size_t, size_t>>(operation.Data);

				deleteAndShiftFaces(deleteRange.first, deleteRange.second - deleteRange.first );
			}
			else
			{
#ifdef _STRICT_GLMODEL
				throw std::runtime_error("range for delete operation not implemented");
#endif

			}
			break;
		}
		}

	}
	updateBoundingBox();
}


inline void appendOrResizeBuffer(const QVector3D& data, QAttribute& attr, Qt3DRender::QBuffer& buffer)
{
	//update geometry
	QByteArray appendVertexArray;
	size_t newDataSize = 3 * sizeof(float);
	appendVertexArray.resize(newDataSize);
	float* reVertexArray = reinterpret_cast<float*>(appendVertexArray.data());

	//coordinates of left vertex
	reVertexArray[0] = data.x();
	reVertexArray[1] = data.y();
	reVertexArray[2] = data.z();

	uint vertexCount = attr.count();
	//if the attribute array is too small
	auto bufferSize = buffer.data().size();
	if((vertexCount* 3 * sizeof(float) + newDataSize) > bufferSize)
	{
		auto oldData = buffer.data();
		oldData.resize(oldData.size() + ATTRIBUTE_SIZE_INCREMENT);
		buffer.setData(oldData);
	}
	buffer.updateData(vertexCount * 3 * sizeof(float), appendVertexArray);
	attr.setCount(vertexCount + 1);
	return;
}
void GLModel::addVertex(QVector3D pos, QVector3D normal, QVector3D color) 
{
	appendOrResizeBuffer(pos, positionAttribute, vertexBuffer);
	appendOrResizeBuffer(normal, normalAttribute, vertexNormalBuffer);
	appendOrResizeBuffer(color, colorAttribute, vertexColorBuffer);
}

inline void updateBuffer(const QVector3D& data, QAttribute& attr, Qt3DRender::QBuffer& buffer, size_t offset)
{
	//update geometry
	QByteArray appendVertexArray;
	size_t newDataSize = 3 * sizeof(float);
	appendVertexArray.resize(newDataSize);
	float* reVertexArray = reinterpret_cast<float*>(appendVertexArray.data());

	//coordinates of left vertex
	reVertexArray[0] = data.x();
	reVertexArray[1] = data.y();
	reVertexArray[2] = data.z();

	buffer.updateData(offset, appendVertexArray);
	return;
}
void GLModel::updateBoundingBox()
{
	_boundingBox.setPos(m_transform.translation()+
		QVector3D((getMesh()->x_max()+getMesh()->x_min())/2,
		(getMesh()->y_max()+getMesh()->y_min())/2,
		(getMesh()->z_max()+getMesh()->z_min())/2));
	_boundingBox.setSize({
		(getMesh()->x_max() - getMesh()->x_min()),
		(getMesh()->y_max() - getMesh()->y_min()),
		(getMesh()->z_max() - getMesh()->z_min())
		});
}
void GLModel::updateFace(const MeshFace* face)
{
	size_t startingOffset = face->idx * 3 * 3* sizeof(float);
	size_t offset = 0;
	for (size_t i = 0; i < 3; ++i)
	{
		offset = i * sizeof(float) * 3;
		auto vtx = face->mesh_vertex[i];
		updateBuffer(vtx->position, positionAttribute, vertexBuffer, startingOffset + offset);
		QVector3D normal = vtx->vn;
		if (vtx->vn == QVector3D{0, 0, 0})
		{
			normal = { 1,1,1 };
		}
		updateBuffer(normal, normalAttribute, vertexNormalBuffer, startingOffset + offset);
		//updateBuffer(vtx->color, colorAttribute, vertexColorBuffer,  startingOffset + offset);
	}
}

inline void deleteAndShiftBuffer(QAttribute& attr, Qt3DRender::QBuffer& buffer, size_t index, size_t amount)
{
	size_t eraseAmount = amount * 3 * 3 * sizeof(float);

	QByteArray copy = buffer.data();
	copy.remove(index*  3 * 3 * sizeof(float), eraseAmount);
	buffer.setData(copy);
	attr.setCount(attr.count() - amount);
	return;
}
void GLModel::deleteAndShiftFaces(size_t start, size_t deleteAmount)
{
	deleteAndShiftBuffer(positionAttribute, vertexBuffer, start, deleteAmount);
	deleteAndShiftBuffer(normalAttribute, vertexNormalBuffer, start, deleteAmount);
	deleteAndShiftBuffer(colorAttribute, vertexColorBuffer, start, deleteAmount);

}
void GLModel::appendVertices(std::vector<QVector3D> vertices){

    int appendVertexCount = vertices.size();
    QByteArray appendVertexArray;
    appendVertexArray.resize(appendVertexCount*3*sizeof(float));
    float* reVertexArray = reinterpret_cast<float*>(appendVertexArray.data());

    for (int i=0; i<appendVertexCount; i++){
        //coordinates of left vertex
        reVertexArray[i*3+0] = vertices[i].x();
        reVertexArray[i*3+1] = vertices[i].y();
        reVertexArray[i*3+2] = vertices[i].z();
    }

    uint currentVertexCount = positionAttribute.count();
    //qDebug() << "position Attribute size : " << (int)currentVertexCount;
    int currentVertexArraySize = currentVertexCount*3*sizeof(float);
    int appendVertexArraySize = appendVertexArray.size();

    if ((currentVertexArraySize  + appendVertexArraySize) > vertexBuffer.data().size()) {
        auto data = vertexBuffer.data();
        int currentVertexArraySize = data.size();

        int countPowerOf2 = 1;
        while (countPowerOf2 < currentVertexArraySize + appendVertexArraySize) {
            countPowerOf2 <<= 1;
        }

        data.resize(countPowerOf2);
        vertexBuffer.setData(data);
    }

    vertexBuffer.updateData(currentVertexArraySize, appendVertexArray);
    positionAttribute.setCount(currentVertexCount + appendVertexCount);
}

const Mesh* GLModel::getMesh()
{
    return _mesh;
}



void GLModel::appendNormalVertices(std::vector<QVector3D> vertices){
    int vertex_normal_cnt = vertices.size();
    QByteArray appendVertexArray;
    appendVertexArray.resize(vertex_normal_cnt*3*sizeof(float));
    float * reVertexArray = reinterpret_cast<float*>(appendVertexArray.data());

    for (int i=0; i<vertex_normal_cnt; i++){

        //coordinates of left vertex
        reVertexArray[3*i+0] = vertices[i].x();
        reVertexArray[3*i+1] = vertices[i].y();
        reVertexArray[3*i+2] = vertices[i].z();
    }

    uint vertexNormalCount = normalAttribute.count();

    int offset = vertexNormalCount*3*sizeof(float);
    int bytesSize = appendVertexArray.size();

    if ((offset + bytesSize) > vertexNormalBuffer.data().size()) {
        auto data = vertexNormalBuffer.data();

        int countPowerOf2 = 1;
        while (countPowerOf2 < data.size() + bytesSize) {
            countPowerOf2 <<= 1;
        }

        data.resize(countPowerOf2);

        vertexNormalBuffer.setData(data);
    }

    vertexNormalBuffer.updateData(offset, appendVertexArray);
    normalAttribute.setCount(vertexNormalCount+ vertex_normal_cnt);
    return;
}

void GLModel::appendColorVertices(std::vector<QVector3D> vertices){
    int vertex_color_cnt = vertices.size();
    QByteArray appendVertexArray;
    appendVertexArray.resize(vertex_color_cnt*3*sizeof(float));
    float * reVertexArray = reinterpret_cast<float*>(appendVertexArray.data());

//    for (int i=0; i<vertex_color_cnt; i++){
//        //coordinates of left vertex
//        //97 185 192
//        //reVertexArray[3*i+0] = 0.38;
//        //reVertexArray[3*i+1] = 0.725;
//        //reVertexArray[3*i+2] = 0.753;
//        reVertexArray[3*i+0] = 0.278;
//        reVertexArray[3*i+1] = 0.670;
//        reVertexArray[3*i+2] = 0.706;
//    }

    int i = 0;
    for( auto iter = vertices.begin() ; iter != vertices.end() ; iter++, i++ ) {
        reVertexArray[3*i+0] = (*iter).x();
        reVertexArray[3*i+1] = (*iter).y();
        reVertexArray[3*i+2] = (*iter).z();
    }

    uint vertexColorCount = colorAttribute.count();

    int offset = vertexColorCount*3*sizeof(float);
    int bytesSize = appendVertexArray.size();

    if ((offset + bytesSize) > vertexColorBuffer.data().size()) {
        auto data = vertexColorBuffer.data();

        int countPowerOf2 = 1;
        while (countPowerOf2 < data.size() + bytesSize) {
            countPowerOf2 <<= 1;
        }

        data.resize(countPowerOf2);

        vertexColorBuffer.setData(data);
    }

    vertexColorBuffer.updateData(vertexColorCount*3*sizeof(float), appendVertexArray);
    colorAttribute.setCount(vertexColorCount + vertex_color_cnt);
    return;
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
    float min = world2Screen(cuttingPoints[0]).distanceToPoint(pickPosition);
    for (int i=0; i<cuttingPoints.size(); i++){

        if (world2Screen(cuttingPoints[i]).distanceToPoint(pickPosition) < min){
            minIdx = i;
            min = world2Screen(cuttingPoints[i]).distanceToPoint(pickPosition);
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
    if ((pick->position().x() < 260 && pick->position().y() < 330)|| cutMode == 1 || pick->button() != Qt3DRender::QPickEvent::Buttons::LeftButton) // cut panel and if cut mode isn't freecut
        return;

    QVector3D v = pick->localIntersection();
    QVector3D result_v = QVector3D(v.x(), -v.z(), v.y());
    result_v = result_v*2;

    bool found_nearby_v = false;


    for (int i=0; i<parentModel->cuttingPoints.size(); i++){
        if (result_v.distanceToPoint(parentModel->cuttingPoints[i]) <0.5f){;
            parentModel->removeCuttingPoint(i);
            found_nearby_v = true;
            break;
        }
    }

    if (!found_nearby_v){
        parentModel->addCuttingPoint(result_v);
    }

    // remove cutting contour and redraw cutting contour
    parentModel->removeCuttingContour();
    if (parentModel->cuttingPoints.size() >=2){
        qDebug() << "tt";
        parentModel->generateCuttingContour(parentModel->cuttingPoints);
        parentModel->regenerateCuttingPoint(parentModel->cuttingPoints);
    }


    if (parentModel->cuttingPoints.size() >= 3)
        QMetaObject::invokeMethod(qmlManager->cutPopup, "colorApplyFinishButton", Q_ARG(QVariant, 2));
    else
        QMetaObject::invokeMethod(qmlManager->cutPopup, "colorApplyFinishButton", Q_ARG(QVariant, 0));

}

void GLModel::mouseClicked(QPickEvent *pick)
{
    qDebug() << "handle Picker clicked" << pick->buttons() << pick->button();

    /*if (pick->button() == Qt::RightButton)
        return;*/

    if (!parentModel)
        return;

    if (qmlManager->getViewMode() == VIEW_MODE_SUPPORT) {
        qmlManager->openYesNoPopUp(false, "", "Support will disappear.", "", 18, "", ftrSupportDisappear, 1);
        return;
    }

    //---------------- rgoo routine init --------------------
    ////m_objectPicker->setDragEnabled(false);
    m_transform.setScale(1.0f);
    qDebug() << "setting scale back to 1.0";
    qmlManager->resetCursor();

    isReleased = true;
    qDebug() << "Released";

	if (isMoved) {
		if (components().size() > 4)
		{
			removeComponent(dragMesh);
			qmlManager->fixMesh();
			qDebug() << "dragMesh removed";
		}
		//removeComponent(dragMesh);

		qmlManager->modelMoveDone();
		isMoved = false;
		return;
	}


    //---------------- rgoo routine end --------------------

    if (qmlManager->yesno_popup->property("isFlawOpen").toBool())
        return;

    if (qmlManager->selectedModels[0] != nullptr && !(pick->button() & Qt::LeftButton)){ // when right button clicked
        if (qmlManager->selectedModels[0]->ID == parentModel->ID){
            qDebug() << "mttab alive 1";
            QMetaObject::invokeMethod(qmlManager->mttab, "tabOnOff");
            //m_objectPicker->setEnabled(false);
            return;
        }

    }



    if (!cutActive && !extensionActive && !labellingActive && !layflatActive && !manualSupportActive && !isMoved)// && !layerViewActive && !supportViewActive)
        qmlManager->modelSelected(parentModel->ID);

    qDebug() << "model selected emit" << pick->position() << parentModel->ID;
    qDebug() << "pick button : " << !(pick->button() & Qt::LeftButton) << pick->button();

    if (!(pick->button() & Qt::LeftButton)){
        qDebug() << "mttab alive";
        QMetaObject::invokeMethod(qmlManager->mttab, "tabOnOff");
    }

    QPickTriangleEvent *trianglePick = static_cast<QPickTriangleEvent*>(pick);
    qDebug() << "trianglePick : " << trianglePick << qmlManager->selectedModels.size() << qmlManager;
	if (trianglePick->triangleIndex() >= _mesh->getRenderOrderFaces()->size() )
	{
		qDebug() << "trianglePick out of bound";

		return;
	}
	const MeshFace* shadow_meshface = (*_mesh->getRenderOrderFaces())[trianglePick->triangleIndex()];
	parentModel->targetMeshFace = shadow_meshface->parentFace;
	qDebug() << "found parent meshface" << shadow_meshface->parentFace->idx;

    if (labellingActive && trianglePick && trianglePick->localIntersection() != QVector3D(0,0,0)) {

        //parentModel->uncolorExtensionFaces();
        //parentModel->generateColorAttributes();
        //parentModel->colorExtensionFaces();

        QMetaObject::invokeMethod(qmlManager->labelPopup, "labelUpdate");

        if (labellingTextPreview && labellingTextPreview->isEnabled()) {
            labellingTextPreview->setTranslation(pick->localIntersection() + parentModel->targetMeshFace->fn);
            labellingTextPreview->setNormal(parentModel->targetMeshFace->fn);
            labellingTextPreview->planeSelected = true;
        }
        else {
            labellingTextPreview->planeSelected = false;
        }
/*
        QString label_text = "";
        QString label_font = "";
        bool label_font_bold = false;
        int label_size = 12;
        int contentWidth = 64;
        if (labellingTextPreview){
            label_text = labellingTextPreview->text;
            label_font = labellingTextPreview->fontName;
            label_font_bold = (labellingTextPreview->fontWeight==QFont::Bold)? true:false;
            label_size = labellingTextPreview->fontSize;
            contentWidth = labellingTextPreview->contentWidth;
            labellingTextPreview->deleteLater();
            labellingTextPreview = nullptr;
            qDebug() << "labelling Text existed!" << label_text << label_font << label_size;
        }
        else
            qDebug()<<"no labelling text";
        //qDebug() << "me:"<<this << "parent:"<<this->parentModel<<"shadow:"<<this->shadowModel;
        labellingTextPreview = new LabellingTextPreview(this);
        labellingTextPreview->setEnabled(true);

        if (labellingTextPreview && labellingTextPreview->isEnabled()) {
            labellingTextPreview->setTranslation(pick->localIntersection() + parentModel->targetMeshFace->fn);
            labellingTextPreview->setNormal(parentModel->targetMeshFace->fn);
            if (label_text != ""){
                labellingTextPreview->setFontName(label_font);
                labellingTextPreview->setFontBold(label_font_bold);
                labellingTextPreview->setFontSize(label_size);
                labellingTextPreview->setText(label_text, contentWidth);
            }
            labellingTextPreview->planeSelected = true;
        }
        else {
            qDebug() << "pinpin ^^^^^^^^^^^^^^^^^ 1";
            labellingTextPreview->planeSelected = false;
        }*/
    }


    if (cutActive){
        if (cutMode == 1){
            qDebug() << "cut mode 1";

        } else if (cutMode == 2){// && parentModel->numPoints< sizeof(parentModel->sphereEntity)/4) {
            qDebug() << pick->localIntersection()<<"pick" << cuttingPoints.size() << parentModel->cuttingPoints.size();
            QVector3D v = pick->localIntersection();
            qDebug() << v.distanceToPoint(parentModel->cuttingPoints[parentModel->cuttingPoints.size()-1]);

            bool found_nearby_v = false;
            for (int i=0; i<parentModel->cuttingPoints.size(); i++){
                if (v.distanceToPoint(parentModel->cuttingPoints[i]) <0.5f){
                    parentModel->removeCuttingPoint(i);
                    found_nearby_v = true;
                    break;
                }
            }

            if (!found_nearby_v){
                qDebug() << "Check 0 " << parentModel->cuttingPoints.size();
                parentModel->addCuttingPoint(v+m_transform.translation());
                qDebug() << "Check 1 " << parentModel->cuttingPoints.size();
            }

            parentModel->removeCuttingContour();
            if (parentModel->cuttingPoints.size() >= 2){
                parentModel->generateCuttingContour(parentModel->cuttingPoints);
                parentModel->regenerateCuttingPoint(parentModel->cuttingPoints);
            }
            //generatePlane();
            //parentModel->ft->ct->addCuttingPoint(parentModel, v);
        } else if (cutMode == 9999){
            qDebug() << "current cut mode :" << cutMode;
            qmlManager->setProgress(1);
            //return;
        }
    }

    if (extensionActive && trianglePick  && trianglePick->localIntersection() != QVector3D(0,0,0)){
        qDebug() << "found parent meshface" << shadow_meshface->parentFace->idx;
        parentModel->uncolorExtensionFaces();
        emit extensionSelect();
        parentModel->generateColorAttributes();

        parentModel->colorExtensionFaces();
    }

    if (hollowShellActive){
        qDebug() << "getting handle picker clicked signal hollow shell active";
        qDebug() << "found parent meshface" << shadow_meshface->parentFace->idx;
        // translate hollowShellSphere to mouse position
        QVector3D v = pick->localIntersection();
        QVector3D tmp = m_transform.translation();
        float zlength = _mesh->z_max() - _mesh->z_min();
        qmlManager->hollowShellSphereTransform->setTranslation(v + QVector3D(tmp.x(),tmp.y(),-_mesh->z_min()));


    }

    if (layflatActive && trianglePick && trianglePick->localIntersection() != QVector3D(0,0,0)){

        qDebug() << "target 1 " <<shadow_meshface->fn ;

        parentModel->uncolorExtensionFaces();
        emit layFlatSelect();
        parentModel->generateColorAttributes();
        parentModel->colorExtensionFaces();
    }

    if (manualSupportActive && trianglePick  && trianglePick->localIntersection() != QVector3D(0,0,0)){
        qDebug() << "manual support handle picker clicked";
        parentModel->uncolorExtensionFaces();
        emit extensionSelect();
        parentModel->generateColorAttributes();
        parentModel->colorExtensionFaces();
    }
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
    //shadowModel->//m_objectPicker->setEnabled(true);
    //closeLayflat();
    emit resetLayflat();
}

// need to create new _mesh object liek Mesh* leftMesh = new Mesh();
void GLModel::bisectModel(Plane plane, Mesh& lmesh, Mesh& rmesh){
    // if the cutting plane is at min or max, it will not going to cut and just end
    if (isFlatcutEdge && !shadowModel->shellOffsetActive){
        shadowModel->removePlane();
        removeCuttingPoints();
        removeCuttingContour();
        QMetaObject::invokeMethod(qmlManager->boxUpperTab, "all_off");
        qmlManager->setProgress(1);
		
        _boundingBox.setEnabled(false);
        return;
    }

    // do bisecting _mesh

    Paths3D cuttingEdges;

	for (const MeshFace& mf : *_mesh->getFaces()) {
		bool faceLeftToPlane = false;
        bool faceRightToPlane = false;
        Plane target_plane;
        for (int vn=0; vn<3; vn++){
            const MeshVertex& mv = *mf.mesh_vertex[vn];
            target_plane.push_back(mv.position);
            if (modelcut::isLeftToPlane(plane, mv.position)) // if one vertex is left to plane, append to left vertices part
                faceLeftToPlane = true;
            else {
                faceRightToPlane = true;
            }
        }
        if (faceLeftToPlane && faceRightToPlane){ // cutting edge
            Path3D intersection = _mesh->intersectionPath(plane, target_plane);
            cuttingEdges.push_back(intersection);
            std::vector<QVector3D> upper;
            std::vector<QVector3D> lower;

            for (int i=0; i<3; i++){
                if (target_plane[i].distanceToPlane(plane[0],plane[1],plane[2]) >0)
                    upper.push_back(target_plane[i]);
                else
                    lower.push_back(target_plane[i]);
            }

            QVector3D target_plane_normal = QVector3D::normal(target_plane[0], target_plane[1], target_plane[2]);

            if (upper.size() == 2){
                float dotproduct = QVector3D::dotProduct(target_plane_normal, QVector3D::normal(upper[1],upper[0],intersection[1].position));
                bool facingNormal = dotproduct>0;//QVector3D::normal(lower[0], intersection[0].position, intersection[1].position))>0;//abs((target_plane_normal- QVector3D::normal(lower[0], intersection[0].position, intersection[1].position)).length())<1;
                bool facingNormalAmbiguous = abs(dotproduct)<0.1;
                if (facingNormal || facingNormalAmbiguous){
                    rmesh.addFace(upper[1], upper[0], intersection[1].position);
                    rmesh.addFace(intersection[1].position, intersection[0].position, upper[1]);
                    lmesh.addFace(lower[0], intersection[0].position, intersection[1].position);
                }
                if (!facingNormal || facingNormalAmbiguous) {
                    rmesh.addFace(upper[0], upper[1], intersection[1].position);
                    rmesh.addFace(intersection[0].position, intersection[1].position, upper[1]);
                    lmesh.addFace(intersection[0].position, lower[0], intersection[1].position);
                }
            } else if (lower.size() == 2){
                float dotproduct = QVector3D::dotProduct(target_plane_normal, QVector3D::normal(lower[0], intersection[1].position, intersection[0].position));
                bool facingNormal = dotproduct>0;
                bool facingNormalAmbiguous = abs(dotproduct)<0.1;

                if (facingNormal || facingNormalAmbiguous){
                    lmesh.addFace(lower[0], intersection[1].position, intersection[0].position);
                    lmesh.addFace(lower[0], lower[1], intersection[1].position);
                    rmesh.addFace(upper[0], intersection[0].position, intersection[1].position);
                }
                if (!facingNormal || facingNormalAmbiguous){
                    lmesh.addFace(intersection[1].position, lower[0], intersection[0].position);
                    lmesh.addFace(lower[1], lower[0], intersection[1].position);
                    rmesh.addFace(intersection[0].position, upper[0], intersection[1].position);
                }
            } else {
                qDebug() << "wrong faces";
            }


        } else if (faceLeftToPlane){
            lmesh.addFace(mf.mesh_vertex[0]->position, mf.mesh_vertex[1]->position, mf.mesh_vertex[2]->position);
        } else if (faceRightToPlane){											   							   
            rmesh.addFace(mf.mesh_vertex[0]->position, mf.mesh_vertex[1]->position, mf.mesh_vertex[2]->position);
        }
    }


    if (cutFillMode == 2){ // if fill holes
        // contour construction
        Paths3D contours = contourConstruct3D(cuttingEdges);

        QVector3D plane_normal = QVector3D::normal(plane[0], plane[1], plane[2]);

        // copy paths3d to std::vector<containmentPath*>
        for (int c=0; c<contours.size(); c++){
        //for (Path3D contour : contours){
            for (MeshVertex mv : contours[c]){
                IntPoint ip;
                ip.X = round(mv.position.x()*scfg->resolution);
                ip.Y = round(mv.position.y()*scfg->resolution);
                contours[c].projection.push_back(ip);
            }
        }

        // search for containment tree construction
        for (int target_idx=0; target_idx<contours.size(); target_idx++){
            for (int in_idx=0; in_idx<contours.size(); in_idx++){
            //for (Path3D in_path : contours){
                if ((contours[in_idx].size()>3) && (contours[target_idx].size()>3)
                        && (target_idx != in_idx) && (pathInpath(contours[target_idx], contours[in_idx]))){
                    contours[target_idx].outer.push_back(contours[in_idx]);
                    contours[in_idx].inner.push_back(contours[target_idx]);
                }
            }
        }

        qDebug() << "after cutting edges :" << contours.size();

        /*// contour 2 polygon done by poly2tri
        std::vector<p2t::Point*> ContourPoints;

        for (Path3D contour : contours){
            for (MeshVertex mv : contour){
                //ContourPoints.push_back(new p2t::Point(mv.position.x(), mv.position.y()));
                ContourPoints.push_back(new p2t::Point(mv.position.y(), mv.position.z()));
            }
        }
        qDebug() << "putted contourPoints";

        float d = plane_normal.x()*plane[0].x() +
                plane_normal.y()*plane[0].y() +
                plane_normal.z()*plane[0].z();

        p2t::CDT cdt(ContourPoints);
        cdt.Triangulate();
        std::vector<p2t::Triangle*> triangles = cdt.GetTriangles();
        qDebug() << "done triangulation";

        for (p2t::Triangle* triangle : triangles){
            leftMesh->addFace(QVector3D(triangle->GetPoint(0)->x, triangle->GetPoint(0)->y,
                                        0),//(d - (plane_normal.x()*triangle->GetPoint(0)->x) - (plane_normal.y()*triangle->GetPoint(0)->y))/plane_normal.z()),
                              QVector3D(triangle->GetPoint(1)->x, triangle->GetPoint(1)->y,
                                        0),//(d - (plane_normal.x()*triangle->GetPoint(1)->x) - (plane_normal.y()*triangle->GetPoint(1)->y))/plane_normal.z()),
                              QVector3D(triangle->GetPoint(2)->x, triangle->GetPoint(2)->y,
                                        0));//(d - (plane_normal.x()*triangle->GetPoint(2)->x) - (plane_normal.y()*triangle->GetPoint(2)->y))/plane_normal.z()));
            rightMesh->addFace(QVector3D(triangle->GetPoint(1)->x, triangle->GetPoint(1)->y,
                                         0),//(d - (plane_normal.x()*triangle->GetPoint(1)->x) - (plane_normal.y()*triangle->GetPoint(1)->y))/plane_normal.z()),
                               QVector3D(triangle->GetPoint(0)->x, triangle->GetPoint(0)->y,
                                         0),//(d - (plane_normal.x()*triangle->GetPoint(0)->x) - (plane_normal.y()*triangle->GetPoint(0)->y))/plane_normal.z()),
                               QVector3D(triangle->GetPoint(2)->x, triangle->GetPoint(2)->y,
                                         0));//(d - (plane_normal.x()*triangle->GetPoint(2)->x) - (plane_normal.y()*triangle->GetPoint(2)->y))/plane_normal.z()));
        }*/


        for (Path3D contour : contours){
            qDebug() << "contour size : " << contour.size();
            if (contour.size() <= 2){
                continue;
            }

            QVector3D centerOfMass = QVector3D(0,0,0);
            for (MeshVertex mv : contour){
                centerOfMass += mv.position;
            }
            centerOfMass /= contour.size();

            // get orientation
            bool ccw = true;
            QVector3D current_plane_normal = QVector3D::normal(contour[1].position, centerOfMass, contour[0].position);
            if (QVector3D::dotProduct(current_plane_normal, plane_normal)>0){
                ccw = false;
            }

            for (int i=0; i<contour.size(); i++){
                if (ccw){
                    lmesh.addFace(contour[i].position, centerOfMass, contour[(i+1)%contour.size()].position);
                    rmesh.addFace(contour[(i+1)%contour.size()].position, centerOfMass, contour[i].position);
                } else {
                    lmesh.addFace(contour[(i+1)%contour.size()].position, centerOfMass, contour[i].position);
                    rmesh.addFace(contour[i].position, centerOfMass, contour[(i+1)%contour.size()].position);
                }
            }
        }

    }


    qDebug() << "done bisect";

    // 승환 30%
    qmlManager->setProgress(0.22);
    lmesh.connectFaces();

    // 승환 40%
    qmlManager->setProgress(0.41);
    rmesh.connectFaces();

    // 승환 50%
    qmlManager->setProgress(0.56);
    qDebug() << "done connecting";

    emit bisectDone(&lmesh, &rmesh);
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
    if (parentModel->cuttingPoints.size()<3){
        qDebug()<<"Error: There is not enough vectors to render a plane.";
        return;
    }

    QVector3D v1;
    QVector3D v2;
    QVector3D v3;
    v1=parentModel->cuttingPoints[parentModel->cuttingPoints.size()-3];
    v2=parentModel->cuttingPoints[parentModel->cuttingPoints.size()-2];
    v3=parentModel->cuttingPoints[parentModel->cuttingPoints.size()-1];
    parentModel->planeMaterial = new Qt3DExtras::QPhongAlphaMaterial();
    parentModel->planeMaterial->setAmbient(QColor(QRgb(0xF4F4F4)));
    parentModel->planeMaterial->setDiffuse(QColor(QRgb(0xF4F4F4)));
    parentModel->planeMaterial->setSpecular(QColor(QRgb(0xF4F4F4)));
    parentModel->planeMaterial->setAlpha(0.5f);

    // set cutting plane
    parentModel->cuttingPlane.clear();
    parentModel->cuttingPlane.push_back(v1);
    parentModel->cuttingPlane.push_back(v2);
    parentModel->cuttingPlane.push_back(v3);

    //To manipulate plane color, change the QRgb(0x~~~~~~).
    QVector3D tmp = parentModel->m_transform.translation();
    QVector3D world_origin(0,0,0);
    QVector3D original_normal(0,1,0);
    QVector3D desire_normal(QVector3D::normal(v1,v2,v3)); //size=1
    float angle = qAcos(QVector3D::dotProduct(original_normal,desire_normal))*180/M_PI;
    QVector3D crossproduct_vector(QVector3D::crossProduct(original_normal,desire_normal));

    for (int i=0;i<2;i++){
        parentModel->planeEntity[i] = new Qt3DCore::QEntity(parentModel->parentModel);
        //qDebug() << "generatePlane---------------------==========-=-==-" << parentModel->parentModel;
        parentModel->clipPlane[i]=new Qt3DExtras::QPlaneMesh(this);
        parentModel->clipPlane[i]->setHeight(100.0);
        parentModel->clipPlane[i]->setWidth(100.0);

        parentModel->planeTransform[i]=new Qt3DCore::QTransform();
        parentModel->planeTransform[i]->setScale(2.0f);
        parentModel->planeTransform[i]->setRotation(QQuaternion::fromAxisAndAngle(crossproduct_vector, angle+180*i));
        float zlength = parentModel->_mesh->z_max() - parentModel->_mesh->z_min();
        parentModel->planeTransform[i]->setTranslation(desire_normal*(-world_origin.distanceToPlane(v1,v2,v3))+QVector3D(tmp.x(),tmp.y(),-parentModel->_mesh->z_min()));

        parentModel->planeObjectPicker[i] = new Qt3DRender::QObjectPicker;//parentModel->planeEntity[i]);

        parentModel->planeObjectPicker[i]->setHoverEnabled(true);
        parentModel->planeObjectPicker[i]->setEnabled(true);
        //QObject::connect(parentModel->planeObjectPicker[i], SIGNAL(clicked(Qt3DRender::QPickEvent*)), this, SLOT(mouseClicked(Qt3DRender::QPickEvent*)));
        QObject::connect(parentModel->planeObjectPicker[i], SIGNAL(released(Qt3DRender::QPickEvent*)), this, SLOT(mouseClickedFreeCut(Qt3DRender::QPickEvent*)));


        parentModel->planeEntity[i]->addComponent(parentModel->planeObjectPicker[i]);

        parentModel->planeEntity[i]->addComponent(parentModel->clipPlane[i]);
        parentModel->planeEntity[i]->addComponent(parentModel->planeTransform[i]); //jj
        parentModel->planeEntity[i]->addComponent(parentModel->planeMaterial);
    }

    parentModel->removeCuttingPoints();
    //modelCut();
}

void GLModel::generateSupport(){

    // copy _mesh data from original _mesh
    layerMesh = new Mesh;
	for (const MeshFace& mf : *_mesh->getFaces()) {
		layerMesh->addFace(mf.mesh_vertex[0]->position, mf.mesh_vertex[1]->position, mf.mesh_vertex[2]->position, &mf);
    }

    float x_length = _mesh->x_max() - _mesh->x_min();
    float y_length = _mesh->y_max() - _mesh->y_min();
    float z_length = _mesh->z_max() - _mesh->z_min();
    size_t xy_reserve = x_length * y_length;
    size_t xyz_reserve = xy_reserve * z_length;
    qDebug() << "********************xy_reserve = " << xy_reserve;
    qDebug() << "********************faces_reserve = " << _mesh->getFaces()->size();
    qDebug() << "********************vertices_reserve = " << _mesh->getVertices()->size();

    layerInfillMesh = new Mesh;
    layerSupportMesh = new Mesh;
    layerRaftMesh = new Mesh;

    QVector3D t = m_transform.translation();

    t.setZ(_mesh->z_min() * -1.0f);
    layerInfillMesh->vertexMove(t);
    layerSupportMesh->vertexMove(t);
    layerRaftMesh->vertexMove(t);

    // generate cylinders
    /*for( auto iter = slicer->slices.overhang_points.begin() ; iter != slicer->slices.overhang_points.end() ; iter++ ) {
        qDebug() << "-------" << (*iter)->position.X << (*iter)->position.Y << (*iter)->position.Z;
        generateSupporter(layerSupportMesh, *iter);
        generateRaft(layerRaftMesh, *iter);
    }*/

    /*for( auto iter = slicer->slices.begin() ; iter != slicer->slices.end() ; iter++ ) {
        qDebug() << "infile" << iter->infill.size() << "outershell" << iter->outershell.size() << "support" << iter->support.size() << "z" << iter->z;
        generateInfill(layerInfillMesh, &(*iter));
    }*/

    t.setZ(scfg->raft_thickness);
    layerMesh->vertexMove(t);
    t.setZ(_mesh->z_min() + scfg->raft_thickness);
    layerInfillMesh->vertexMove(t);
    layerSupportMesh->vertexMove(t);
    layerRaftMesh->vertexMove(t);

    layerInfillMesh->connectFaces();
    layerSupportMesh->connectFaces();
    layerRaftMesh->connectFaces();


    emit _updateModelMesh(true);
}

void GLModel::removePlane(){
    //qDebug() << "--remove plane called--" << this << parentModel;
    if (parentModel->planeMaterial == nullptr){
        qDebug() << "## parentModel have no planeMaterial";
        return;
    }
    delete parentModel->planeMaterial;
    parentModel->planeMaterial = nullptr;
    for (int i=0;i<2;i++){
        if (parentModel->clipPlane[i] != nullptr){
            delete parentModel->clipPlane[i];
            parentModel->clipPlane[i] = nullptr;
        }
        if (parentModel->planeTransform[i] != nullptr){
            delete parentModel->planeTransform[i];
            parentModel->planeTransform[i] = nullptr;
        }
        if (parentModel->planeObjectPicker[i] != nullptr){
            //QObject::disconnect(parentModel->planeObjectPicker[i], SIGNAL(clicked(Qt3DRender::QPickEvent*)), this, SLOT(mouseClickedFreeCut(Qt3DRender::QPickEvent*)));
            delete parentModel->planeObjectPicker[i];
            parentModel->planeObjectPicker[i] = nullptr;
        }
        if (parentModel->planeEntity[i] != nullptr){
            delete parentModel->planeEntity[i];
            parentModel->planeEntity[i] = nullptr;
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

    sphereEntity.push_back(new Qt3DCore::QEntity(parentModel));
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
    if(cutMode == 2 && parentModel->cuttingPoints.size() < 3)
        return ;

    parentModel->saveUndoState();
    qmlManager->openProgressPopUp();

	auto lmesh = new Mesh();
	auto rmesh = new Mesh();
    if (cutMode == 1){ // flat cut
        if (parentModel->cuttingPlane.size() != 3){
            return;
        }
        if (this->shellOffsetActive && parentModel->isFlatcutEdge == true) {
            getSliderSignal(0.0);
        }
        parentModel->bisectModel(parentModel->cuttingPlane, *lmesh, *rmesh);
    } else if (cutMode == 2){ // free cut
        if (parentModel->cuttingPoints.size() >= 3){
            // do bisecting _mesh

            modelcut::cutAway(lmesh, rmesh, parentModel->_mesh, parentModel->cuttingPoints, parentModel->cutFillMode);
            if (lmesh->getFaces()->size() == 0 || rmesh->getFaces()->size() == 0){
                qDebug() << "cutting contour selected not cutting";
                qmlManager->setProgress(1);
                //cutModeSelected(9999);
                cutModeSelected(2); // reset
                return;
            }
            emit parentModel->bisectDone(lmesh, rmesh);
        }
    }
}

void GLModel::generateRLModel(Mesh* lmesh, Mesh* rmesh){
    qDebug() << "** generateRLModel" << this;
    if (lmesh->getFaces()->size() != 0){
        qmlManager->createModelFile(lmesh, filename+"_l");
        qDebug() << "leftmodel created";
    }
    // 승환 70%
    qmlManager->setProgress(0.72);
    if (rmesh->getFaces()->size() != 0){
        qmlManager->createModelFile(rmesh, filename+"_r");
        qDebug() << "rightmodel created";
    }

    qDebug() << "finding models";

    // 승환 90%
    GLModel* leftmodel = qmlManager->findGLModelByName(filename+"_l");
    GLModel* rightmodel = qmlManager->findGLModelByName(filename+"_r");
    qDebug() << "found models : " << leftmodel << rightmodel;
    if (leftmodel != nullptr && rightmodel != nullptr){
        leftmodel->twinModel = rightmodel;
        rightmodel->twinModel = leftmodel;
    }

    qmlManager->setProgress(0.91);

    if (shadowModel->shellOffsetActive){
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

    qDebug() << "left right generated";

    //parentModel->deleteLater();
    shadowModel->removePlane();

    removeCuttingPoints();
    removeCuttingContour();

    qDebug() << "removed cutting plane";
    // delete original model
    qmlManager->deleteModelFile(ID);

    qDebug() << "deleted original model";
    // do auto arrange
    qmlManager->runArrange();
    _boundingBox.setEnabled(false);

    // 승환 100%
    qmlManager->setProgress(1);
}

// hollow shell part
void GLModel::indentHollowShell(double radius){
    qDebug() << "hollow shell called" << radius;

    QVector3D center = (targetMeshFace->mesh_vertex[0]->position +targetMeshFace->mesh_vertex[1]->position + targetMeshFace->mesh_vertex[2]->position)/3;
	HollowShell::hollowShell(_mesh, targetMeshFace, center, radius);
}

GLModel::~GLModel(){
    deleteShadowModel();
}


void GLModel::mgoo(Qt3DRender::QPickEvent* v)
{
    if(v->buttons()>1){
        return;
    }

    if (qmlManager->selectedModels[0] != nullptr &&
            (qmlManager->selectedModels[0]->shadowModel->scaleActive ||
             qmlManager->selectedModels[0]->shadowModel->cutActive ||
             qmlManager->selectedModels[0]->shadowModel->shellOffsetActive ||
             qmlManager->selectedModels[0]->shadowModel->extensionActive ||
             qmlManager->selectedModels[0]->shadowModel->labellingActive ||
             qmlManager->selectedModels[0]->shadowModel->layflatActive ||
             qmlManager->selectedModels[0]->shadowModel->layerViewActive ||
             qmlManager->selectedModels[0]->shadowModel->manualSupportActive ||
             qmlManager->selectedModels[0]->shadowModel->supportViewActive ||
             qmlManager->orientationActive ||
             qmlManager->rotateActive ||
             qmlManager->saveActive))
        return;

    // if not selected by qmlmanager, return
    bool modelSelected = false;
    for (GLModel* glm : qmlManager->selectedModels){
        if (glm == this->parentModel){
            modelSelected = true;
        }
    }
    if (!modelSelected){
        //m_objectPicker->setDragEnabled(false);
        isReleased = true;
        return;
    }

    qmlManager->moveButton->setProperty("state", "active");
    qmlManager->setClosedHandCursor();

    if (!isMoved){ // called only once on dragged
        parentModel->saveUndoState();
        qmlManager->hideMoveArrow();
        qDebug() << "hiding move arrow";
        // for mgoo out problem

        float x_diff = parentModel->_mesh->x_max() - parentModel->_mesh->x_min();
        float y_diff = parentModel->_mesh->y_max() - parentModel->_mesh->y_min();
        float z_diff = parentModel->_mesh->z_max() - parentModel->_mesh->z_min();
        float biggest = x_diff>y_diff? x_diff : y_diff;
        biggest = z_diff>biggest? z_diff : biggest;
        float scale_val = biggest > 50.0f ? 1.0f : 100.0f/biggest;
        m_transform.setScale(scale_val);

		if (components().size() < 5) {
			//removeComponent(dragMesh);
			dragMesh->setRadius(biggest);
			addComponent(dragMesh);
			//qDebug() << "COMPONENTS(A): " << components();
			//qDebug() << "dragMesh added";
		}
        isMoved = true;
    }

    QVector2D currentPoint = (QVector2D)v->position();

    QVector3D xAxis3D = QVector3D(1,0,0);
    QVector3D yAxis3D =  QVector3D(0,1,0);
    QVector2D xAxis2D = (world2Screen(lastpoint+xAxis3D) - world2Screen(lastpoint));
    QVector2D yAxis2D = (world2Screen(lastpoint+yAxis3D) - world2Screen(lastpoint));
    QVector2D target = currentPoint - prevPoint;

    float b = (target.y()*xAxis2D.x() - target.x()*xAxis2D.y())/
            (xAxis2D.x()*yAxis2D.y()-xAxis2D.y()*yAxis2D.x());
    float a = (target.x() - b*yAxis2D.x())/xAxis2D.x();

    // move ax + by amount
    qmlManager->modelMoveF(1,a);
    qmlManager->modelMoveF(2,b);

    //qmlManager->showMoveArrow();
    prevPoint = currentPoint;

}

void GLModel::pgoo(Qt3DRender::QPickEvent* v){
    qDebug() << "pgoo";
    if(v->buttons()>1) // pass if click with right mouse
        return;

    if (!isReleased) // to remove double click on release problem
        return;
    else
        isReleased = false;

    qDebug() << "Pressed   " << v->position() << m_transform.translation();
    //qmlManager->lastModelSelected();

    if(qmlManager->selectedModels[0] != nullptr) {
        //m_objectPicker->setDragEnabled(true);

        lastpoint=v->localIntersection();
        prevPoint = (QVector2D) v->position();
        qDebug() << "Dragged";
    }
}

void GLModel::cutModeSelected(int type){

    qDebug() << "cut mode selected1" << type;
    parentModel->removeCuttingPoints();
    //removePlane();
    parentModel->removeCuttingContour();
    //parentModel->removePlane();
    cutMode = type;
    if (cutMode == 1){
        parentModel->addCuttingPoint(QVector3D(1,0,0));
        parentModel->addCuttingPoint(QVector3D(1,1,0));
        parentModel->addCuttingPoint(QVector3D(2,0,0));
        generatePlane();
    } else if (cutMode == 2){
        qDebug() << QCursor::pos();
        parentModel->addCuttingPoint(QVector3D(1,0,0));
        parentModel->addCuttingPoint(QVector3D(1,1,0));
        parentModel->addCuttingPoint(QVector3D(2,0,0));
        generateClickablePlane(); // plane is also clickable
    } else {

    }
    return;
}

void GLModel::cutFillModeSelected(int type){
    parentModel->cutFillMode = type;
    return;
}

void GLModel::getSliderSignal(double value){
    if (cutActive||shellOffsetActive){
        if (value == 0.0 || value == 1.8){
            parentModel->isFlatcutEdge = true;
        }
        else {
            parentModel->isFlatcutEdge = false;
        }
        float zlength = parentModel->_mesh->z_max() - parentModel->_mesh->z_min();
        QVector3D v1(1,0, parentModel->_mesh->z_min() + value*zlength/1.8);
        QVector3D v2(1,1, parentModel->_mesh->z_min() + value*zlength/1.8);
        QVector3D v3(2,0, parentModel->_mesh->z_min() + value*zlength/1.8);

        QVector3D world_origin(0,0,0);
        QVector3D original_normal(0,1,0);
        QVector3D desire_normal(QVector3D::normal(v1,v2,v3)); //size=1
        float angle = qAcos(QVector3D::dotProduct(original_normal,desire_normal))*180/M_PI+(value-1)*30;
        QVector3D crossproduct_vector(QVector3D::crossProduct(original_normal,desire_normal));

        QVector3D tmp = parentModel->m_transform.translation();

        for (int i=0;i<2;i++){
            parentModel->planeTransform[i]->setTranslation(desire_normal*(-world_origin.distanceToPlane(v1,v2,v3)) +QVector3D(tmp.x(),tmp.y(),-parentModel->_mesh->z_min()));
            parentModel->planeEntity[i]->addComponent(parentModel->planeTransform[i]);
        }

        parentModel->cuttingPlane[0] = v1;
        parentModel->cuttingPlane[1] = v2;
        parentModel->cuttingPlane[2] = v3;
    } else if (hollowShellActive){
        // change radius of hollowShellSphere
        hollowShellRadius = value;
        qmlManager->hollowShellSphereMesh->setRadius(hollowShellRadius);

        qDebug() << "getting slider signal: current radius is " << value;
    }
}

void GLModel::getLayerViewSliderSignal(double value) {
    if (!shadowModel || !shadowModel->layerViewActive)
        return;

    float height = (_mesh->z_max() - _mesh->z_min() + scfg->raft_thickness) * value;
    int layer_num = int(height/scfg->layer_height)+1;
    if (value <= 0.002f)
        layer_num = 0;

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
    layerViewPlaneTransform[0]->setTranslation(QVector3D(0,0,layer_num*scfg->layer_height));

    // change phong material of original model
    float h = (_mesh->z_max() - _mesh->z_min() + scfg->raft_thickness) * value + _mesh->z_min();
    m_layerMaterialHeight->setValue(QVariant::fromValue(h));

    m_layerMaterialRaftHeight->setValue(QVariant::fromValue(qmlManager->getLayerViewFlags() & LAYER_INFILL != 0 ?
                _mesh->z_min() :
                _mesh->z_max()));
}

/** HELPER functions **/
QVector2D GLModel::world2Screen(QVector3D target){
    QVariant value;
    qRegisterMetaType<QVariant>("QVariant");
    QMetaObject::invokeMethod(qmlManager->mttab, "world2Screen", Qt::DirectConnection, Q_RETURN_ARG(QVariant,value),
                              Q_ARG(QVariant, target));

    QVector2D result = qvariant_cast<QVector2D>(value);

    return result;
}
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

Mesh* GLModel::toSparse(Mesh* mesh){
    int i=0;
    int jump = (mesh->getFaces()->size()<100000) ? 1:mesh->getFaces()->size()/30000; // 30000 is chosen for 800000 mesh, 30
    int factor = (jump ==1) ? 1:3*log(jump);

    Mesh* newMesh = new Mesh;

    if (jump == 1){ // if mesh face count is less than 30000

        for (const MeshFace& mf : *mesh->getFaces()){
            newMesh->addFace(mf.mesh_vertex[0]->position, mf.mesh_vertex[1]->position, mf.mesh_vertex[2]->position, &mf);
        }
    } else {

		for (const MeshFace& mf : *mesh->getFaces()) {
			if (i%jump==0){
                QCoreApplication::processEvents();

                QVector3D point1 =mf.mesh_vertex[0]->position;
                QVector3D point2 =mf.mesh_vertex[1]->position;
                QVector3D point3 =mf.mesh_vertex[2]->position;
                QVector3D CenterOfMass = (point1+point2+point3)/3;
                point1=GLModel::spreadPoint(point1,CenterOfMass,factor);
                point2=GLModel::spreadPoint(point2,CenterOfMass,factor);
                point3=GLModel::spreadPoint(point3,CenterOfMass,factor);
                //newMesh->addFace(mf.mesh_vertex[0]->position, mf.mesh_vertex[1]->position, mf.mesh_vertex[2]->position);
                newMesh->addFace(point1,point2,point3, &mf);
            }
            i+=1;
        }
    }

    return newMesh;
}

void GLModel::getTextChanged(QString text, int contentWidth)
{
    if (labellingTextPreview && labellingTextPreview->isEnabled()){
        applyLabelInfo(text, contentWidth, labellingTextPreview->fontName, (labellingTextPreview->fontWeight==QFont::Bold)? true:false, labellingTextPreview->fontSize);
    }
}

void GLModel::openLabelling()
{
    labellingActive = true;

    qmlManager->lastModelSelected();
    if ((qmlManager->selectedModels[0] != nullptr) && (qmlManager->selectedModels[0] != this)
            && (qmlManager->selectedModels[0]->shadowModel != this)) {
        labellingActive = false;
       /* if (labellingTextPreview) {
            qDebug() << "pinpin ^^^^^^^^^^^^^^^^^ 2";
            labellingTextPreview->planeSelected = false;
            labellingTextPreview->deleteLater();
            labellingTextPreview = nullptr;
        }*/
    }

}

void GLModel::closeLabelling()
{
    qDebug() << "close labelling ******************";
    labellingActive = false;
    /*
    if (labellingTextPreview){
        qDebug() << "pinpin ^^^^^^^^^^^^^^^^^ 3";
        labellingTextPreview->planeSelected = false;
        labellingTextPreview->deleteLater();
        labellingTextPreview = nullptr;
    }*/
    parentModel->targetMeshFace = nullptr;

//    stateChangeLabelling();
}

void GLModel::stateChangeLabelling() {
    qmlManager->keyboardHandlerFocus();
    (qmlManager->keyboardHandler)->setFocus(true);
}

void GLModel::getFontNameChanged(QString fontName)
{
    if (labellingTextPreview && labellingTextPreview->isEnabled()){
        applyLabelInfo(labellingTextPreview->text, labellingTextPreview->contentWidth, fontName, (labellingTextPreview->fontWeight==QFont::Bold)? true:false, labellingTextPreview->fontSize);
    }
}

void GLModel::getFontBoldChanged(bool isbold){
    if (labellingTextPreview && labellingTextPreview->isEnabled()){
        applyLabelInfo(labellingTextPreview->text, labellingTextPreview->contentWidth, labellingTextPreview->fontName, isbold, labellingTextPreview->fontSize);

    }
}

void GLModel::getFontSizeChanged(int fontSize)
{
    if (labellingTextPreview && labellingTextPreview->isEnabled()){
        applyLabelInfo(labellingTextPreview->text, labellingTextPreview->contentWidth, labellingTextPreview->fontName, (labellingTextPreview->fontWeight==QFont::Bold)? true:false, fontSize);
    }
}

/* make a new labellingTextPreview and apply label info's */
void GLModel::applyLabelInfo(QString text, int contentWidth, QString fontName, bool isBold, int fontSize){
    QVector3D translation;
    bool selected = false;

    //qDebug() << "applyLabelInfo +++++++++++++++++++++++++ "<<text<<contentWidth<<fontName<<isBold<<fontSize << labellingTextPreview->isEnabled();

    if (labellingTextPreview && labellingTextPreview->isEnabled()){
        translation = labellingTextPreview->translation;
        selected = labellingTextPreview->planeSelected;
        labellingTextPreview->deleteLater();
        labellingTextPreview = nullptr;
    }
    labellingTextPreview = new LabellingTextPreview(this);
    labellingTextPreview->setEnabled(true);

    labellingTextPreview->setFontName(fontName);
    labellingTextPreview->setFontSize(fontSize);
    labellingTextPreview->setFontBold(isBold);
    labellingTextPreview->setText(text, contentWidth);
    labellingTextPreview->planeSelected = selected;

    if (parentModel->targetMeshFace != nullptr) {
        labellingTextPreview->setTranslation(translation);
        labellingTextPreview->setNormal(parentModel->targetMeshFace->fn);
    }
   /*
    if (labellingTextPreview && labellingTextPreview->isEnabled() && parentModel->targetMeshFace !=nullptr) {
        labellingTextPreview->setTranslation(translation);
        labellingTextPreview->setNormal(parentModel->targetMeshFace->fn);
        if (text != ""){
            //labellingTextPreview->setText(label_text, label_text.size());
            labellingTextPreview->setFontName(fontName);
            labellingTextPreview->setFontBold(isBold);
            labellingTextPreview->setFontSize(fontSize);
            labellingTextPreview->setText(text, contentWidth);
        }
        labellingTextPreview->planeSelected = selected;
    }
    */

}


void GLModel::generateText3DMesh()
{
    qDebug() << "generateText3DMesh +++++++++++++++++++++++++";
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

    labellingTextPreview->planeSelected = false;

    parentModel->saveUndoState();

    //qDebug() <<m_transform.translation();
    //qDebug() << labellingTextPreview->translation;

    QVector3D* originalVertices = reinterpret_cast<QVector3D*>(vertexBuffer.data().data());
    int originalVerticesSize = vertexBuffer.data().size() / sizeof(float) / 3;

    QVector3D* vertices;
    int verticesSize;
    unsigned int* indices;
    int indicesSize;
    float depth = 0.5f;
    float scale = labellingTextPreview->ratioY * labellingTextPreview->scaleY;
    QVector3D translation = m_transform.translation()+labellingTextPreview->translation + QVector3D(0,-0.3,0);

    Qt3DCore::QTransform transform, normalTransform;

    QVector3D normal = labellingTextPreview->normal;

    QVector3D ref = QVector3D(0, 0, 1);
    auto tangent = QVector3D::crossProduct(normal, ref);

    tangent.normalize();
    auto binormal = QVector3D::crossProduct(tangent, normal);
    binormal.normalize();

    QQuaternion quat = QQuaternion::fromAxes(tangent, normal, binormal) * QQuaternion::fromAxisAndAngle(QVector3D(0, 0, 1), 180)* QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), 90);

    transform.setScale(scale);
    transform.setRotation(quat);
    transform.setTranslation(translation);

    generateText3DGeometry(&vertices, &verticesSize,
                           &indices, &indicesSize,
                           QFont(labellingTextPreview->fontName, labellingTextPreview->fontSize, labellingTextPreview->fontWeight, false),
                           labellingTextPreview->text,
                           depth,
                           parentModel->_mesh,
                           -labellingTextPreview->normal,
                           transform.matrix(),
                           normalTransform.matrix());

    std::vector<QVector3D> outVertices;
    std::vector<QVector3D> outNormals;
    for (int i = 0; i < indicesSize / 3; ++i) {
        // Insert vertices in CCW order
        outVertices.push_back(vertices[2 * indices[3*i + 2] + 0]);
        outVertices.push_back(vertices[2 * indices[3*i + 1] + 0]);
        outVertices.push_back(vertices[2 * indices[3*i + 0] + 0]);
        parentModel->_mesh->addFace(vertices[2 * indices[3*i + 2] + 0], vertices[2 * indices[3*i + 1] + 0], vertices[2 * indices[3*i + 0] + 0]);
        //qDebug() << vertices[2 * indices[3*i + 2] + 0]<< vertices[2 * indices[3*i + 1] + 0]<< vertices[2 * indices[3*i + 0] + 0];
        outNormals.push_back(vertices[2 * indices[3*i + 2] + 1]);
        outNormals.push_back(vertices[2 * indices[3*i + 1] + 1]);
        outNormals.push_back(vertices[2 * indices[3*i + 0] + 1]);
    }

    parentModel->_mesh->connectFaces();
    if (GLModel* glmodel = qobject_cast<GLModel*>(parent())) {
        glmodel->appendVertices(outVertices);
        glmodel->appendNormalVertices(outNormals);

        emit glmodel->_updateModelMesh(true);
    }


}

// for extension

void GLModel::colorExtensionFaces(){
    removeComponent(m_meshMaterial);
    addComponent(m_meshVertexMaterial);
}

void GLModel:: uncolorExtensionFaces(){
    resetColorMesh(_mesh, &vertexColorBuffer, extendFaces);
    removeComponent(m_meshVertexMaterial);
    addComponent(m_meshMaterial);
}
void GLModel::generateColorAttributes(){
    extendColorMesh(_mesh,targetMeshFace,&vertexColorBuffer,&extendFaces);
}
void GLModel::generateExtensionFaces(double distance){
    if (targetMeshFace == NULL)
        return;

    saveUndoState();
    extendMesh(_mesh, targetMeshFace, distance);
    targetMeshFace = NULL;
    emit _updateModelMesh(true);
}

void GLModel::generateLayFlat(){
    //MeshFace shadow_meshface = *targetMeshFace;

    if(targetMeshFace == NULL)
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
    if (targetMeshFace == NULL)
        return;
    QVector3D t = m_transform.translation();
    t.setZ(_mesh->z_min()+scfg->raft_thickness);
    QVector3D targetPosition = targetMeshFace->mesh_vertex[0]->position- t;
    /*OverhangPoint* targetOverhangPosition = new OverhangPoint(targetPosition.x()*scfg->resolution,
                                                              targetPosition.y()*scfg->resolution,
                                                              targetPosition.z()*scfg->resolution,
                                                              scfg->default_support_radius);

    generateSupporter(layerSupportMesh, targetOverhangPosition, nullptr, nullptr, layerSupportMesh->z_min());*/
    targetMeshFace = NULL;
    emit _updateModelMesh(true);
}

// for shell offset
void GLModel::generateShellOffset(double factor){
    //saveUndoState();
    qDebug() << "generate shell Offset";
    qmlManager->openProgressPopUp();
    QString original_filename = filename;

    cutFillMode = 1;
    shellOffsetFactor = factor;

    shadowModel->modelCut();

}


void GLModel::openLayflat(){
    //qDebug() << "open layflat called" << this << this->shadowModel;
    layflatActive = true;

    qmlManager->lastModelSelected();
    if ((qmlManager->selectedModels[0] != nullptr) && (qmlManager->selectedModels[0] != this)
            && (qmlManager->selectedModels[0]->shadowModel != this))
        layflatActive = false;
    //qDebug() << "open layflat called end  " << layflatActive;

    /*
    QApplication::setOverrideCursor(QCursor(Qt::UpArrowCursor));
    shadowModel->//m_objectPicker->setEnabled(false);
    //m_objectPicker = new Qt3DRender::QObjectPicker(this);
    QObject::connect(//m_objectPicker,SIGNAL(clicked(Qt3DRender::QPickEvent*)), this, SLOT(mouseClickedLayflat(Qt3DRender::QPickEvent*)));
    this->addComponent(//m_objectPicker);
    this->shadowModel->layflatActive = true;
    */
}

void GLModel::closeLayflat(){
    layflatActive = false;
    parentModel->uncolorExtensionFaces();
    parentModel->targetMeshFace = nullptr;
}
void GLModel::openExtension(){
    extensionActive = true;

    qmlManager->lastModelSelected();
    if ((qmlManager->selectedModels[0] != nullptr) && (qmlManager->selectedModels[0] != this)
            && (qmlManager->selectedModels[0]->shadowModel != this))
        extensionActive = false;

}

void GLModel::closeExtension(){
    extensionActive = false;
    parentModel->uncolorExtensionFaces();
    parentModel->targetMeshFace = nullptr;
}

void GLModel::openManualSupport(){
    manualSupportActive = true;
    qDebug() << "open manual support";
}

void GLModel::closeManualSupport(){
    manualSupportActive = false;
    parentModel->uncolorExtensionFaces();
    parentModel->targetMeshFace = nullptr;
    qDebug() << "close manual support";
}

void GLModel::openScale(){
    scaleActive = true;
    qmlManager->sendUpdateModelInfo();
    //QMetaObject::invokeMethod(qmlManager->scalePopup, "updateSizeInfo", Q_ARG(QVariant, parentModel->_mesh->x_max()-parentModel->_mesh->x_min()), Q_ARG(QVariant, parentModel->_mesh->y_max()-parentModel->_mesh->y_min()), Q_ARG(QVariant, parentModel->_mesh->z_max()-parentModel->_mesh->z_min()));
}

void GLModel::closeScale(){
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
    cutActive = false;
    removePlane();
    parentModel->removeCuttingPoints();
    parentModel->removeCuttingContour();
}

void GLModel::openHollowShell(){
    qDebug() << "open HollowShell called";
    hollowShellActive = true;
    qmlManager->hollowShellSphereEntity->setProperty("visible", true);
}

void GLModel::closeHollowShell(){
    qDebug() << "close HollowShell called";
    hollowShellActive = false;
    qmlManager->hollowShellSphereEntity->setProperty("visible", false);
}

void GLModel::openShellOffset(){
    qDebug() << "openShelloffset";
    shellOffsetActive = true;
    parentModel->addCuttingPoint(QVector3D(1,0,0));
    parentModel->addCuttingPoint(QVector3D(1,1,0));
    parentModel->addCuttingPoint(QVector3D(2,0,0));

    generatePlane();
    //m_objectPicker->setHoverEnabled(false); // to reduce drag load
    //m_objectPicker->setDragEnabled(false);
}

void GLModel::closeShellOffset(){
    qDebug() << "closeShelloffset";
    shellOffsetActive = false;
    removePlane();
    parentModel->removeCuttingPoints();
    parentModel->removeCuttingContour();
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
        if (shadowModel->layerViewActive){
            // remove layer view components
            removeLayerViewComponents();
        }
        shadowModel->layerViewActive = false;
        shadowModel->supportViewActive = false;
        addComponent(m_meshMaterial);
        removeComponent(m_layerMaterial);
        break;
    case VIEW_MODE_SUPPORT:
        if (shadowModel->layerViewActive){
            // remove layer view components
            removeLayerViewComponents();
        }
        shadowModel->layerViewActive = false;
        shadowModel->supportViewActive = true;
        addComponent(m_meshMaterial);
        removeComponent(m_layerMaterial);

        break;
    case VIEW_MODE_LAYER:
        shadowModel->layerViewActive = true;
        shadowModel->supportViewActive = false;
        addComponent(m_layerMaterial);

        // generate layer view plane materials
        layerViewPlaneMaterial = new Qt3DExtras::QTextureMaterial();
        layerViewPlaneMaterial->setAlphaBlendingEnabled(false);
        layerViewPlaneEntity[0] = new Qt3DCore::QEntity(parentModel);
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

    emit _updateModelMesh(true);
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
    gl3Technique->addRenderPass(gl3Pass);

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

void GLModel::addShadowModel(Mesh* mesh)
{
    if (shadowModel)
    {
#ifdef _STRICT_GLMODEL
        throw std::runtime_error("Shadow model added twice ");
#endif
    }
    qDebug() << "shadowmodel added";
    //set ID of shadow model to -ID of the parent for debugging purposes
    int shadowModelID = -2;
    shadowModel = new GLModel(this->mainWindow, this, mesh, filename, true, shadowModelID);
    //delete and disconnect previous shadowModel
    qmlManager->connectShadow(shadowModel);

}



void GLModel::deleteShadowModel()
{
    if (shadowModel)
    {
        qDebug()<< "shadowmodel removed";
        qmlManager->disconnectShadow(this);
        delete shadowModel;
        shadowModel = nullptr;
    }
}

void GLModel::updateShadowModelImpl()
{
    _mesh = toSparse(parentModel->_mesh);
	m_transform.setTranslation(QVector3D((_mesh->x_max() + _mesh->x_min()) / 2, (_mesh->y_max() + _mesh->y_min()) / 2, (_mesh->z_max() + _mesh->z_min()) / 2));
	_mesh->centerMesh();
	clearMem();
	updateAllVertices(_mesh);

}
void GLModel::setSupport()
{
    GenerateSupport generatesupport;
    supportMesh = generatesupport.generateSupport(_mesh);
}

const Mesh* GLModel::getSupport()
{
    return supportMesh;
}
void GLModel::setHitTestable(bool isEnable)
{
	_hitEnabled = isEnable;
}

bool GLModel::isHitTestable()
{
	return _hitEnabled;
}

void GLModel::setBoundingBoxVisible(bool isEnabled)
{
	_boundingBox.setEnabled(isEnabled);
}

const Qt3DCore::QTransform* GLModel::getTransform() const
{
	return &m_transform;
}

void GLModel::setTranslation(const QVector3D& t)
{
	m_transform.setTranslation(t);
	updateBoundingBox();
}

void GLModel::setMatrix(const QMatrix4x4& matrix)
{
	m_transform.setMatrix(matrix);
	updateBoundingBox();
}

