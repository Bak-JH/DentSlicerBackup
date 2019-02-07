#include <QRenderSettings>
#include "glmodel.h"
#include <QString>
#include <QtMath>
#include <cfloat>

#include "qmlmanager.h"
#include "feature/text3dgeometrygenerator.h"
#include "feature/shelloffset.h"
#include "feature/supportview.h"
#include <QtConcurrent/QtConcurrent>
#include <QFuture>
#include <QFileDialog>
#include <iostream>
#include <QDir>

int GLModel::globalID = 0;


GLModel::GLModel(QObject* mainWindow, QNode *parent, Mesh* loadMesh, QString fname, bool isShadow)
    : QEntity(parent)
    , filename(fname)
    , mainWindow(mainWindow)
    , x(0.0f)
    , y(0.0f)
    , z(0.0f)
    , v_cnt(0)
    , f_cnt(0)
    , m_transform(new Qt3DCore::QTransform())
    //, m_objectPicker(new Qt3DRender::QObjectPicker())
    , parentModel((GLModel*)(parent))
    , cutMode(1)
    , layerMesh(nullptr)
    , layerInfillMesh(nullptr)
    , layerSupportMesh(nullptr)
    , layerRaftMesh(nullptr)
    , slicer(nullptr)
{
    connect(&futureWatcher, SIGNAL(finished()), this, SLOT(slicingDone()));
    qDebug() << "new model made _______________________________"<<this<< "parent:"<<this->parentModel<<"shadow:"<<this->shadowModel;

    // generates shadow model for object picking
    if (isShadow){

        lmesh = new Mesh();
        rmesh = new Mesh();

        mesh = toSparse(parentModel->mesh);
        m_transform->setTranslation(QVector3D((mesh->x_max+mesh->x_min)/2, (mesh->y_max+mesh->y_min)/2, (mesh->z_max+mesh->z_min)/2));
        mesh->centerMesh();

        initialize(mesh->faces.size());
        addVertices(mesh, false);
        applyGeometry();

        addComponent(m_transform);

        m_meshMaterial = new QPhongMaterial();
        /*m_meshAlphaMaterial = new QPhongAlphaMaterial();
        m_layerMaterial = new QPhongMaterial();
        m_meshMaterial->setAmbient(QColor(255,0,0));
        m_meshMaterial->setDiffuse(QColor(173,215,218));
        m_meshMaterial->setSpecular(QColor(182,237,246));
        m_meshMaterial->setShininess(0.0f);

        addComponent(m_meshMaterial);*/

        addMouseHandlers();

        QObject::connect(this,SIGNAL(_generateSupport()),this,SLOT(generateSupport()));
        QObject::connect(this,SIGNAL(_updateModelMesh(bool)),this,SLOT(updateModelMesh(bool)));

        labellingTextPreview = new LabellingTextPreview(this);
        /*
        labellingTextPreview = new LabellingTextPreview(this);
        labellingTextPreview->setEnabled(false);
        labellingTextPreview->setTranslation(QVector3D(100,0,0));
        */

        return;
    }

    // Add to Part List
    ID = globalID++;
    qmlManager->addPart(getFileName(fname.toStdString().c_str()), ID);

    m_meshMaterial = new QPhongMaterial();
    m_meshAlphaMaterial = new QPhongAlphaMaterial();
    m_meshVertexMaterial = new QPerVertexColorMaterial();

    generateLayerViewMaterial();

    this->changecolor(0);
    if (filename != "" && (filename.contains(".stl")||filename.contains(".STL"))\
            && loadMesh == nullptr){
        mesh = new Mesh();
        loadMeshSTL(mesh, filename.toLocal8Bit().constData());
    } else if (filename != "" && (filename.contains(".obj")||filename.contains(".OBJ"))\
            && loadMesh == nullptr){
        mesh = new Mesh();
        loadMeshOBJ(mesh, filename.toLocal8Bit().constData());
    } else {
        mesh = loadMesh;
    }

    // 승환 25%
    qmlManager->setProgress(0.23);

    lmesh = new Mesh();
    rmesh = new Mesh();

    initialize(mesh->faces.size());
    addVertices(mesh, false);
    applyGeometry();

    // 승환 50%
    qmlManager->setProgress(0.49);
    //QFuture<void> future = QtConcurrent::run(this, &GLModel::initialize, mesh);
    //future = QtConcurrent::run(this, &GLModel::addVertices, mesh);

    Qt3DExtras::QDiffuseMapMaterial *diffuseMapMaterial = new Qt3DExtras::QDiffuseMapMaterial();

    addComponent(m_transform);
    addComponent(m_meshMaterial);

    qDebug() << "created original model";

    //repairMesh(mesh);

    // create shadow model to handle picking settings
    shadowModel = new GLModel(this->mainWindow, this, mesh, filename, true);
    // 승환 75%
    qmlManager->setProgress(0.73);

    QObject::connect(this,SIGNAL(bisectDone()),this,SLOT(generateRLModel()));
    QObject::connect(this,SIGNAL(_generateSupport()),this,SLOT(generateSupport()));
    QObject::connect(this,SIGNAL(_updateModelMesh(bool)),this,SLOT(updateModelMesh(bool)));

    qDebug() << "created shadow model";

    ft = new featureThread(this, 0);
    //arsignal = new arrangeSignalSender();//unused, signal from qml goes right into QmlManager.runArrange


    /*QList<QObject*> temp;
    temp.append(mainWindow);
    QObject *partList = (QEntity *)FindItemByName(temp, "partList");

    QMetaObject::invokeMethod(partList, "addPart",
        Q_ARG(QVariant, getFileName(fname.toStdString().c_str())),
        Q_ARG(QVariant, ID));
    */

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

void GLModel::addMouseHandlers(){

    m_objectPicker = new Qt3DRender::QObjectPicker(this);

    m_objectPicker->setHoverEnabled(false); // to reduce drag load
    m_objectPicker->setDragEnabled(false);
    // add only m_objectPicker
    QObject::connect(m_objectPicker, SIGNAL(released(Qt3DRender::QPickEvent*)), this, SLOT(handlePickerClicked(Qt3DRender::QPickEvent*)));
    QObject::connect(m_objectPicker, SIGNAL(moved(Qt3DRender::QPickEvent*)), this, SLOT(mgoo(Qt3DRender::QPickEvent*)));
    QObject::connect(m_objectPicker, SIGNAL(pressed(Qt3DRender::QPickEvent*)), this, SLOT(pgoo(Qt3DRender::QPickEvent*)));
    QObject::connect(m_objectPicker, SIGNAL(entered()), this, SLOT(engoo()));
    QObject::connect(m_objectPicker, SIGNAL(exited()), this, SLOT(exgoo()));
    addComponent(m_objectPicker);
}

void GLModel::removeMouseHandlers(){
    if (m_objectPicker == nullptr)
        return;
    //m_objectPicker->setDragEnabled(true);
    // add only m_objectPicker
    QObject::disconnect(m_objectPicker, SIGNAL(released(Qt3DRender::QPickEvent*)), this, SLOT(handlePickerClicked(Qt3DRender::QPickEvent*)));
    QObject::disconnect(m_objectPicker, SIGNAL(moved(Qt3DRender::QPickEvent*)), this, SLOT(mgoo(Qt3DRender::QPickEvent*)));
    QObject::disconnect(m_objectPicker, SIGNAL(pressed(Qt3DRender::QPickEvent*)), this, SLOT(pgoo(Qt3DRender::QPickEvent*)));
    QObject::disconnect(m_objectPicker, SIGNAL(entered()), this, SLOT(engoo()));
    QObject::disconnect(m_objectPicker, SIGNAL(exited()), this, SLOT(exgoo()));
    removeComponent(m_objectPicker);
    m_objectPicker->deleteLater();
}

void GLModel::changecolor(int mode){
    if (mode == -1) mode = colorMode;
    switch(mode){
    case 0: // default
        m_meshMaterial->setAmbient(QColor(130,130,140));
        m_meshMaterial->setDiffuse(QColor(131,206,220));
        m_meshMaterial->setDiffuse(QColor(97,185,192));
        m_meshMaterial->setSpecular(QColor(0,0,0));
        m_meshMaterial->setShininess(0.0f);

        m_meshAlphaMaterial->setAmbient(QColor(130,130,140));;
        m_meshAlphaMaterial->setDiffuse(QColor(131,206,220));
        m_meshAlphaMaterial->setDiffuse(QColor(97,185,192));
        m_meshAlphaMaterial->setSpecular(QColor(0,0,0));
        m_meshAlphaMaterial->setShininess(0.0f);
        colorMode = 0;
        break;
    case 1:
        //m_meshMaterial->setAmbient(QColor(180,180,180));
        m_meshMaterial->setDiffuse(QColor(100,255,100));
        m_meshMaterial->setDiffuse(QColor(130,208,125));
        m_meshMaterial->setSpecular(QColor(0,0,0));
        m_meshMaterial->setShininess(0.0f);

        m_meshAlphaMaterial->setDiffuse(QColor(100,255,100));
        m_meshAlphaMaterial->setDiffuse(QColor(130,208,125));
        m_meshAlphaMaterial->setSpecular(QColor(0,0,0));
        m_meshAlphaMaterial->setShininess(0.0f);
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
    float printing_x = 100;
    float printing_y = 80;
    float printing_z = 120;
    float printing_safegap = 1;
    // is it inside the printing area or not?
    QVector3D tmp = m_transform->translation();
    if ((tmp.x() + mesh->x_min) < printing_safegap - printing_x/2 |
        (tmp.x() + mesh->x_max) > printing_x/2 - printing_safegap|
        (tmp.y() + mesh->y_min) <  printing_safegap - printing_y/2|
        (tmp.y() + mesh->y_max) > printing_y/2 - printing_safegap|
        (tmp.z() + mesh->z_max) > printing_z){
        this->changecolor(2);
    }else{
        this->changecolor(-1);
        this->changecolor(3);
    }
//    qDebug() << tmp << mesh->x_max << mesh->x_min << mesh->y_max << mesh->y_min << mesh->z_max;
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

    // need to remove redo State since it contains
    mesh->nextMesh = nullptr;

    // copy current Mesh as temporary prev_mesh
    Mesh* temp_prev_mesh = new Mesh();
    temp_prev_mesh->faces.reserve(mesh->faces.size()*3);
    temp_prev_mesh->vertices.reserve(mesh->faces.size()*3);

    // only need to copy faces, verticesHash, vertices
    foreach(MeshVertex mv, mesh->vertices){
        temp_prev_mesh->vertices.push_back(mv);
    }

    for (vector<MeshFace>::iterator it = mesh->faces.begin(); it!= mesh->faces.end(); ++it){
        //qDebug() << it->neighboring_faces.size();
        temp_prev_mesh->faces.push_back((*it));
        // clear and copy neighboring faces
        /*temp_prev_mesh->faces.end()->neighboring_faces[0].clear();
        temp_prev_mesh->faces.end()->neighboring_faces[1].clear();
        temp_prev_mesh->faces.end()->neighboring_faces[2].clear();*/
        /*temp_prev_mesh->faces.end()->neighboring_faces[0].insert(temp_prev_mesh->faces.end()->neighboring_faces[0].end(), mf.neighboring_faces[0].begin(), mf.neighboring_faces[0].end());
        temp_prev_mesh->faces.end()->neighboring_faces[1].insert(temp_prev_mesh->faces.end()->neighboring_faces[1].end(), mf.neighboring_faces[1].begin(), mf.neighboring_faces[1].end());
        temp_prev_mesh->faces.end()->neighboring_faces[2].insert(temp_prev_mesh->faces.end()->neighboring_faces[2].end(), mf.neighboring_faces[2].begin(), mf.neighboring_faces[2].end());*/
    }

    for (QHash<uint32_t, MeshVertex>::iterator it = mesh->vertices_hash.begin(); it!=mesh->vertices_hash.end(); ++it){
        temp_prev_mesh->vertices_hash.insert(it.key(), it.value());
    }

    temp_prev_mesh->x_max = mesh->x_max;
    temp_prev_mesh->x_min = mesh->x_min;
    temp_prev_mesh->y_max = mesh->y_max;
    temp_prev_mesh->y_min = mesh->y_min;
    temp_prev_mesh->z_max = mesh->z_max;
    temp_prev_mesh->z_min = mesh->z_min;

    // it takes too long
    /*foreach (MeshFace mf, mesh->faces){
        temp_prev_mesh->addFace(mesh->vertices[mf.mesh_vertex[0]].position,
                mesh->vertices[mf.mesh_vertex[1]].position,
                mesh->vertices[mf.mesh_vertex[2]].position);
    }*/
    //temp_prev_mesh->connectFaces();

    temp_prev_mesh->prevMesh = mesh->prevMesh;
    if (mesh->prevMesh != nullptr)
        mesh->prevMesh->nextMesh = temp_prev_mesh;
    temp_prev_mesh->nextMesh = mesh;
    temp_prev_mesh->time = QTime::currentTime();
    temp_prev_mesh->m_translation = m_transform->translation();
    temp_prev_mesh->m_matrix = m_transform->matrix();

    Mesh* deleteTargetMesh = mesh;

    int saveCnt = (mesh->faces.size()>100000)? 3: 10;

    for (int i=0; i<saveCnt; i++){ // maximal undo count is 10
        if (deleteTargetMesh != nullptr)
            deleteTargetMesh = deleteTargetMesh->prevMesh;
    }
    if (deleteTargetMesh != nullptr){
        deleteTargetMesh->nextMesh->prevMesh = nullptr;
        delete deleteTargetMesh;
    }

    mesh->prevMesh = temp_prev_mesh;


    // for model cut, shell offset
    lmesh->prevMesh = temp_prev_mesh;
    rmesh->prevMesh = temp_prev_mesh;
}

void GLModel::loadUndoState(){
    //while (updateLock); // if it is not locked
    if (updateLock)
        return;
    updateLock = true;
    qDebug() << this << "achieved lock";

    QMetaObject::invokeMethod(qmlManager->boxUpperTab, "all_off");

    if (mesh->prevMesh != nullptr){
        qDebug() << "load undo state";
        if (twinModel != NULL && mesh->prevMesh == twinModel->mesh->prevMesh){ // same parent, cut generated
            qmlManager->deleteModelFile(twinModel->ID);
        }
        if (mesh->time.isNull() || mesh->m_translation == QVector3D(0,0,0) || mesh->m_matrix.isIdentity()){ // most recent job
            mesh->time = QTime::currentTime();
            mesh->m_translation = m_transform->translation();
            mesh->m_matrix = m_transform->matrix();
        }

        mesh = mesh->prevMesh;

        // move model mesh and rotate model mesh
        /*mesh->vertexMove(mesh->m_translation);
        mesh->vertexRotate(mesh->m_matrix);*/
        if (!mesh->m_matrix.isIdentity()){
            mesh->vertexRotate(mesh->m_matrix.inverted());
            mesh->m_matrix = QMatrix4x4();
        }
        m_transform->setTranslation(mesh->m_translation);
        m_transform->setRotationX(0);
        m_transform->setRotationY(0);
        m_transform->setRotationZ(0);
        emit _updateModelMesh(true);
    } else {
        updateLock = false;
        qDebug() << "no undo state";
        int saveCnt = (mesh->faces.size()>100000)? 3: 10;
        qmlManager->openResultPopUp("Undo state doesn't exist.","","Maximum "+QVariant(saveCnt).toString().toStdString()+" states are saved for this model.");
    }
}

void GLModel::loadRedoState(){
    if (mesh->nextMesh != nullptr){
        qDebug() << "load Redo State";
        mesh = mesh->nextMesh;
        // move model mesh and rotate model mesh
        /*mesh->vertexMove(mesh->m_translation);
        mesh->vertexRotate(mesh->m_matrix);*/
        if (!mesh->m_matrix.isIdentity()){
            mesh->vertexRotate(mesh->m_matrix.inverted());
            mesh->m_matrix = QMatrix4x4();
        }


        m_transform->setTranslation(mesh->m_translation);
        m_transform->setRotationX(0);
        m_transform->setRotationY(0);
        m_transform->setRotationZ(0);
        emit _updateModelMesh(true);
    } else {
        qDebug() << "no redo status";
    }
}

void GLModel::moveModelMesh(QVector3D direction){
    mesh->vertexMove(direction);
    /*if (shadowModel != NULL)
        shadowModel->moveModelMesh(direction);*/

    qDebug() << "moved vertex";
    emit _updateModelMesh(true);
}
void GLModel::scaleModelMesh(float scaleX, float scaleY, float scaleZ){
    /* To fix center of the model */
    float centerX = (mesh->x_max + mesh->x_min)/2;
    float centerY = (mesh->y_max + mesh->y_min)/2;
    mesh->vertexScale(scaleX, scaleY, scaleZ, centerX, centerY);
    /*if (shadowModel != NULL)
        shadowModel->scaleModelMesh(scale);*/

    emit _updateModelMesh(true);
}

void GLModel::rotateModelMesh(int Axis, float Angle){
    Qt3DCore::QTransform* tmp = new Qt3DCore::QTransform();
    switch(Axis){
    case 1:{
        tmp->setRotationX(Angle);
        break;
    }
    case 2:{
        tmp->setRotationY(Angle);
        break;
    }
    case 3:{
        tmp->setRotationZ(Angle);
        break;
    }
    }
    rotateModelMesh(tmp->matrix());
}


void GLModel::rotateModelMesh(QMatrix4x4 matrix){
    mesh->vertexRotate(matrix);
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
    qDebug() << "update Model Mesh";
    // delete allocated buffers, geometry
    delete vertexBuffer;
    delete vertexColorBuffer;
    delete vertexNormalBuffer;
    delete positionAttribute;
    delete normalAttribute;
    delete colorAttribute;
    removeComponent(m_geometryRenderer);
    delete m_geometry;
    delete m_geometryRenderer;
    // reinitialize with updated mesh

    int viewMode = qmlManager->getViewMode();
    switch( viewMode ) {
    case VIEW_MODE_OBJECT:
        initialize(mesh->faces.size());
        addVertices(mesh, false);
        break;
    case VIEW_MODE_SUPPORT:
        if( layerMesh != nullptr ) {
            initialize(layerMesh->faces.size() + layerSupportMesh->faces.size() + layerRaftMesh->faces.size());
            addVertices(layerMesh, false);
            addVertices(layerSupportMesh, false);
            addVertices(layerRaftMesh, false);
        } else {
            initialize(mesh->faces.size());
            addVertices(mesh, false);
        }
        break;
    case VIEW_MODE_LAYER:
        if( layerMesh != nullptr ) {
            int faces = layerMesh->faces.size() +
                    (qmlManager->getLayerViewFlags() & LAYER_INFILL != 0 ? layerInfillMesh->faces.size() : 0) +
                    (qmlManager->getLayerViewFlags() & LAYER_SUPPORTERS != 0 ? layerSupportMesh->faces.size() : 0) +
                    (qmlManager->getLayerViewFlags() & LAYER_RAFT != 0 ? layerRaftMesh->faces.size() : 0);
            initialize(faces);
            addVertices(layerMesh, false);
            if( qmlManager->getLayerViewFlags() & LAYER_INFILL ) {
                addVertices(layerInfillMesh, false, QVector3D(1.0f, 1.0f, 0.0f));
            }
            if( qmlManager->getLayerViewFlags() & LAYER_SUPPORTERS ) {
                addVertices(layerSupportMesh, false);
            }
            if( qmlManager->getLayerViewFlags() & LAYER_RAFT) {
                addVertices(layerRaftMesh, false, QVector3D(0.0f, 0.0f, 0.0f));
            }
        } else {
            initialize(mesh->faces.size());
            addVertices(mesh, false);
        }
        break;
    }
    applyGeometry();

    QVector3D tmp = m_transform->translation();
    float zlength = mesh->z_max - mesh->z_min;
    //if (shadowModel != NULL) // since shadow model transformed twice

    m_transform->setTranslation(QVector3D(tmp.x(),tmp.y(),-mesh->z_min));
    //QMetaObject::invokeMethod(qmlManager->boundedBox, "setPosition", Q_ARG(QVariant, m_transform->translation()+QVector3D((mesh->x_max+mesh->x_min)/2,(mesh->y_max+mesh->y_min)/2,(mesh->z_max+mesh->z_min)/2)));
    //QMetaObject::invokeMethod(qmlManager->boundedBox, "setSize", Q_ARG(QVariant, mesh->x_max - mesh->x_min),
    //                                                 Q_ARG(QVariant, mesh->y_max - mesh->y_min),
    //                                                 Q_ARG(QVariant, mesh->z_max - mesh->z_min));
    qmlManager->sendUpdateModelInfo();

    checkPrintingArea();
    //QMetaObject::invokeMethod(qmlManager->scalePopup, "updateSizeInfo", Q_ARG(QVariant, mesh->x_max-mesh->x_min), Q_ARG(QVariant, mesh->y_max-mesh->y_min), Q_ARG(QVariant, mesh->z_max-mesh->z_min));
    qDebug() << "model transform :" <<m_transform->translation() << mesh->x_max << mesh->x_min << mesh->y_max << mesh->y_min << mesh->z_max << mesh->z_min;


    // create new object picker, shadowmodel, remove prev shadowmodel
    //QVector3D translation = shadowModel->m_transform->translation();
    if (shadowModel !=NULL && shadowUpdate){
        QObject::disconnect(shadowModel, SIGNAL(modelSelected(int)), qmlManager, SLOT(modelSelected(int)));
        shadowModel->removeMouseHandlers();
        qmlManager->disconnectHandlers(this);
        GLModel* prevShadowModel = shadowModel;
        switch( viewMode ) {
            case VIEW_MODE_OBJECT:
                shadowModel=new GLModel(this->mainWindow, this, mesh, filename, true);
                break;
            case VIEW_MODE_SUPPORT:
                shadowModel=new GLModel(this->mainWindow, this, layerMesh, filename, true);
                shadowModel->m_transform->setTranslation(shadowModel->m_transform->translation()+QVector3D(0,0,scfg->raft_thickness));
                break;
            case VIEW_MODE_LAYER:
                shadowModel=new GLModel(this->mainWindow, this, mesh, filename, true);
                break;
        }
        /*float mesh_x_center = m_transform->translation().x()+(mesh->x_max +mesh->x_min)/2;
        float mesh_y_center = m_transform->translation().y()+(mesh->y_max +mesh->y_min)/2;
        float mesh_z_center = m_transform->translation().z()+(mesh->z_max +mesh->z_min)/2;
        shadowModel=new GLModel(this->mainWindow, this, mesh->vertexMoved(-QVector3D(mesh_x_center,mesh_y_center,mesh_z_center)), filename, true);
        shadowModel->m_transform->setTranslation(QVector3D(mesh_x_center, mesh_y_center, 0));*/
        shadowModel->copyModelAttributeFrom(prevShadowModel);
        prevShadowModel->deleteLater();

        // reconnect handler if current selected model is updated
        if (qmlManager->selectedModels[0]==this)
            qmlManager->connectHandlers(this);
        //shadowModel->m_transform->setTranslation(translation);
        QObject::connect(shadowModel, SIGNAL(modelSelected(int)), qmlManager, SLOT(modelSelected(int)));
    }
    updateLock = false;
    qDebug() << this << "released lock";
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
                QFuture<void> future = QtConcurrent::run(ste, &STLexporter::exportSTL, m_glmodel->mesh,fileName);
                break;
                */
            }
        case ftrExport: // support view & layer view & export // on export, temporary variable = true;
            {
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
                    //ste->exportSTL(m_glmodel->mesh, fileName);
                    if(fileName == "")
                        return;
                } else { // support view & layerview
                    qDebug() << "export to temp file";
                    fileName =  QDir::tempPath();
                    qDebug() << fileName;
                }

                // slice file
                qmlManager->openProgressPopUp();
                QFuture<Slicer*> future = QtConcurrent::run(se, &SlicingEngine::slice, data, m_glmodel->mesh, fileName + "/" + m_glmodel->filename.split("/").last() );
                m_glmodel->futureWatcher.setFuture(future);
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
                rotateResult* rotateres= ot->Tweak(m_glmodel->mesh,true,45,&m_glmodel->appropriately_rotated);
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
                repairMesh(m_glmodel->mesh);

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
                shellOffset(m_glmodel, -0.5);
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

void GLModel::initialize(const int& faces_cnt){
    m_geometryRenderer = new QGeometryRenderer();
    m_geometry = new QGeometry(m_geometryRenderer);

    QByteArray vertexArray;
    vertexArray.resize(faces_cnt*3*(3)*sizeof(float));
    vertexBuffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer,m_geometry);
    vertexBuffer->setUsage(Qt3DRender::QBuffer::DynamicDraw);
    vertexBuffer->setData(vertexArray);

    QByteArray vertexNormalArray;
    vertexNormalArray.resize(faces_cnt*3*(3)*sizeof(float));
    vertexNormalBuffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer,m_geometry);
    vertexNormalBuffer->setUsage(Qt3DRender::QBuffer::DynamicDraw);
    vertexNormalBuffer->setData(vertexNormalArray);

    QByteArray vertexColorArray;
    vertexColorArray.resize(faces_cnt*3*(3)*sizeof(float));
    vertexColorBuffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer,m_geometry);
    vertexColorBuffer->setUsage(Qt3DRender::QBuffer::DynamicDraw);
    vertexColorBuffer->setData(vertexColorArray);

    // vertex Attributes
    positionAttribute = new QAttribute();
    positionAttribute->setAttributeType(QAttribute::VertexAttribute);
    positionAttribute->setBuffer(vertexBuffer);
    positionAttribute->setDataType(QAttribute::Float);
    positionAttribute->setDataSize(3);
    positionAttribute->setByteOffset(0);
    positionAttribute->setByteStride(3*sizeof(float));
    positionAttribute->setCount(0);
    positionAttribute->setName(QAttribute::defaultPositionAttributeName());

    // normal Attributes
    normalAttribute = new QAttribute();
    normalAttribute->setAttributeType(QAttribute::VertexAttribute);
    normalAttribute->setBuffer(vertexNormalBuffer);
    normalAttribute->setDataType(QAttribute::Float);
    normalAttribute->setDataSize(3);
    normalAttribute->setByteOffset(0);
    normalAttribute->setByteStride(3*sizeof(float));
    normalAttribute->setCount(0);
    normalAttribute->setName(QAttribute::defaultNormalAttributeName());

    // color Attributes
    colorAttribute = new QAttribute();
    colorAttribute->setAttributeType(QAttribute::VertexAttribute);
    colorAttribute->setBuffer(vertexColorBuffer);
    colorAttribute->setDataType(QAttribute::Float);
    colorAttribute->setDataSize(3);
    colorAttribute->setByteOffset(0);
    colorAttribute->setByteStride(3 * sizeof(float));
    colorAttribute->setCount(0);
    colorAttribute->setName(QAttribute::defaultColorAttributeName());

    return;
}

