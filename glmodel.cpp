#include <QRenderSettings>
#include "glmodel.h"
#include <QString>
#include <QtMath>
#include <cfloat>

#include "qmlmanager.h"
#include "feature/text3dgeometrygenerator.h"
#include "feature/shelloffset.h"
#include <QtConcurrent/QtConcurrent>
#include <QFuture>
#include <QFileDialog>

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
{
    connect(&futureWatcher, SIGNAL(finished()), this, SLOT(slicingDone()));

    // generates shadow model for object picking
    if (isShadow){

        lmesh = new Mesh();
        rmesh = new Mesh();

        mesh = toSparse(parentModel->mesh);
        initialize(mesh);
        addVertices(mesh, false);
        applyGeometry();

        addComponent(m_transform);

        m_meshMaterial = new QPhongMaterial();
        /*m_meshMaterial->setAmbient(QColor(255,0,0));
        m_meshMaterial->setDiffuse(QColor(173,215,218));
        m_meshMaterial->setSpecular(QColor(182,237,246));
        m_meshMaterial->setShininess(0.0f);

        addComponent(m_meshMaterial);*/

        addMouseHandlers();

        QObject::connect(this,SIGNAL(_updateModelMesh()),this,SLOT(updateModelMesh()));

        /*labellingTextPreview = new LabellingTextPreview(this);
        labellingTextPreview->setEnabled(false);

        labellingTextPreview->setTranslation(QVector3D(100,0,0));
        */

        return;
    }

    m_meshMaterial = new QPhongMaterial();
    m_meshVertexMaterial = new QPerVertexColorMaterial();

    this->changecolor(0);
    if (filename != "" && (filename.contains(".stl")||filename.contains(".STL"))\
            && !EndsWith(filename.toStdString(), std::string("_left").c_str()) && !EndsWith(filename.toStdString(),  std::string("_right").c_str()) && !EndsWith(filename.toStdString(),  std::string("_offset").c_str())){
        mesh = new Mesh();
        loadMeshSTL(mesh, filename.toStdString().c_str());
    } else if (filename != "" && (filename.contains(".obj")||filename.contains(".OBJ"))\
            && !EndsWith(filename.toStdString(), std::string("_left").c_str()) && !EndsWith(filename.toStdString(),  std::string("_right").c_str()) && !EndsWith(filename.toStdString(),  std::string("_offset").c_str())){
        mesh = new Mesh();
        loadMeshOBJ(mesh, filename.toStdString().c_str());
    } else {
        mesh = loadMesh;
    }

    // 승환 25%
    qmlManager->setProgress(0.23);

    lmesh = new Mesh();
    rmesh = new Mesh();

    initialize(mesh);
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
    QObject::connect(this,SIGNAL(_updateModelMesh()),this,SLOT(updateModelMesh()));


    qDebug() << "created shadow model";

    ft = new featureThread(this, 0);
    //arsignal = new arrangeSignalSender();//unused, signal from qml goes right into QmlManager.runArrange

    // Add to Part List
    ID = globalID++;

    /*QList<QObject*> temp;
    temp.append(mainWindow);
    QObject *partList = (QEntity *)FindItemByName(temp, "partList");

    QMetaObject::invokeMethod(partList, "addPart",
        Q_ARG(QVariant, getFileName(fname.toStdString().c_str())),
        Q_ARG(QVariant, ID));
    */
    qmlManager->addPart(getFileName(fname.toStdString().c_str()), ID);
}

void GLModel::addMouseHandlers(){

    m_objectPicker = new Qt3DRender::QObjectPicker(this);

    m_objectPicker->setHoverEnabled(true);
    //m_objectPicker->setDragEnabled(true);
    // add only m_objectPicker
    QObject::connect(m_objectPicker, SIGNAL(clicked(Qt3DRender::QPickEvent*)), this, SLOT(handlePickerClicked(Qt3DRender::QPickEvent*)));
    QObject::connect(m_objectPicker, SIGNAL(moved(Qt3DRender::QPickEvent*)), this, SLOT(mgoo(Qt3DRender::QPickEvent*)));
    QObject::connect(m_objectPicker, SIGNAL(pressed(Qt3DRender::QPickEvent*)), this, SLOT(pgoo(Qt3DRender::QPickEvent*)));
    QObject::connect(m_objectPicker, SIGNAL(released(Qt3DRender::QPickEvent*)), this, SLOT(rgoo(Qt3DRender::QPickEvent*)));
    QObject::connect(m_objectPicker, SIGNAL(entered()), this, SLOT(engoo()));
    QObject::connect(m_objectPicker, SIGNAL(exited()), this, SLOT(exgoo()));
    addComponent(m_objectPicker);
}