void GLModel::applyGeometry(){

    m_geometry->addAttribute(positionAttribute);
    m_geometry->addAttribute(normalAttribute);
    m_geometry->addAttribute(colorAttribute);

    m_geometryRenderer->setInstanceCount(1);
    m_geometryRenderer->setFirstVertex(0);
    m_geometryRenderer->setFirstInstance(0);
    m_geometryRenderer->setPrimitiveType(QGeometryRenderer::Triangles);
    m_geometryRenderer->setGeometry(m_geometry);

    addComponent(m_geometryRenderer);

    return;
}

void GLModel::addVertex(QVector3D vertex){

    //update geometry
    QByteArray appendVertexArray;
    appendVertexArray.resize(1*3*sizeof(float));
    float* reVertexArray = reinterpret_cast<float*>(appendVertexArray.data());

    //coordinates of left vertex
    reVertexArray[0] = x;
    reVertexArray[1] = y;
    reVertexArray[2] = z;

    uint vertexCount = positionAttribute->count();
    vertexBuffer->updateData(vertexCount*6*sizeof(float),appendVertexArray);
    positionAttribute->setCount(vertexCount+1);

    return;
}

void GLModel::clearVertices(){
    //uint vertexCount = positionAttribute->count();
    //vertexBuffer->
    //positionAttribute->setCount(vertexCount+vertexCount);
    return;
}

void GLModel::addVertices(Mesh* mesh, bool CW, QVector3D vertexColor)
{
    int face_size = mesh->faces.size();
    int face_idx = 0;
    /*foreach (MeshFace mf , mesh->faces){

        vector<int> result_vs;
        for (int fn=2; fn>=0; fn--){
            result_vs.push_back(mf.mesh_vertex[fn]);
        }
        addIndexes(result_vs);
    }*/
    foreach (MeshFace mf , mesh->faces){
        face_idx ++;
        if (face_idx %100 ==0)
            QCoreApplication::processEvents();

        vector<QVector3D> result_vs;
        if (CW){
            for (int fn=2; fn>=0; fn--){
                result_vs.push_back(mesh->idx2MV(mf.mesh_vertex[fn]).position);
            }
        } else {
            for (int fn=0; fn<=2; fn++){
                result_vs.push_back(mesh->idx2MV(mf.mesh_vertex[fn]).position);
            }
        }


        addVertices(result_vs);
    }

    foreach (MeshFace mf , mesh->faces){
        face_idx ++;
        if (face_idx %100 ==0)
            QCoreApplication::processEvents();

        vector<QVector3D> result_vns;
        if (CW){
            for (int fn=2; fn>=0; fn--){
                result_vns.push_back(mesh->idx2MV(mf.mesh_vertex[fn]).vn);

                if(mesh->idx2MV(mf.mesh_vertex[fn]).vn[0] == 0 &&\
                        mesh->idx2MV(mf.mesh_vertex[fn]).vn[1] == 0 &&\
                        mesh->idx2MV(mf.mesh_vertex[fn]).vn[2] == 0)
                        {
                            result_vns.pop_back();
                            result_vns.push_back(QVector3D(1,1,1));
                        }
            }
        } else {
            for (int fn=0; fn<=2; fn++){
                result_vns.push_back(mesh->idx2MV(mf.mesh_vertex[fn]).vn);

                if(mesh->idx2MV(mf.mesh_vertex[fn]).vn[0] == 0 &&\
                        mesh->idx2MV(mf.mesh_vertex[fn]).vn[1] == 0 &&\
                        mesh->idx2MV(mf.mesh_vertex[fn]).vn[2] == 0)
                        {
                            result_vns.pop_back();
                            result_vns.push_back(QVector3D(1,1,1));
                        }
            }
        }

        addNormalVertices(result_vns);
    }

    foreach (MeshFace mf , mesh->faces){
        face_idx ++;
        if (face_idx %100 ==0)
            QCoreApplication::processEvents();

        vector<QVector3D> result_vcs;
        if (CW){
            for (int fn=2; fn>=0; fn--){
                result_vcs.push_back(vertexColor);
            }
        } else {
            for (int fn=0; fn<=2; fn++){
                result_vcs.push_back(vertexColor);
            }
        }
        addColorVertices(result_vcs);
    }
}