void GLModel::removeMouseHandlers(){
    if (m_objectPicker == nullptr)
        return;
    //m_objectPicker->setDragEnabled(true);
    // add only m_objectPicker
    QObject::disconnect(m_objectPicker, SIGNAL(clicked(Qt3DRender::QPickEvent*)), this, SLOT(handlePickerClicked(Qt3DRender::QPickEvent*)));
    QObject::disconnect(m_objectPicker, SIGNAL(moved(Qt3DRender::QPickEvent*)), this, SLOT(mgoo(Qt3DRender::QPickEvent*)));
    QObject::disconnect(m_objectPicker, SIGNAL(pressed(Qt3DRender::QPickEvent*)), this, SLOT(pgoo(Qt3DRender::QPickEvent*)));
    QObject::disconnect(m_objectPicker, SIGNAL(released(Qt3DRender::QPickEvent*)), this, SLOT(rgoo(Qt3DRender::QPickEvent*)));
    QObject::disconnect(m_objectPicker, SIGNAL(entered()), this, SLOT(engoo()));
    QObject::disconnect(m_objectPicker, SIGNAL(exited()), this, SLOT(exgoo()));
    removeComponent(m_objectPicker);
    m_objectPicker->deleteLater();
}

void GLModel::changecolor(int mode){
    if (mode == -1) mode = colorMode;
    switch(mode){
    case 0: // default
        m_meshMaterial->setAmbient(QColor(130,130,140));;
        m_meshMaterial->setDiffuse(QColor(131,206,220));
        m_meshMaterial->setDiffuse(QColor(97,185,192));
        m_meshMaterial->setSpecular(QColor(0,0,0));
        m_meshMaterial->setShininess(0.0f);
        colorMode = 0;
        break;
    case 1:
        //m_meshMaterial->setAmbient(QColor(180,180,180));
        m_meshMaterial->setDiffuse(QColor(100,255,100));
        m_meshMaterial->setDiffuse(QColor(130,208,125));
        m_meshMaterial->setSpecular(QColor(0,0,0));
        m_meshMaterial->setShininess(0.0f);
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
    // need to change to memcpy or something
    qDebug () << "save prev mesh";

    // need to remove redo State since it contains
    mesh->nextMesh = nullptr;

    // copy current Mesh as temporary prev_mesh
    Mesh* temp_prev_mesh = new Mesh();
    temp_prev_mesh->faces.reserve(mesh->faces.size()*3);
    temp_prev_mesh->vertices.reserve(mesh->faces.size()*3);
    foreach (MeshFace mf, mesh->faces){
        temp_prev_mesh->addFace(mesh->vertices[mf.mesh_vertex[0]].position,
                mesh->vertices[mf.mesh_vertex[1]].position,
                mesh->vertices[mf.mesh_vertex[2]].position);
    }
    temp_prev_mesh->connectFaces();
    temp_prev_mesh->prevMesh = mesh->prevMesh;
    if (mesh->prevMesh != nullptr)
        mesh->prevMesh->nextMesh = temp_prev_mesh;
    temp_prev_mesh->nextMesh = mesh;

    Mesh* deleteTargetMesh = mesh;
    for (int i=0; i<10; i++){ // maximal undo count is 10
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
    if (mesh->prevMesh != nullptr){
        mesh = mesh->prevMesh;
        emit _updateModelMesh();
    }
}

void GLModel::loadRedoState(){
    if (mesh->nextMesh != nullptr){
        mesh = mesh->nextMesh;
        emit _updateModelMesh();
    }
}

void GLModel::moveModelMesh(QVector3D direction){
    mesh->vertexMove(direction);
    /*if (shadowModel != NULL)
        shadowModel->moveModelMesh(direction);*/

    qDebug() << "moved vertex";
    emit _updateModelMesh();
}
void GLModel::scaleModelMesh(float scaleX, float scaleY, float scaleZ){
    mesh->vertexScale(scaleX, scaleY, scaleZ);
    /*if (shadowModel != NULL)
        shadowModel->scaleModelMesh(scale);*/

    emit _updateModelMesh();
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
    emit _updateModelMesh();
}

void GLModel::copyModelAttributeFrom(GLModel* from){
    cutMode = from->cutMode;
    cutFillMode = from->cutFillMode;
    labellingActive = from->labellingActive;
    extensionActive = from->extensionActive;
    cutActive = from->cutActive;
    hollowShellActive = from->hollowShellActive;
    shellOffsetActive = from->shellOffsetActive;
    layflatActive = from->layflatActive;

}

void GLModel::updateModelMesh(){
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
    initialize(mesh);
    addVertices(mesh, false);
    applyGeometry();

    QMetaObject::invokeMethod(qmlManager->boundedBox, "setPosition", Q_ARG(QVariant, m_transform->translation()+QVector3D((mesh->x_max+mesh->x_min)/2,(mesh->y_max+mesh->y_min)/2,(mesh->z_max+mesh->z_min)/2)));
    QMetaObject::invokeMethod(qmlManager->boundedBox, "setSize", Q_ARG(QVariant, mesh->x_max - mesh->x_min),
                                                     Q_ARG(QVariant, mesh->y_max - mesh->y_min),
                                                     Q_ARG(QVariant, mesh->z_max - mesh->z_min));

    // create new object picker, shadowmodel, remove prev shadowmodel
    //QVector3D translation = shadowModel->m_transform->translation();
    if (shadowModel !=NULL){
        QObject::disconnect(shadowModel, SIGNAL(modelSelected(int)), qmlManager, SLOT(modelSelected(int)));
        shadowModel->removeMouseHandlers();
        qmlManager->disconnectHandlers(this);
        GLModel* prevShadowModel = shadowModel;
        shadowModel=new GLModel(this->mainWindow, this, mesh, filename, true);
        shadowModel->copyModelAttributeFrom(prevShadowModel);
        prevShadowModel->deleteLater();

        // reconnect handler if current selected model is updated
        if (qmlManager->selectedModel==this)
            qmlManager->connectHandlers(this);
        //shadowModel->m_transform->setTranslation(translation);
        QObject::connect(shadowModel, SIGNAL(modelSelected(int)), qmlManager, SLOT(modelSelected(int)));
    }

    //m_objectPicker->setDragEnabled(true);
    // add only m_objectPicker
    /*if (shadowModel == NULL){ // it is shadowmodel itself
        removeMouseHandlers();
        addMouseHandlers();
    }*/

    /*GLModel* temp = shadowModel;
    shadowModel=new GLModel(this->mainWindow, this, mesh, filename, true);
    shadowModel->m_objectPicker = op;
    temp->removeModel();*/
    // need to reenable objectPicker

    QVector3D tmp = m_transform->translation();
    float zlength = mesh->z_max - mesh->z_min;
    //if (shadowModel != NULL) // since shadow model transformed twice



    m_transform->setTranslation(QVector3D(tmp.x(),tmp.y(),-mesh->z_min));
    checkPrintingArea();
    QMetaObject::invokeMethod(qmlManager->scalePopup, "updateSizeInfo", Q_ARG(QVariant, mesh->x_max-mesh->x_min), Q_ARG(QVariant, mesh->y_max-mesh->y_min), Q_ARG(QVariant, mesh->z_max-mesh->z_min));
    qDebug() << "model transform :" <<m_transform->translation() << mesh->x_max << mesh->x_min << mesh->y_max << mesh->y_min << mesh->z_max << mesh->z_min;


}

void GLModel::slicingDone(){
    QString result = futureWatcher.result()->slicingInfo;
    slicingInfo = result;
    qmlManager->slicingData->setProperty("visible", true);
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
    switch (optype){
        case ftrOpen:
            {
                break;
            }
        case ftrSave:
            {
                qDebug() << "file save called";
                QString fileName = QFileDialog::getSaveFileName(nullptr, tr("Save to STL file"), "", tr("3D Model file (*.stl)"));
                qmlManager->openProgressPopUp();
                QFuture<void> future = QtConcurrent::run(ste, &STLexporter::exportSTL, m_glmodel->mesh,fileName);
                break;
            }
        case ftrExport:
            {
                // save to file
                /*QString fileName = QFileDialog::getSaveFileName(nullptr, tr("Save to STL file"), "", tr("3D Model file (*.stl)"));
                ste->exportSTL(m_glmodel->mesh, fileName);
                */

                // slice file
                qmlManager->openProgressPopUp();
                QFuture<Slicer*> future = QtConcurrent::run(se, &SlicingEngine::slice, data, m_glmodel->mesh, m_glmodel->filename);
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
                m_glmodel->saveUndoState();
                qmlManager->openProgressPopUp();
                rotateResult* rotateres= ot->Tweak(m_glmodel->mesh,true,45,&m_glmodel->appropriately_rotated);
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
                shellOffset(m_glmodel, -0.5);
                break;
            }
        case ftrExtend:
            {

                break;
            }
        case ftrSupport:
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

void GLModel::initialize(const Mesh* mesh){

    m_geometryRenderer = new QGeometryRenderer();
    m_geometry = new QGeometry(m_geometryRenderer);


    QByteArray vertexArray;
    vertexArray.resize(mesh->faces.size()*3*(3)*sizeof(float));
    vertexBuffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer,m_geometry);
    vertexBuffer->setUsage(Qt3DRender::QBuffer::DynamicDraw);
    vertexBuffer->setData(vertexArray);

    QByteArray vertexNormalArray;
    vertexNormalArray.resize(mesh->faces.size()*3*(3)*sizeof(float));
    vertexNormalBuffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer,m_geometry);
    vertexNormalBuffer->setUsage(Qt3DRender::QBuffer::DynamicDraw);
    vertexNormalBuffer->setData(vertexNormalArray);

    QByteArray vertexColorArray;
    vertexColorArray.resize(mesh->faces.size()*3*(3)*sizeof(float));
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

void GLModel::addVertices(Mesh* mesh, bool CW)
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
                result_vcs.push_back(QVector3D(1.0f, 0.0f, 0.0f));
            }
        } else {
            for (int fn=0; fn<=2; fn++){
                result_vcs.push_back(QVector3D(1.0f, 0.0f, 0.0f));
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

    for (int i=0; i<vertex_color_cnt; i++){
        //coordinates of left vertex
        //97 185 192
        //reVertexArray[3*i+0] = 0.38;
        //reVertexArray[3*i+1] = 0.725;
        //reVertexArray[3*i+2] = 0.753;
        reVertexArray[3*i+0] = 0.278;
        reVertexArray[3*i+1] = 0.670;
        reVertexArray[3*i+2] = 0.706;
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

void GLModel::handlePickerClickedFreeCut(Qt3DRender::QPickEvent* pick)
{
    if (pick->position().x() < 260 && pick->position().y() < 330) // cut panel
        return;
    qDebug() << pick->position();
    qDebug() << "handle picker clicked freecut";
    qDebug() << pick->localIntersection()<<"pick" << cuttingPoints.size() << parentModel->cuttingPoints.size();
    QVector3D v = pick->localIntersection();
    QVector3D result_v = QVector3D(v.x(), -v.z(), v.y());
    result_v = result_v*2;
    parentModel->addCuttingPoint(result_v);
}

void GLModel::handlePickerClicked(QPickEvent *pick)
{
    qDebug() << "handle Picker clicked";
    if (!parentModel)
        return;

    if (qmlManager->yesno_popup->property("isFlawOpen").toBool())
        return;

    if(qmlManager->selectedModel != nullptr && (pick->button() & Qt::RightButton)){ // when right button clicked
        if(qmlManager->selectedModel->ID == parentModel->ID){
            QMetaObject::invokeMethod(qmlManager->mttab, "tabOnOff");
            return;
        }

    }

    if (!cutActive && !extensionActive && !labellingActive && !layflatActive && !isMoved)
        emit modelSelected(parentModel->ID);

    qDebug() << "model selected emit" << pick->position() << parentModel->ID;

    if(pick->button() & Qt::RightButton){
        QMetaObject::invokeMethod(qmlManager->mttab, "tabOnOff");
    }

    QPickTriangleEvent *trianglePick = static_cast<QPickTriangleEvent*>(pick);

    if (labellingActive && trianglePick) {
        MeshFace shadow_meshface = mesh->faces[trianglePick->triangleIndex()];

        parentModel->targetMeshFace = &parentModel->mesh->faces[shadow_meshface.parent_idx];

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
        }
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
        }
    }

    if (cutActive){
        if (cutMode == 1){
            qDebug() << "cut mode 1";

        } else if (cutMode == 2){// && parentModel->numPoints< sizeof(parentModel->sphereEntity)/4) {
            qDebug() << pick->localIntersection()<<"pick" << cuttingPoints.size() << parentModel->cuttingPoints.size();
            QVector3D v = pick->localIntersection();
            parentModel->addCuttingPoint(v);

                //generatePlane();
            //parentModel->ft->ct->addCuttingPoint(parentModel, v);
        } else if (cutMode == 9999){
            qDebug() << "current cut mode :" << cutMode;
            //return;
        }
    }

    if (extensionActive && trianglePick){
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

    if (layflatActive){
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
        /*qDebug() << trianglePick->localIntersection() \
                 << parentModel->mesh->idx2MV(parentModel->targetMeshFace->mesh_vertex[0]).position\
                << parentModel->mesh->idx2MV(parentModel->targetMeshFace->mesh_vertex[1]).position\
                << parentModel->mesh->idx2MV(parentModel->targetMeshFace->mesh_vertex[2]).position;
        */
        // << parentModel->targetMeshFace->mesh_vertex[1] << parentModel->targetMeshFace->mesh_vertex[2];
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
    QObject::disconnect(m_objectPicker,SIGNAL(clicked(Qt3DRender::QPickEvent*)), this, SLOT(handlePickerClickedLayflat(Qt3DRender::QPickEvent*)));
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

void GLModel::generatePlane(){
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
        QObject::connect(parentModel->planeObjectPicker[i], SIGNAL(clicked(Qt3DRender::QPickEvent*)), this, SLOT(handlePickerClickedFreeCut(Qt3DRender::QPickEvent*)));//SLOT(handlePickerClicked(Qt3DRender::QPickEvent*)));

        parentModel->planeEntity[i]->addComponent(parentModel->clipPlane[i]);
        parentModel->planeEntity[i]->addComponent(parentModel->planeTransform[i]);
        parentModel->planeEntity[i]->addComponent(parentModel->planeMaterial);
        parentModel->planeEntity[i]->addComponent(parentModel->planeObjectPicker[i]);
    }

    parentModel->removeCuttingPoints();
    //modelCut();
}

void GLModel::removePlane(){
    if (parentModel->planeMaterial == nullptr)
        return;
    delete parentModel->planeMaterial;
    parentModel->planeMaterial = nullptr;
    for (int i=0;i<2;i++){
        delete parentModel->clipPlane[i];
        delete parentModel->planeTransform[i];
        QObject::disconnect(parentModel->planeObjectPicker[i], SIGNAL(clicked(Qt3DRender::QPickEvent*)), this, SLOT(handlePickerClickedFreeCut(Qt3DRender::QPickEvent*)));
        delete parentModel->planeObjectPicker[i];
        delete parentModel->planeEntity[i];
    }
}

void GLModel::addCuttingPoint(QVector3D v){
    QVector3D tmp = m_transform->translation();
    float zlength = mesh->z_max - mesh->z_min;
    cuttingPoints.push_back(v);

    sphereMesh.push_back(new Qt3DExtras::QSphereMesh);
    sphereMesh[sphereMesh.size()-1]->setRadius(0.5);
    sphereTransform.push_back(new Qt3DCore::QTransform);
    sphereTransform[sphereTransform.size()-1]->setTranslation(v + QVector3D(tmp.x(),tmp.y(), -mesh->z_min));

    sphereMaterial.push_back(new Qt3DExtras::QPhongMaterial());
    sphereMaterial[sphereMaterial.size()-1]->setAmbient(QColor(QRgb(0x0049FF)));
    sphereMaterial[sphereMaterial.size()-1]->setDiffuse(QColor(QRgb(0x0049FF)));
    sphereMaterial[sphereMaterial.size()-1]->setSpecular(QColor(QRgb(0x0049FF)));
    sphereMaterial[sphereMaterial.size()-1]->setShininess(0.0f);

    sphereEntity.push_back(new Qt3DCore::QEntity(parentModel));
    sphereEntity[sphereEntity.size()-1]->addComponent(sphereMesh[sphereMesh.size()-1]);
    sphereEntity[sphereEntity.size()-1]->addComponent(sphereTransform[sphereTransform.size()-1]);
    sphereEntity[sphereEntity.size()-1]->addComponent(sphereMaterial[sphereMaterial.size()-1]);
}

void GLModel::removeCuttingPoints(){

    qDebug() << "in the removeCuttingPOints";

    qDebug() << "ok till here";
    for (int i=0; i<sphereEntity.size(); i++){
        sphereEntity[i]->removeComponent(sphereMesh[i]);
        sphereEntity[i]->removeComponent(sphereTransform[i]);
        sphereEntity[i]->removeComponent(sphereMaterial[i]);
    }
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

    qDebug() <<"remove ID   " << ID;
    QMetaObject::invokeMethod(partList, "deletePart", Q_ARG(QVariant, ID));
}

void GLModel::modelCut(){
    qDebug() << "modelcut called" << cutMode;
    if(cutMode == 0)
        return ;

    parentModel->saveUndoState();

    qmlManager->openProgressPopUp();

    if (cutMode == 1){
        if (parentModel->cuttingPlane.size() != 3){
            return;
        }

        parentModel->bisectModel(parentModel->cuttingPlane);
    } else if (cutMode == 2){
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

            emit parentModel->bisectDone();
        }
    }
}

void GLModel::generateRLModel(){
    if (lmesh->faces.size() != 0)
        qmlManager->createModelFile(lmesh, filename+"_left");

    // 승환 70%
    qmlManager->setProgress(0.72);
    if (rmesh->faces.size() != 0)
        qmlManager->createModelFile(rmesh, filename+"_right");

    // 승환 90%
    qmlManager->setProgress(0.91);

    if (shadowModel->shellOffsetActive){
        GLModel* leftmodel = qmlManager->findGLModelByName(filename+"_left");
        GLModel* rightmodel = qmlManager->findGLModelByName(filename+"_right");
        shellOffset(leftmodel, (float)shellOffsetFactor);
        qmlManager->deleteModelFile(rightmodel->ID);
        QMetaObject::invokeMethod(qmlManager->boxUpperTab, "all_off");
    }

    qDebug() << "left right generated";

    //parentModel->deleteLater();
    shadowModel->removePlane();
    removeCuttingPoints();

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
}
void GLModel::mgoo(Qt3DRender::QPickEvent* v)
{
    if(v->buttons()>1){
        return;
    }
    if (cutActive || extensionActive || labellingActive || layflatActive)
        return;

    qmlManager->setClosedHandCursor();
    isMoved = true;
    QVector2D currentPoint = (QVector2D)v->position();

    QList<QVector3D> targetPoints;

    targetPoints.append(lastpoint + QVector3D(-5,-5,0));
    targetPoints.append(lastpoint + QVector3D(5,-5,0));
    targetPoints.append(lastpoint + QVector3D(0,0,0));
    targetPoints.append(lastpoint + QVector3D(-5,5,0));
    targetPoints.append(lastpoint + QVector3D(5,5,0));
    /*
    qDebug() << "answer   " << targetPoints.at(0) << targetPoints.at(1);
    qDebug() << "answer              " << targetPoints.at(2);
    qDebug() << "answer   " << targetPoints.at(3) << targetPoints.at(4);

    qDebug() << "--------------------------------------------------";

    qDebug() << "answer   " << world2Screen(targetPoints.at(0)) << world2Screen(targetPoints.at(1));
    qDebug() << "answer              " << world2Screen(targetPoints.at(2));
    qDebug() << "answer   " << world2Screen(targetPoints.at(3)) << world2Screen(targetPoints.at(4));

    qDebug() << "--------------------------------------------------";
    */

    QVector2D xAxis = (world2Screen(targetPoints.at(4)) - world2Screen(targetPoints.at(3))) / 10;
    QVector2D yAxis = (world2Screen(targetPoints.at(0)) - world2Screen(targetPoints.at(3))) / 10;
    QVector2D target = prevPoint - currentPoint;

    QVector3D calculateX, calculateY;
    if(xAxis.length() != 0)
        calculateX = (targetPoints.at(4) - targetPoints.at(3)) * QVector2D::dotProduct(xAxis,target) /xAxis.length() / 100;
    else
        calculateX = QVector3D(0,0,0);

    if(yAxis.length() != 0)
        calculateY = (targetPoints.at(0) - targetPoints.at(3)) * QVector2D::dotProduct(yAxis,target) /yAxis.length() / 100;
    else
        calculateY = QVector3D(0,0,0);
    qDebug() << "calculate X   " << calculateX;
    qDebug() << "calculate Y   " << calculateY;

    float scaleFactor = qmlManager->systemTransform->scale3D().x() / 0.004f;
    qmlManager->modelMoveF(1,-(calculateX.x() + calculateY.x()) * 1.5 / scaleFactor);
    qmlManager->modelMoveF(2,-(calculateX.y() + calculateY.y()) * 1.5 / scaleFactor);

    prevPoint = currentPoint;
}

void GLModel::pgoo(Qt3DRender::QPickEvent* v){
    if(v->buttons()>1) // pass if click with right mouse
        return;

    qDebug() << "Pressed   " << v->position();
    m_objectPicker->setDragEnabled(true);
    lastpoint=v->localIntersection();
    prevPoint = (QVector2D) v->position();
    
}

void GLModel::rgoo(Qt3DRender::QPickEvent* v){
    qDebug() << "Released";
    m_objectPicker->setDragEnabled(false);
    qmlManager->resetCursor();

    if(isMoved)
        qmlManager->modelMoveDone(3);
    isMoved = false;
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
    removePlane();
    qDebug() << "cut mode selected3" << type;
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
    int jump = (mesh->faces.size()<30000) ? 1:mesh->faces.size()/30000; // 30000 is chosen for 800000 mesh, 30
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
}

void GLModel::closeLabelling()
{
    labellingActive = false;
    if (labellingTextPreview){
        labellingTextPreview->deleteLater();
        labellingTextPreview = nullptr;
    }
    parentModel->targetMeshFace = nullptr;
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

void GLModel::applyLabelInfo(QString text, int contentWidth, QString fontName, bool isBold, int fontSize){
    QVector3D translation;

    if (labellingTextPreview && labellingTextPreview->isEnabled()){
        translation = labellingTextPreview->translation;
        labellingTextPreview->deleteLater();
        labellingTextPreview = nullptr;
    }
    labellingTextPreview = new LabellingTextPreview(this);
    labellingTextPreview->setEnabled(true);

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
    }
}


void GLModel::generateText3DMesh()
{
    if (!labellingTextPreview)
        return;

    parentModel->saveUndoState();

    QVector3D* originalVertices = reinterpret_cast<QVector3D*>(vertexBuffer->data().data());
    int originalVerticesSize = vertexBuffer->data().size() / sizeof(float) / 3;

    QVector3D* vertices;
    int verticesSize;
    unsigned int* indices;
    int indicesSize;
    float depth = 0.5f;
    float scale = labellingTextPreview->ratioY * labellingTextPreview->scaleY;
    QVector3D translation = labellingTextPreview->translation+ QVector3D(0,-0.3,0);

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
                           mesh,
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

        outNormals.push_back(vertices[2 * indices[3*i + 2] + 1]);
        outNormals.push_back(vertices[2 * indices[3*i + 1] + 1]);
        outNormals.push_back(vertices[2 * indices[3*i + 0] + 1]);
    }
    parentModel->mesh->connectFaces();
    if (GLModel* glmodel = qobject_cast<GLModel*>(parent())) {
        glmodel->addVertices(outVertices);
        glmodel->addNormalVertices(outNormals);

        emit glmodel->_updateModelMesh();
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
    emit _updateModelMesh();
}

void GLModel::generateLayFlat(){
    //MeshFace shadow_meshface = *targetMeshFace;

    if(targetMeshFace == NULL)
        return;

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
    qDebug() << "open layflat called";
    layflatActive = true;
    qDebug() << "open layflat called end  " << layflatActive;
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
}

void GLModel::closeExtension(){
    extensionActive = false;
    parentModel->uncolorExtensionFaces();
    parentModel->targetMeshFace = nullptr;
}

void GLModel::openScale(){
    QMetaObject::invokeMethod(qmlManager->scalePopup, "updateSizeInfo", Q_ARG(QVariant, parentModel->mesh->x_max-parentModel->mesh->x_min), Q_ARG(QVariant, parentModel->mesh->y_max-parentModel->mesh->y_min), Q_ARG(QVariant, parentModel->mesh->z_max-parentModel->mesh->z_min));
}

void GLModel::closeScale(){
    qDebug() << "close scale";
}

// for shell offset
void GLModel::generateShellOffset(double factor){
    saveUndoState();
    qmlManager->openProgressPopUp();
    QString original_filename = filename;

    cutFillMode = 1;
    shellOffsetFactor = factor;

    shadowModel->modelCut();

}

void GLModel::openCut(){
    qDebug() << "opencut called";
    cutActive = true;
}

void GLModel::closeCut(){
    qDebug() << "closecut called";
    cutActive = false;
    removePlane();
    parentModel->removeCuttingPoints();
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
}