void GLModel::addVertices(vector<QVector3D> vertices){

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

    uint currentVertexCount = positionAttribute->count();
    //qDebug() << "position Attribute size : " << (int)currentVertexCount;
    int currentVertexArraySize = currentVertexCount*3*sizeof(float);
    int appendVertexArraySize = appendVertexArray.size();

    if ((currentVertexArraySize  + appendVertexArraySize) > vertexBuffer->data().size()) {
        auto data = vertexBuffer->data();
        int currentVertexArraySize = data.size();

        int countPowerOf2 = 1;
        while (countPowerOf2 < currentVertexArraySize + appendVertexArraySize) {
            countPowerOf2 <<= 1;
        }

        data.resize(countPowerOf2);
        vertexBuffer->setData(data);
    }

    vertexBuffer->updateData(currentVertexArraySize, appendVertexArray);
    positionAttribute->setCount(currentVertexCount + appendVertexCount);
}


void GLModel::addNormalVertices(vector<QVector3D> vertices){
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

    uint vertexNormalCount = normalAttribute->count();

    int offset = vertexNormalCount*3*sizeof(float);
    int bytesSize = appendVertexArray.size();

    if ((offset + bytesSize) > vertexNormalBuffer->data().size()) {
        auto data = vertexNormalBuffer->data();

        int countPowerOf2 = 1;
        while (countPowerOf2 < data.size() + bytesSize) {
            countPowerOf2 <<= 1;
        }

        data.resize(countPowerOf2);

        vertexNormalBuffer->setData(data);
    }

    vertexNormalBuffer->updateData(offset, appendVertexArray);
    normalAttribute->setCount(vertexNormalCount+ vertex_normal_cnt);
    return;
}

void GLModel::addColorVertices(vector<QVector3D> vertices){
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

    uint vertexColorCount = colorAttribute->count();

    int offset = vertexColorCount*3*sizeof(float);
    int bytesSize = appendVertexArray.size();

    if ((offset + bytesSize) > vertexColorBuffer->data().size()) {
        auto data = vertexColorBuffer->data();

        int countPowerOf2 = 1;
        while (countPowerOf2 < data.size() + bytesSize) {
            countPowerOf2 <<= 1;
        }

        data.resize(countPowerOf2);

        vertexColorBuffer->setData(data);
    }

    vertexColorBuffer->updateData(vertexColorCount*3*sizeof(float), appendVertexArray);
    colorAttribute->setCount(vertexColorCount + vertex_color_cnt);
    return;
}


void GLModel::addIndexes(vector<int> indexes){
    int idx_cnt = indexes.size();
    QByteArray* appendIdxArray = new QByteArray(reinterpret_cast<char*>(indexes.data()), indexes.size());
    uint indexCount = indexAttribute->count();
    indexBuffer->updateData(indexCount*sizeof(int), *appendIdxArray);
    indexAttribute->setCount(indexCount + idx_cnt);
    return;
}

void GLModel::handlePickerEnteredFreeCutSphere()
{
    QCursor cursorEraser = QCursor(QPixmap(":/Resource/cursor_eraser.png"));
    QApplication::setOverrideCursor(cursorEraser);

}

void GLModel::handlePickerExitedFreeCutSphere()
{
    qmlManager->resetCursor();
}
void GLModel::handlePickerClickedFreeCutSphere(Qt3DRender::QPickEvent* pick)
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

    if (cuttingPoints.size() >= 2)
        QMetaObject::invokeMethod(qmlManager->cutPopup, "colorApplyFinishButton", Q_ARG(QVariant, 2));
    else
        QMetaObject::invokeMethod(qmlManager->cutPopup, "colorApplyFinishButton", Q_ARG(QVariant, 0));
}

void GLModel::handlePickerClickedFreeCut(Qt3DRender::QPickEvent* pick)
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


    if (parentModel->cuttingPoints.size() >= 2)
        QMetaObject::invokeMethod(qmlManager->cutPopup, "colorApplyFinishButton", Q_ARG(QVariant, 2));
    else
        QMetaObject::invokeMethod(qmlManager->cutPopup, "colorApplyFinishButton", Q_ARG(QVariant, 0));


}

void GLModel::handlePickerClicked(QPickEvent *pick)
{
    qDebug() << "handle Picker clicked" << pick->buttons() << pick->button();

    /*if (pick->button() == Qt::RightButton)
        return;*/

    if (!parentModel)
        return;

    //---------------- rgoo routine init --------------------
    m_objectPicker->setDragEnabled(false);
    m_transform->setScale(1.0f);
    qDebug() << "setting scale back to 1.0";
    qmlManager->resetCursor();

    isReleased = true;
    qDebug() << "Released";

    if (isMoved){
        qmlManager->modelMoveDone(3);
        isMoved = false;
        return;
    }

    //---------------- rgoo routine end --------------------

    if (qmlManager->yesno_popup->property("isFlawOpen").toBool())
        return;

    if (qmlManager->selectedModels[0] != nullptr && (pick->button() & Qt::RightButton)){ // when right button clicked
        if (qmlManager->selectedModels[0]->ID == parentModel->ID){
            QMetaObject::invokeMethod(qmlManager->mttab, "tabOnOff");
            m_objectPicker->setEnabled(false);
            return;
        }

    }



    if (!cutActive && !extensionActive && !labellingActive && !layflatActive && !manualSupportActive && !isMoved)// && !layerViewActive && !supportViewActive)
        emit modelSelected(parentModel->ID);

    qDebug() << "model selected emit" << pick->position() << parentModel->ID;

    if(pick->button() & Qt::RightButton){
        QMetaObject::invokeMethod(qmlManager->mttab, "tabOnOff");
    }

    QPickTriangleEvent *trianglePick = static_cast<QPickTriangleEvent*>(pick);
    qDebug() << "trianglePick : " << trianglePick << qmlManager->selectedModels.size() << qmlManager;

    if (labellingActive && trianglePick && trianglePick->localIntersection() != QVector3D(0,0,0)) {
        MeshFace shadow_meshface = mesh->faces[trianglePick->triangleIndex()];

        //parentModel->uncolorExtensionFaces();
        parentModel->targetMeshFace = &parentModel->mesh->faces[shadow_meshface.parent_idx];
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
                parentModel->addCuttingPoint(v+m_transform->translation());
                qDebug() << "Check 1 " << parentModel->cuttingPoints.size();
            }

            /*if (v.distanceToPoint(parentModel->cuttingPoints[parentModel->cuttingPoints.size()-1]) < 0.5f){
                qDebug() << "removing cutting point";
                //parentModel->removeCuttingPoint();
            } else {
                parentModel->addCuttingPoint(v);
            }*/

            // remove cutting contour and redraw cutting contour
            parentModel->removeCuttingContour();
            if (parentModel->cuttingPoints.size() >=2){
                qDebug() << "TTTTTTTTTTTTTTTTt";
                parentModel->generateCuttingContour(parentModel->cuttingPoints);
                parentModel->regenerateCuttingPoint(parentModel->cuttingPoints);
            }

            //generatePlane();
            //parentModel->ft->ct->addCuttingPoint(parentModel, v);
        } else if (cutMode == 9999){
            qDebug() << "current cut mode :" << cutMode;
            //return;
        }
    }

    if (extensionActive && trianglePick  && trianglePick->localIntersection() != QVector3D(0,0,0)){
        MeshFace shadow_meshface = mesh->faces[trianglePick->triangleIndex()];
        qDebug() << "found parent meshface" << shadow_meshface.parent_idx;
        parentModel->uncolorExtensionFaces();
        emit extensionSelect();
        parentModel->targetMeshFace = &parentModel->mesh->faces[shadow_meshface.parent_idx];
        parentModel->generateColorAttributes();
        /*qDebug() << trianglePick->localIntersection() \
                 << parentModel->mesh->idx2MV(parentModel->targetMeshFace->mesh_vertex[0]).position\
                << parentModel->mesh->idx2MV(parentModel->targetMeshFace->mesh_vertex[1]).position\
                << parentModel->mesh->idx2MV(parentModel->targetMeshFace->mesh_vertex[2]).position;
        */
        // << parentModel->targetMeshFace->mesh_vertex[1] << parentModel->targetMeshFace->mesh_vertex[2];
        parentModel->colorExtensionFaces();
    }

    if (hollowShellActive){
        qDebug() << "getting handle picker clicked signal hollow shell active";
        MeshFace shadow_meshface = mesh->faces[trianglePick->triangleIndex()];
        qDebug() << "found parent meshface" << shadow_meshface.parent_idx;
        parentModel->targetMeshFace = &parentModel->mesh->faces[shadow_meshface.parent_idx];
        // translate hollowShellSphere to mouse position
        QVector3D v = pick->localIntersection();
        QVector3D tmp = m_transform->translation();
        float zlength = mesh->z_max - mesh->z_min;
        qmlManager->hollowShellSphereTransform->setTranslation(v + QVector3D(tmp.x(),tmp.y(),-mesh->z_min));

        //parentModel->indentHollowShell(10);
        // emit hollowShellSelect();
    }

    if (layflatActive && trianglePick && trianglePick->localIntersection() != QVector3D(0,0,0)){
        /*
        m_objectPicker->setEnabled(false);
        this->parentModel->m_objectPicker = new Qt3DRender::QObjectPicker(this->parentModel);
        QObject::connect(this->parentModel->m_objectPicker, SIGNAL(clicked(Qt3DRender::QPickEvent*)), this->parentModel, SLOT(handlePickerClickedLayflat(Qt3DRender::QPickEvent*)));
        this->parentModel->addComponent(this->parentModel->m_objectPicker);
        */

        MeshFace shadow_meshface = mesh->faces[trianglePick->triangleIndex()];
        qDebug() << "target 1 " <<shadow_meshface.fn ;

        qDebug() << "found parent meshface" << shadow_meshface.parent_idx;
        parentModel->uncolorExtensionFaces();
        emit layFlatSelect();
        parentModel->targetMeshFace = &parentModel->mesh->faces[shadow_meshface.parent_idx];
        parentModel->generateColorAttributes();
        parentModel->colorExtensionFaces();
    }

    if (manualSupportActive && trianglePick  && trianglePick->localIntersection() != QVector3D(0,0,0)){
        qDebug() << "manual support handle picker clicked";
        MeshFace shadow_meshface = mesh->faces[trianglePick->triangleIndex()];
        qDebug() << "found parent meshface" << shadow_meshface.parent_idx;
        parentModel->uncolorExtensionFaces();
        emit extensionSelect();
        parentModel->targetMeshFace = &parentModel->mesh->faces[shadow_meshface.parent_idx];
        parentModel->generateColorAttributes();
        parentModel->colorExtensionFaces();
    }
}

void GLModel::handlePickerClickedLayflat(MeshFace shadow_meshface){
    /*
    qDebug() << "layflat picker!";
    QPickTriangleEvent *trianglePick = static_cast<QPickTriangleEvent*>(pick);
    MeshFace shadow_meshface = mesh->faces[trianglePick->triangleIndex()];
    */
    uncolorExtensionFaces();
    QVector3D tmp_fn = shadow_meshface.fn;
    Qt3DCore::QTransform* tmp = new Qt3DCore::QTransform();
    float x= tmp_fn.x();
    float y= tmp_fn.y();
    float z=tmp_fn.z();
    float angleX = qAtan2(y,z)*180/M_PI;
    if (z>0) angleX+=180;
    float angleY = qAtan2(x,z)*180/M_PI;
    tmp->setRotationX(angleX);
    tmp->setRotationY(angleY);
    rotateModelMesh(tmp->matrix());
    QObject::disconnect(m_objectPicker,SIGNAL(released(Qt3DRender::QPickEvent*)), this, SLOT(handlePickerClickedLayflat(Qt3DRender::QPickEvent*)));
    delete m_objectPicker;
    shadowModel->m_objectPicker->setEnabled(true);
    //closeLayflat();
    emit resetLayflat();
}
void GLModel::bisectModel(Plane plane){
    QFuture<void> future = QtConcurrent::run(this, &bisectModel_internal, plane);
}

// need to create new mesh object liek Mesh* leftMesh = new Mesh();
void GLModel::bisectModel_internal(Plane plane){

    Mesh* leftMesh = lmesh;
    Mesh* rightMesh = rmesh;
    // do bisecting mesh
    leftMesh->faces.reserve(mesh->faces.size()*3);
    leftMesh->vertices.reserve(mesh->faces.size()*3);
    rightMesh->faces.reserve(mesh->faces.size()*3);
    rightMesh->vertices.reserve(mesh->faces.size()*3);

    Paths3D cuttingEdges;

    foreach (MeshFace mf, mesh->faces){
        bool faceLeftToPlane = false;
        bool faceRightToPlane = false;
        Plane target_plane;
        for (int vn=0; vn<3; vn++){
            MeshVertex mv = mesh->vertices[mf.mesh_vertex[vn]];
            target_plane.push_back(mv.position);
            if (isLeftToPlane(plane, mv.position)) // if one vertex is left to plane, append to left vertices part
                faceLeftToPlane = true;
            else {
                faceRightToPlane = true;
            }
        }
        if (faceLeftToPlane && faceRightToPlane){ // cutting edge
            Path3D intersection = mesh->intersectionPath(plane, target_plane);
            cuttingEdges.push_back(intersection);
            vector<QVector3D> upper;
            vector<QVector3D> lower;

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
                    rightMesh->addFace(upper[1], upper[0], intersection[1].position);
                    rightMesh->addFace(intersection[1].position, intersection[0].position, upper[1]);
                    leftMesh->addFace(lower[0], intersection[0].position, intersection[1].position);
                }
                if (!facingNormal || facingNormalAmbiguous) {
                    rightMesh->addFace(upper[0], upper[1], intersection[1].position);
                    rightMesh->addFace(intersection[0].position, intersection[1].position, upper[1]);
                    leftMesh->addFace(intersection[0].position, lower[0], intersection[1].position);
                }
            } else if (lower.size() == 2){
                float dotproduct = QVector3D::dotProduct(target_plane_normal, QVector3D::normal(lower[0], intersection[1].position, intersection[0].position));
                bool facingNormal = dotproduct>0;
                bool facingNormalAmbiguous = abs(dotproduct)<0.1;

                if (facingNormal || facingNormalAmbiguous){
                    leftMesh->addFace(lower[0], intersection[1].position, intersection[0].position);
                    leftMesh->addFace(lower[0], lower[1], intersection[1].position);
                    rightMesh->addFace(upper[0], intersection[0].position, intersection[1].position);
                }
                if (!facingNormal || facingNormalAmbiguous){
                    leftMesh->addFace(intersection[1].position, lower[0], intersection[0].position);
                    leftMesh->addFace(lower[1], lower[0], intersection[1].position);
                    rightMesh->addFace(intersection[0].position, upper[0], intersection[1].position);
                }
            } else {
                qDebug() << "wrong faces";
            }


        } else if (faceLeftToPlane){
            leftMesh->addFace(mesh->vertices[mf.mesh_vertex[0]].position, mesh->vertices[mf.mesh_vertex[1]].position, mesh->vertices[mf.mesh_vertex[2]].position);
        } else if (faceRightToPlane){
            rightMesh->addFace(mesh->vertices[mf.mesh_vertex[0]].position, mesh->vertices[mf.mesh_vertex[1]].position, mesh->vertices[mf.mesh_vertex[2]].position);
        }
    }


    if (cutFillMode == 2){ // if fill holes
        // contour construction
        Paths3D contours = contourConstruct3D(cuttingEdges);

        QVector3D plane_normal = QVector3D::normal(plane[0], plane[1], plane[2]);

        // copy paths3d to vector<containmentPath*>
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
                    leftMesh->addFace(contour[i].position, centerOfMass, contour[(i+1)%contour.size()].position);
                    rightMesh->addFace(contour[(i+1)%contour.size()].position, centerOfMass, contour[i].position);
                } else {
                    leftMesh->addFace(contour[(i+1)%contour.size()].position, centerOfMass, contour[i].position);
                    rightMesh->addFace(contour[i].position, centerOfMass, contour[(i+1)%contour.size()].position);
                }
            }
        }
    }


    qDebug() << "done bisect";

    // 승환 30%
    qmlManager->setProgress(0.22);
    leftMesh->connectFaces();

    // 승환 40%
    qmlManager->setProgress(0.41);
    rightMesh->connectFaces();

    // 승환 50%
    qmlManager->setProgress(0.56);
    qDebug() << "done connecting";

    emit bisectDone();
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
}
void GLModel::regenerateCuttingPoint(vector<QVector3D> points){
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

void GLModel::generateCuttingContour(vector<QVector3D> cuttingContour){
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
    QVector3D tmp = parentModel->m_transform->translation();
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
        float zlength = parentModel->mesh->z_max - parentModel->mesh->z_min;
        parentModel->planeTransform[i]->setTranslation(desire_normal*(-world_origin.distanceToPlane(v1,v2,v3))+QVector3D(tmp.x(),tmp.y(),-parentModel->mesh->z_min));

        parentModel->planeObjectPicker[i] = new Qt3DRender::QObjectPicker;//parentModel->planeEntity[i]);

        parentModel->planeObjectPicker[i]->setHoverEnabled(true);
        parentModel->planeObjectPicker[i]->setEnabled(true);
        //QObject::connect(parentModel->planeObjectPicker[i], SIGNAL(clicked(Qt3DRender::QPickEvent*)), this, SLOT(handlePickerClicked(Qt3DRender::QPickEvent*)));
        QObject::connect(parentModel->planeObjectPicker[i], SIGNAL(released(Qt3DRender::QPickEvent*)), this, SLOT(handlePickerClickedFreeCut(Qt3DRender::QPickEvent*)));


        parentModel->planeEntity[i]->addComponent(parentModel->planeObjectPicker[i]);

        parentModel->planeEntity[i]->addComponent(parentModel->clipPlane[i]);
        parentModel->planeEntity[i]->addComponent(parentModel->planeTransform[i]); //jj
        parentModel->planeEntity[i]->addComponent(parentModel->planeMaterial);
    }

    parentModel->removeCuttingPoints();
    //modelCut();
}

void GLModel::generateSupport(){

    // copy mesh data from original mesh
    layerMesh = new Mesh;
    layerMesh->faces.reserve(mesh->faces.size()*2);
    layerMesh->vertices.reserve(mesh->vertices.size()*2);
    foreach (MeshFace mf, mesh->faces){
        layerMesh->addFace(mesh->idx2MV(mf.mesh_vertex[0]).position, mesh->idx2MV(mf.mesh_vertex[1]).position, mesh->idx2MV(mf.mesh_vertex[2]).position, mf.idx);
    }

    float x_length = mesh->x_max - mesh->x_min;
    float y_length = mesh->y_max - mesh->y_min;
    float z_length = mesh->z_max - mesh->z_min;
    size_t xy_reserve = x_length * y_length;
    size_t xyz_reserve = xy_reserve * z_length;
    qDebug() << "********************xy_reserve = " << xy_reserve;
    qDebug() << "********************faces_reserve = " << mesh->faces.size();
    qDebug() << "********************vertices_reserve = " << mesh->vertices.size();

    layerInfillMesh = new Mesh;
    layerInfillMesh->faces.reserve(xyz_reserve * scfg->infill_density); // infill 되면 다시 기준잡기
    layerInfillMesh->vertices.reserve(xyz_reserve * scfg->infill_density);
    layerSupportMesh = new Mesh;
    layerSupportMesh->faces.reserve(xy_reserve * 300 * scfg->support_density);
    layerSupportMesh->vertices.reserve(xy_reserve * 300 * scfg->support_density);
    layerRaftMesh = new Mesh;
    layerRaftMesh->faces.reserve(xy_reserve * 300 * scfg->support_density);
    layerRaftMesh->vertices.reserve(xy_reserve * 300 * scfg->support_density);
    QVector3D t = m_transform->translation();

    t.setZ(mesh->z_min * -1.0f);
    layerInfillMesh->vertexMove(t);
    layerSupportMesh->vertexMove(t);
    layerRaftMesh->vertexMove(t);

    // generate cylinders
    for( auto iter = slicer->slices.overhang_points.begin() ; iter != slicer->slices.overhang_points.end() ; iter++ ) {
        qDebug() << "-------" << (*iter);
        generateSupporter(layerSupportMesh, *iter);
        generateRaft(layerRaftMesh, *iter);
    }

    /*for( auto iter = slicer->slices.begin() ; iter != slicer->slices.end() ; iter++ ) {
        qDebug() << "infile" << iter->infill.size() << "outershell" << iter->outershell.size() << "support" << iter->support.size() << "z" << iter->z;
        generateInfill(layerInfillMesh, &(*iter));
    }*/

    t.setZ(scfg->raft_thickness);
    layerMesh->vertexMove(t);
    t.setZ(mesh->z_min + scfg->raft_thickness);
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
            //QObject::disconnect(parentModel->planeObjectPicker[i], SIGNAL(clicked(Qt3DRender::QPickEvent*)), this, SLOT(handlePickerClickedFreeCut(Qt3DRender::QPickEvent*)));
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
    QVector3D tmp = m_transform->translation();
    float zlength = mesh->z_max - mesh->z_min;
    cuttingPoints.push_back(v);

    sphereMesh.push_back(new Qt3DExtras::QSphereMesh);
    sphereMesh[sphereMesh.size()-1]->setRadius(0.4);

    sphereTransform.push_back(new Qt3DCore::QTransform);
    //sphereTransform[sphereTransform.size()-1]->setTranslation(v + QVector3D(tmp.x(),tmp.y(), -mesh->z_min));
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

    QObject::connect(sphereObjectPicker[sphereObjectPicker.size()-1], SIGNAL(entered()), this, SLOT(handlePickerEnteredFreeCutSphere()));
    QObject::connect(sphereObjectPicker[sphereObjectPicker.size()-1], SIGNAL(exited()), this, SLOT(handlePickerExitedFreeCutSphere()));
    QObject::connect(sphereObjectPicker[sphereObjectPicker.size()-1], SIGNAL(clicked(Qt3DRender::QPickEvent*)), this, SLOT(handlePickerClickedFreeCutSphere(Qt3DRender::QPickEvent*)));
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

void GLModel::removeModel(){
    delete vertexBuffer;
    delete vertexNormalBuffer;
    delete vertexColorBuffer;
    delete positionAttribute;
    delete normalAttribute;
    delete colorAttribute;
    removeComponent(m_geometryRenderer);
    removeComponent(m_meshMaterial);
    delete m_geometry;
    delete m_geometryRenderer;

    deleteLater();
}

void GLModel::removeModelPartList(){
    //remove part list
    QList<QObject*> temp;
    temp.append(mainWindow);
    QObject *partList = (QEntity *)FindItemByName(temp, "partList");
    QObject *yesno_popup = (QEntity *)FindItemByName(temp, "yesno_popup");

    qDebug() <<"remove ID   " << ID;
    QMetaObject::invokeMethod(partList, "deletePart", Q_ARG(QVariant, ID));
    QMetaObject::invokeMethod(yesno_popup, "deletePart", Q_ARG(QVariant, ID));
}

void GLModel::modelCut(){
    qDebug() << "modelcut called" << cutMode;
    if(cutMode == 0)
        return ;
    if(cutMode == 2 && parentModel->cuttingPoints.size() < 3)
        return ;

    parentModel->saveUndoState();

    qmlManager->openProgressPopUp();

    if (cutMode == 1){ // flat cut
        if (parentModel->cuttingPlane.size() != 3){
            return;
        }

        parentModel->bisectModel(parentModel->cuttingPlane);
    } else if (cutMode == 2){ // free cut
        if (parentModel->cuttingPoints.size() >= 3){

            // create left, rightmesh
            Mesh* leftMesh = parentModel->lmesh;
            Mesh* rightMesh = parentModel->rmesh;
            // do bisecting mesh
            leftMesh->faces.reserve(mesh->faces.size()*3);
            leftMesh->vertices.reserve(mesh->faces.size()*3);
            rightMesh->faces.reserve(mesh->faces.size()*3);
            rightMesh->vertices.reserve(mesh->faces.size()*3);

            cutAway(leftMesh, rightMesh, parentModel->mesh, parentModel->cuttingPoints, parentModel->cutFillMode);



            if (leftMesh->faces.size() == 0 || rightMesh->faces.size() == 0){
                qDebug() << "cutting contour selected not cutting";
                qmlManager->setProgress(1);
                cutModeSelected(9999); // reset
                return;
            }

            emit parentModel->bisectDone();
        }
    }
}

void GLModel::generateRLModel(){
    qDebug() << "** generateRLModel" << this;
    if (lmesh->faces.size() != 0){
        qmlManager->createModelFile(lmesh, filename+"_l");
        qDebug() << "leftmodel created";
    }
    // 승환 70%
    qmlManager->setProgress(0.72);
    if (rmesh->faces.size() != 0){
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
            shellOffset(leftmodel, (float)shellOffsetFactor);
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
    QMetaObject::invokeMethod(qmlManager->boundedBox, "hideBox");

    // 승환 100%
    qmlManager->setProgress(1);
}

// hollow shell part
void GLModel::indentHollowShell(double radius){
    qDebug() << "hollow shell called" << radius;

    QVector3D center = (mesh->idx2MV(targetMeshFace->mesh_vertex[0]).position +mesh->idx2MV(targetMeshFace->mesh_vertex[1]).position + mesh->idx2MV(targetMeshFace->mesh_vertex[2]).position)/3;
    hollowShell(mesh, targetMeshFace, center, radius);
}

GLModel::~GLModel(){
    /*delete m_transform;
    delete m_objectPicker;
    delete m_meshMaterial;
    delete mesh;
    delete rmesh;
    delete lmesh;
    delete colorAttribute;
    delete normalAttribute;
    delete positionAttribute;
    delete vertexColorBuffer;
    delete vertexNormalBuffer;
    delete vertexBuffer;
    delete m_geometry;
    delete m_geometryRenderer;*/
}

void GLModel::engoo(){
    m_meshMaterial->setAmbient(QColor(10,200,10));
}

void GLModel::exgoo(){
    m_meshMaterial->setAmbient(QColor(81,200,242));
    qDebug() << "exgoo";
}

void GLModel::mgoo(Qt3DRender::QPickEvent* v)
{
    if(v->buttons()>1){
        return;
    }

    if (qmlManager->selectedModels[0] != nullptr &&
            (qmlManager->selectedModels[0]->shadowModel->cutActive ||
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
        m_objectPicker->setDragEnabled(false);
        isReleased = true;
        qmlManager->modelClicked = false;
        return;
    }

    qmlManager->moveButton->setProperty("state", "active");
    qmlManager->setClosedHandCursor();

    if (!isMoved){ // called only once on dragged
        parentModel->saveUndoState();
        qmlManager->hideMoveArrow();
        qDebug() << "hiding move arrow";
        // for mgoo out problem
        float x_diff = parentModel->mesh->x_max - parentModel->mesh->x_min;
        float y_diff = parentModel->mesh->y_max - parentModel->mesh->y_min;
        float z_diff = parentModel->mesh->z_max - parentModel->mesh->z_min;
        float biggest = x_diff>y_diff? x_diff : y_diff;
        biggest = z_diff>biggest? z_diff : biggest;
        float scale_val = biggest > 50.0f ? 1.0f : 100.0f/biggest;
        m_transform->setScale(scale_val);
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
    qmlManager->modelClicked = true;
    if(v->buttons()>1) // pass if click with right mouse
        return;

    if (!isReleased) // to remove double click on release problem
        return;
    else
        isReleased = false;

    qDebug() << "Pressed   " << v->position() << m_transform->translation();
    m_objectPicker->setDragEnabled(true);

    lastpoint=v->localIntersection();
    prevPoint = (QVector2D) v->position();
}


/*void GLModel::drawLine(QVector3D endpoint)
{
    if (cutMode == 2){
        float line_length=endpoint.distanceToPoint(lastpoint);
        QVector3D original_normal(0,1,0);
        QVector3D desire_normal(endpoint-lastpoint);
        desire_normal.normalize();//size=1
        float angle = qAcos(QVector3D::dotProduct(original_normal,desire_normal))*180/M_PI;
        QVector3D crossproduct_vector(QVector3D::crossProduct(original_normal,desire_normal));
        QVector3D desire_point=(endpoint+lastpoint)/2;
        Qt3DExtras::QCylinderMesh* line = new Qt3DExtras::QCylinderMesh;
        line->setRadius(0.05);
        line->setLength(line_length);
        Qt3DCore::QTransform * lineTransform = new Qt3DCore::QTransform;
        lineTransform->setRotation(QQuaternion::fromAxisAndAngle(crossproduct_vector, angle));
        lineTransform->setTranslation(desire_point);
        Qt3DExtras::QPhongMaterial * lineMaterial = new Qt3DExtras::QPhongMaterial();
        lineMaterial->setDiffuse(QColor(QRgb(0x00aa00)));
        Qt3DCore::QEntity* lineEntity = new Qt3DCore::QEntity(parentModel);
        lineEntity->addComponent(line);
        lineEntity->addComponent(lineTransform);
        lineEntity->addComponent(lineMaterial);
        lastpoint=endpoint;
    }
}*/

void GLModel::cutModeSelected(int type){

    qDebug() << "cut mode selected1" << type;
    parentModel->removeCuttingPoints();
    qDebug() << "cut mode selected2" << type;
    //removePlane();
    parentModel->removeCuttingContour();
    qDebug() << "cut mode selected3" << type;
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
        float zlength = parentModel->mesh->z_max - parentModel->mesh->z_min;
        QVector3D v1(1,0, parentModel->mesh->z_min + value*zlength/1.8);
        QVector3D v2(1,1, parentModel->mesh->z_min + value*zlength/1.8);
        QVector3D v3(2,0, parentModel->mesh->z_min + value*zlength/1.8);

        QVector3D world_origin(0,0,0);
        QVector3D original_normal(0,1,0);
        QVector3D desire_normal(QVector3D::normal(v1,v2,v3)); //size=1
        float angle = qAcos(QVector3D::dotProduct(original_normal,desire_normal))*180/M_PI+(value-1)*30;
        QVector3D crossproduct_vector(QVector3D::crossProduct(original_normal,desire_normal));

        QVector3D tmp = parentModel->m_transform->translation();

        for (int i=0;i<2;i++){
            parentModel->planeTransform[i]->setTranslation(desire_normal*(-world_origin.distanceToPlane(v1,v2,v3)) +QVector3D(tmp.x(),tmp.y(),-parentModel->mesh->z_min));
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
    float height = (mesh->z_max - mesh->z_min + scfg->raft_thickness) * value + mesh->z_min;
    m_layerMaterialHeight->setValue(QVariant::fromValue(height));

    m_layerMaterialRaftHeight->setValue(QVariant::fromValue(qmlManager->getLayerViewFlags() & LAYER_INFILL != 0 ?
                mesh->z_min :
                mesh->z_max));
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
bool GLModel::EndsWith(const string& a, const string& b) {
    if (b.size() > a.size()) return false;
    return std::equal(a.begin() + a.size() - b.size(), a.end(), b.begin());
}

QString GLModel::getFileName(const string& s){
   char sep = '/';

   size_t i = s.rfind(sep, s.length());
   if (i != string::npos) {
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
    int jump = (mesh->faces.size()<100000) ? 1:mesh->faces.size()/30000; // 30000 is chosen for 800000 mesh, 30
    int factor = (jump ==1) ? 1:3*log(jump);

    Mesh* newMesh = new Mesh;

    if (jump == 1){ // if mesh face count is less than 30000
        newMesh->faces.reserve(mesh->faces.size()*2);
        newMesh->vertices.reserve(mesh->vertices.size()*2);
        foreach (MeshFace mf, mesh->faces){
            newMesh->addFace(mesh->idx2MV(mf.mesh_vertex[0]).position, mesh->idx2MV(mf.mesh_vertex[1]).position, mesh->idx2MV(mf.mesh_vertex[2]).position, mf.idx);
        }
    } else {
        newMesh->faces.reserve(mesh->faces.size());
        newMesh->vertices.reserve(mesh->faces.size());
        foreach (MeshFace mf , mesh->faces){
            if (i%jump==0){
                QCoreApplication::processEvents();

                QVector3D point1 =mesh->idx2MV(mf.mesh_vertex[0]).position;
                QVector3D point2 =mesh->idx2MV(mf.mesh_vertex[1]).position;
                QVector3D point3 =mesh->idx2MV(mf.mesh_vertex[2]).position;
                QVector3D CenterOfMass = (point1+point2+point3)/3;
                point1=GLModel::spreadPoint(point1,CenterOfMass,factor);
                point2=GLModel::spreadPoint(point2,CenterOfMass,factor);
                point3=GLModel::spreadPoint(point3,CenterOfMass,factor);
                //newMesh->addFace(mesh->idx2MV(mf.mesh_vertex[0]).position, mesh->idx2MV(mf.mesh_vertex[1]).position, mesh->idx2MV(mf.mesh_vertex[2]).position);
                newMesh->addFace(point1,point2,point3, mf.idx);
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
    //qDebug() << "labelling state changeD!!!!!!!!!!!!!!";
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

    //qDebug() <<m_transform->translation();
    //qDebug() << labellingTextPreview->translation;

    QVector3D* originalVertices = reinterpret_cast<QVector3D*>(vertexBuffer->data().data());
    int originalVerticesSize = vertexBuffer->data().size() / sizeof(float) / 3;

    QVector3D* vertices;
    int verticesSize;
    unsigned int* indices;
    int indicesSize;
    float depth = 0.5f;
    float scale = labellingTextPreview->ratioY * labellingTextPreview->scaleY;
    QVector3D translation = m_transform->translation()+labellingTextPreview->translation + QVector3D(0,-0.3,0);

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
                           parentModel->mesh,
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
        parentModel->mesh->addFace(vertices[2 * indices[3*i + 2] + 0], vertices[2 * indices[3*i + 1] + 0], vertices[2 * indices[3*i + 0] + 0]);
        //qDebug() << vertices[2 * indices[3*i + 2] + 0]<< vertices[2 * indices[3*i + 1] + 0]<< vertices[2 * indices[3*i + 0] + 0];
        outNormals.push_back(vertices[2 * indices[3*i + 2] + 1]);
        outNormals.push_back(vertices[2 * indices[3*i + 1] + 1]);
        outNormals.push_back(vertices[2 * indices[3*i + 0] + 1]);
    }

    parentModel->mesh->connectFaces();
    if (GLModel* glmodel = qobject_cast<GLModel*>(parent())) {
        glmodel->addVertices(outVertices);
        glmodel->addNormalVertices(outNormals);

        emit glmodel->_updateModelMesh(true);
    }


}

// for extension

void GLModel::colorExtensionFaces(){

    removeComponent(m_meshMaterial);
    addComponent(m_meshVertexMaterial);
    /*
    if (targetMeshFace == NULL)
        return;
    QVector3D normal = targetMeshFace->fn;

    vector<MeshFace*> extension_faces;
    detectExtensionFaces(mesh, normal, targetMeshFace, targetMeshFace, &extension_faces);
    qDebug() << "detected extension faces" << extension_faces.size();

    Paths3D extension_outlines = detectExtensionOutline(mesh, extension_faces);
    qDebug() << "detected extension outlines" << extension_outlines.size();
    */
}

void GLModel:: uncolorExtensionFaces(){
    resetColorMesh(mesh, vertexColorBuffer, extendFaces);
    removeComponent(m_meshVertexMaterial);
    addComponent(m_meshMaterial);

    /*

    if (targetMeshFace == NULL)
        return;

    QVector3D normal = targetMeshFace->fn;

    vector<MeshFace*> extension_faces;
    detectExtensionFaces(mesh, normal, targetMeshFace, targetMeshFace, &extension_faces);
    qDebug() << "detected extension faces" << extension_faces.size();

    Paths3D extension_outlines = detectExtensionOutline(mesh, extension_faces);
    qDebug() << "detected extension outlines" << extension_outlines.size();

    // do uncolor thing
    */
}
void GLModel::generateColorAttributes(){
    extendColorMesh(mesh,targetMeshFace,vertexColorBuffer,&extendFaces);
}
void GLModel::generateExtensionFaces(double distance){
    if (targetMeshFace == NULL)
        return;

    saveUndoState();
    extendMesh(mesh, targetMeshFace, distance);
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
    Qt3DCore::QTransform* tmp1 = new Qt3DCore::QTransform();
    Qt3DCore::QTransform* tmp2 = new Qt3DCore::QTransform();
    tmp1->setRotationX(angleX);
    tmp1->setRotationY(0);
    tmp1->setRotationZ(0);
    //qDebug() << "lay flat 0      ";
    //rotateModelMesh(tmp->matrix());
    tmp2->setRotationX(0);
    tmp2->setRotationY(angleY);
    tmp2->setRotationZ(0);
    rotateModelMesh(tmp1->matrix() * tmp2->matrix());
    //qDebug() << "lay flat 1      ";
    uncolorExtensionFaces();
    //closeLayflat();
    emit resetLayflat();
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
    shadowModel->m_objectPicker->setEnabled(false);
    m_objectPicker = new Qt3DRender::QObjectPicker(this);
    QObject::connect(m_objectPicker,SIGNAL(clicked(Qt3DRender::QPickEvent*)), this, SLOT(handlePickerClickedLayflat(Qt3DRender::QPickEvent*)));
    this->addComponent(m_objectPicker);
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

void GLModel::generateManualSupport(){
    qDebug() << "generateManual support called";
    if (targetMeshFace == NULL)
        return;
    QVector3D t = m_transform->translation();
    t.setZ(mesh->z_min+scfg->raft_thickness);
    QVector3D targetPosition = mesh->idx2MV(targetMeshFace->mesh_vertex[0]).position- t;
    OverhangPoint* targetOverhangPosition = new OverhangPoint(targetPosition.x()*scfg->resolution,
                                                              targetPosition.y()*scfg->resolution,
                                                              targetPosition.z()*scfg->resolution,
                                                              scfg->default_support_radius);

    generateSupporter(layerSupportMesh, targetOverhangPosition, nullptr, nullptr, layerSupportMesh->z_min);
    targetMeshFace = NULL;
    emit _updateModelMesh(true);
}

void GLModel::openScale(){
    qmlManager->sendUpdateModelInfo();
    //QMetaObject::invokeMethod(qmlManager->scalePopup, "updateSizeInfo", Q_ARG(QVariant, parentModel->mesh->x_max-parentModel->mesh->x_min), Q_ARG(QVariant, parentModel->mesh->y_max-parentModel->mesh->y_min), Q_ARG(QVariant, parentModel->mesh->z_max-parentModel->mesh->z_min));
}

void GLModel::closeScale(){
    qmlManager->sendUpdateModelInfo();
    qDebug() << "close scale";
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
    qDebug() << "changeViewMode";
    QMetaObject::invokeMethod(qmlManager->boxUpperTab, "all_off");

    switch( viewMode ) {
    case VIEW_MODE_OBJECT:
        shadowModel->layerViewActive = false;
        shadowModel->supportViewActive = false;
        addComponent(m_meshMaterial);
        removeComponent(m_layerMaterial);
        break;
    case VIEW_MODE_SUPPORT:
        shadowModel->layerViewActive = false;
        shadowModel->supportViewActive = true;
        addComponent(m_meshMaterial);
        removeComponent(m_layerMaterial);
        break;
    case VIEW_MODE_LAYER:
        shadowModel->layerViewActive = true;
        shadowModel->supportViewActive = false;
        addComponent(m_layerMaterial);
        removeComponent(m_meshMaterial);
        break;
    }

    emit _updateModelMesh(true);
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
    blendState->setSourceRgb(QBlendEquationArguments::SourceAlpha);
    blendState->setDestinationRgb(QBlendEquationArguments::OneMinusSourceAlpha);
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
    m_layerMaterial->addParameter(new QParameter(QStringLiteral("specular"), QColor(0, 0, 0)));
}
