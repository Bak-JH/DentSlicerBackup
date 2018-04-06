#include <QRenderSettings>
#include "glmodel.h"
#include <QString>
#include <QtMath>
#include <cfloat>

#include "qmlmanager.h"
#include "feature/text3dgeometrygenerator.h"
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
    , cutMode(0)
    , numPoints(0)
{

    // generates shadow model for object picking
    if (isShadow){

        lmesh = new Mesh();
        rmesh = new Mesh();

        mesh = toSparse(parentModel->mesh);

        initialize(mesh);
        addVertices(mesh, false);

        addComponent(m_transform);

        m_meshMaterial = new QPhongMaterial();
        /*m_meshMaterial->setAmbient(QColor(77,128,0));
        m_meshMaterial->setDiffuse(QColor(173,215,218));
        m_meshMaterial->setSpecular(QColor(182,237,246));
        m_meshMaterial->setShininess(0.0f);
        addComponent(m_meshMaterial);*/

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

        labellingTextPreview = new LabellingTextPreview(this);
        labellingTextPreview->setEnabled(false);

        return;
    }

    m_meshMaterial = new QPhongMaterial();
    m_meshMaterial->setAmbient(QColor(77,128,135));
    m_meshMaterial->setDiffuse(QColor(173,215,218));
    m_meshMaterial->setSpecular(QColor(182,237,246));
    m_meshMaterial->setShininess(0.0f);

    if (filename != ""){
        mesh = new Mesh();
        loadMeshSTL(mesh, filename.toStdString().c_str());
    } else {
        mesh = loadMesh;
    }

    lmesh = new Mesh();
    rmesh = new Mesh();

    initialize(mesh);
    addVertices(mesh, false);
    //QFuture<void> future = QtConcurrent::run(this, &GLModel::initialize, mesh);
    //future = QtConcurrent::run(this, &GLModel::addVertices, mesh);

    Qt3DExtras::QDiffuseMapMaterial *diffuseMapMaterial = new Qt3DExtras::QDiffuseMapMaterial();

    addComponent(m_transform);
    addComponent(m_meshMaterial);

    qDebug() << "created original model";

    //repairMesh(mesh);

    // create shadow model to handle picking settings
    shadowModel = new GLModel(this->mainWindow, this, mesh, filename, true);

    QObject::connect(this,SIGNAL(bisectDone()),this,SLOT(generateRLModel()));
    QObject::connect(this,SIGNAL(_updateModelMesh()),this,SLOT(updateModelMesh()));


    qDebug() << "created shadow model";

    ft = new featureThread(this, 0);
    arsignal = new arrangeSignalSender();

    // Add to Part List
    ID = globalID++;

    QList<QObject*> temp;
    temp.append(mainWindow);
    QObject *partList = (QEntity *)FindItemByName(temp, "partList");

    QMetaObject::invokeMethod(partList, "addPart",
        Q_ARG(QVariant, getFileName(fname.toStdString().c_str())),
        Q_ARG(QVariant, ID));

}

void GLModel::moveModelMesh(QVector3D direction){
    mesh->vertexMove(direction);
    emit _updateModelMesh();
}
void GLModel::scaleModelMesh(float scale){
    mesh->vertexScale(scale);
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

void GLModel::updateModelMesh(){
    delete vertexBuffer;
    delete vertexColorBuffer;
    delete vertexNormalBuffer;
    delete positionAttribute;
    delete normalAttribute;
    delete colorAttribute;
    removeComponent(m_geometryRenderer);
    delete m_geometry;
    delete m_geometryRenderer;
    initialize(mesh);
    addVertices(mesh, false);
    Qt3DRender::QObjectPicker* op = shadowModel->m_objectPicker;
    GLModel* temp = shadowModel;
    shadowModel=new GLModel(this->mainWindow, this, mesh, filename, true);
    shadowModel->m_objectPicker = op;
    temp->removeModel();
    QVector3D tmp = m_transform->translation();
    float zlength = mesh->z_max - mesh->z_min;
    m_transform->setTranslation(QVector3D(tmp.x(),tmp.y(),zlength/2));
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

    connect(ot, SIGNAL(progressChanged(float)), this, SLOT(progressChanged(float)));
    connect(ct, SIGNAL(progressChanged(float)), this, SLOT(progressChanged(float)));
    connect(ar, SIGNAL(progressChanged(float)), this, SLOT(progressChanged(float)));

}

void featureThread::setTypeAndRun(int type){
    optype = type;
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
                QFuture<void> future = QtConcurrent::run(ste, &STLexporter::exportSTL, m_glmodel->mesh,fileName);
                break;
            }
        case ftrExport:
            {
                // save to file
                QString fileName = QFileDialog::getSaveFileName(nullptr, tr("Save to STL file"), "", tr("3D Model file (*.stl)"));
                ste->exportSTL(m_glmodel->mesh, fileName);

                // slice file
                QFuture<void> future = QtConcurrent::run(se, &SlicingEngine::slice, data, fileName);
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
                emit (m_glmodel->arsignal)->runArrange();
                break;
            }
        case ftrOrient:
            {
                if(m_glmodel->appropriately_rotated){
                    markPopup(false);
                } else {
                    markPopup(true);
                    rotateResult* rotateres= ot->Tweak(m_glmodel->mesh,true,45,&m_glmodel->appropriately_rotated);
                    m_glmodel->rotateModelMesh(rotateres->R);
                    //m_glmodel->m_transform->setMatrix(rotateres->R);
                }
                break;
            }
        case ftrScale:
            {
                break;
            }
        case ftrRepair:
            {
                repairMesh(m_glmodel->mesh);
                break;
            }
        case ftrCut:
            {
                // m_glmodel is shadow model
                /*markPopup(false);
                ct->removeCuttingPoints();
                ct->generatePlane(m_glmodel->parentModel);
                moveToThread(curThread);

                Plane plane;
                plane.push_back(ct->cuttingPoints[ct->cuttingPoints.size()-3]);
                plane.push_back(ct->cuttingPoints[ct->cuttingPoints.size()-2]);   //plane에 잘 들어감
                plane.push_back(ct->cuttingPoints[ct->cuttingPoints.size()-1]);
                ct->bisectModel(m_glmodel->mesh, plane, m_glmodel->lmesh, m_glmodel->rmesh);

                //GLModel* leftModel = new GLModel(m_glmodel, m_glmodel->lmesh, "", false);

                qDebug() << "deleting shadowmodel";
                m_glmodel->shadowModel->deleteLater();
                qDebug() << "deleting current Model";
                m_glmodel->deleteLater();*/
                m_glmodel->modelCut();
                break;
            }
        case ftrShellOffset:
            {
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

void featureThread::progressChanged(float value){
    emit featureThread::setProgress(value);
}

void featureThread::markPopup(bool flag){
    if(flag){
        emit featureThread::loadPopup("progress_popup");
    }else{
        emit featureThread::loadPopup("result_orient");
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
    positionAttribute->setCount(12);
    positionAttribute->setName(QAttribute::defaultPositionAttributeName());

    // normal Attributes
    normalAttribute = new QAttribute();
    normalAttribute->setAttributeType(QAttribute::VertexAttribute);
    normalAttribute->setBuffer(vertexNormalBuffer);
    normalAttribute->setDataType(QAttribute::Float);
    normalAttribute->setDataSize(3);
    normalAttribute->setByteOffset(0);
    normalAttribute->setByteStride(3*sizeof(float));
    normalAttribute->setCount(12);
    normalAttribute->setName(QAttribute::defaultNormalAttributeName());

    // color Attributes
    colorAttribute = new QAttribute();
    colorAttribute->setAttributeType(QAttribute::VertexAttribute);
    colorAttribute->setBuffer(vertexColorBuffer);
    colorAttribute->setDataType(QAttribute::Float);
    colorAttribute->setDataSize(3);
    colorAttribute->setByteOffset(0);
    colorAttribute->setByteStride(3 * sizeof(float));
    colorAttribute->setCount(12);
    colorAttribute->setName(QAttribute::defaultColorAttributeName());

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

                if(mesh->idx2MV(mf.mesh_vertex[fn]).vn[0] == 0)
                    if(mesh->idx2MV(mf.mesh_vertex[fn]).vn[1] == 0)
                        if(mesh->idx2MV(mf.mesh_vertex[fn]).vn[2] == 0)
                        {
                            result_vns.pop_back();
                            result_vns.push_back(QVector3D(1,1,1));
                        }
            }
        } else {
            for (int fn=0; fn<=2; fn++){
                result_vns.push_back(mesh->idx2MV(mf.mesh_vertex[fn]).vn);

                if(mesh->idx2MV(mf.mesh_vertex[fn]).vn[0] == 0)
                    if(mesh->idx2MV(mf.mesh_vertex[fn]).vn[1] == 0)
                        if(mesh->idx2MV(mf.mesh_vertex[fn]).vn[2] == 0)
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
        reVertexArray[3*i+0] = vertices[i].x();
        reVertexArray[3*i+1] = vertices[i].y();
        reVertexArray[3*i+2] = vertices[i].z();
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

void GLModel::handlePickerClicked(QPickEvent *pick)
{
    qDebug() << pick->localIntersection()<<"pick" << parentModel->ID;
    emit modelSelected(parentModel->ID);

    if (labelingActive) {
        qDebug() << "1";
        if (labellingTextPreview)
            labellingTextPreview->setEnabled(true);
        parentModel->m_meshMaterial->setDiffuse(QColor(100, 255, 100));

        if (labellingTextPreview && labellingTextPreview->isEnabled()) {
            /*QVector3D tmp = m_transform->translation();
            float zlength = mesh->z_max - mesh->z_min;
            labellingTextPreview->planeTransform->setTranslation(QVector3D(tmp.x(),tmp.y(),zlength/2));*/

            labellingTextPreview->setTranslation(pick->localIntersection());
            labellingTextPreview->setNormal(pick->localIntersection());
        }
        qDebug() << "4";
    }

    QPickTriangleEvent *trianglePick = static_cast<QPickTriangleEvent*>(pick);
    if (cutMode == 1 && parentModel->numPoints< sizeof(parentModel->sphereEntity)/4) {
        qDebug() << pick->localIntersection()<<"pick";
        QVector3D v = pick->localIntersection();
        parentModel->addCuttingPoint(v);
        //parentModel->ft->ct->addCuttingPoint(parentModel, v);
    } else if (cutMode == 9999){
        if (auto* glmodel = qobject_cast<GLModel*>(parent()))
            glmodel->m_meshMaterial->setDiffuse(QColor(100, 255, 100));

        qDebug() << "current cut mode :" << cutMode;
        return;
    }
}

void GLModel::bisectModel(Plane plane){
    QFuture<void> future = QtConcurrent::run(this, &bisectModel_internal, plane);
}

// need to create new mesh object liek Mesh* leftMesh = new Mesh();
void GLModel::bisectModel_internal(Plane plane){

    Mesh* leftMesh = lmesh;
    Mesh* rightMesh = rmesh;
    // do bisecting mesh
    qDebug() << "in bisect";
    leftMesh->faces.reserve(mesh->faces.size());
    leftMesh->vertices.reserve(mesh->faces.size());
    rightMesh->faces.reserve(mesh->faces.size());
    rightMesh->vertices.reserve(mesh->faces.size());

    foreach (MeshFace mf, mesh->faces){
        bool faceLeftToPlane = true;
        for (int vn=0; vn<3; vn++){
            MeshVertex mv = mesh->vertices[mf.mesh_vertex[vn]];
            if (!isLeftToPlane(plane, mv.position)) // if one vertex is left to plane, append to left vertices part
                faceLeftToPlane = false;
        }
        if (faceLeftToPlane)
            leftMesh->addFace(mesh->vertices[mf.mesh_vertex[0]].position, mesh->vertices[mf.mesh_vertex[1]].position, mesh->vertices[mf.mesh_vertex[2]].position);
        else
            rightMesh->addFace(mesh->vertices[mf.mesh_vertex[0]].position, mesh->vertices[mf.mesh_vertex[1]].position, mesh->vertices[mf.mesh_vertex[2]].position);
    }

    qDebug() << "done bisect";

    leftMesh->connectFaces();
    rightMesh->connectFaces();
    qDebug() << "done connecting";
    emit bisectDone();
}

bool GLModel::isLeftToPlane(Plane plane, QVector3D position){
    // determine if position is left to plane or not
    if(position.distanceToPlane(plane[0],plane[1],plane[2])>0)
        return false;
    return true;
}

void GLModel::generatePlane(){
    parentModel->removeCuttingPoints();
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


    //To manipulate plane color, change the QRgb(0x~~~~~~).
    QVector3D world_origin(0,0,0);
    QVector3D original_normal(0,1,0);
    QVector3D desire_normal(QVector3D::normal(v1,v2,v3)); //size=1
    float angle = qAcos(QVector3D::dotProduct(original_normal,desire_normal))*180/M_PI;
    QVector3D crossproduct_vector(QVector3D::crossProduct(original_normal,desire_normal));

    for (int i=0;i<2;i++){
        parentModel->clipPlane[i]=new Qt3DExtras::QPlaneMesh();
        parentModel->clipPlane[i]->setHeight(30.0);
        parentModel->clipPlane[i]->setWidth(30.0);

        parentModel->planeTransform[i]=new Qt3DCore::QTransform();
        parentModel->planeTransform[i]->setScale(2.0f);
        parentModel->planeTransform[i]->setRotation(QQuaternion::fromAxisAndAngle(crossproduct_vector, angle+180*i));
        float zlength = parentModel->mesh->z_max - parentModel->mesh->z_min;
        parentModel->planeTransform[i]->setTranslation(desire_normal*(-world_origin.distanceToPlane(v1,v2,v3))+QVector3D(0,0,zlength/2));

        parentModel->planeEntity[i] = new Qt3DCore::QEntity(parentModel->parentModel);
        parentModel->planeEntity[i]->addComponent(parentModel->clipPlane[i]);
        parentModel->planeEntity[i]->addComponent(parentModel->planeTransform[i]);
        parentModel->planeEntity[i]->addComponent(parentModel->planeMaterial);
    }
    modelCut();
}

void GLModel::removePlane(){
    delete parentModel->planeMaterial;
    for (int i=0;i<2;i++){
        delete parentModel->clipPlane[i];
        delete parentModel->planeTransform[i];
        delete parentModel->planeEntity[i];
    }
}

void GLModel::addCuttingPoint(QVector3D v){
    float zlength = mesh->z_max - mesh->z_min;
    cuttingPoints.push_back(v);
    sphereMesh[numPoints] = new Qt3DExtras::QSphereMesh;
    sphereMesh[numPoints]->setRadius(1);
    sphereTransform[numPoints] = new Qt3DCore::QTransform;
    sphereTransform[numPoints]->setTranslation(v + QVector3D(0,0,zlength/2));

    sphereMaterial[numPoints] = new Qt3DExtras::QPhongMaterial();
    sphereMaterial[numPoints]->setAmbient(QColor(QRgb(0x0049FF)));
    sphereMaterial[numPoints]->setDiffuse(QColor(QRgb(0x0049FF)));
    sphereMaterial[numPoints]->setSpecular(QColor(QRgb(0x0049FF)));
    sphereMaterial[numPoints]->setShininess(0.0f);

    sphereEntity[numPoints] = new Qt3DCore::QEntity(parentModel);
    sphereEntity[numPoints]->addComponent(sphereMesh[numPoints]);
    sphereEntity[numPoints]->addComponent(sphereTransform[numPoints]);
    sphereEntity[numPoints]->addComponent(sphereMaterial[numPoints]);
    numPoints++;

}

void GLModel::removeCuttingPoints(){

    qDebug() << "in the removeCuttingPOints";

    qDebug() << "numPoints:" << numPoints;
    qDebug() << "ok till here";
    for(int i=0;i<numPoints;i++)
    {
        sphereEntity[i]->removeComponent(sphereMesh[i]);
        sphereEntity[i]->removeComponent(sphereTransform[i]);
        sphereEntity[i]->removeComponent(sphereMaterial[i]);
    }
    numPoints=0;
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
    if (parentModel->cuttingPoints.size() < 3){
        return;
    }

    Plane plane;
    plane.push_back(parentModel->cuttingPoints[parentModel->cuttingPoints.size()-3]);
    plane.push_back(parentModel->cuttingPoints[parentModel->cuttingPoints.size()-2]);   //plane에 잘 들어감
    plane.push_back(parentModel->cuttingPoints[parentModel->cuttingPoints.size()-1]);
    qDebug() << parentModel->parentModel;
    //parentModel->bisectModel(parentModel->mesh, plane, leftModel->mesh, rightModel->mesh);

    parentModel->bisectModel(plane);
    GLModel* leftModel = new GLModel(mainWindow, parentModel->parentModel, lmesh, "", false);
    //GLModel* rightModel = new GLModel(mainWindow, parentModel->parentModel, rmesh, "", false);

    //parentModel->bisectModel(parentModel->mesh, plane, parentModel->lmesh, parentModel->rmesh);
    //parentModel->bisectModel_internal(plane);

    //QFuture<void> future = QtConcurrent::run(parentModel, &bisectModel_internal, plane);// parentModel->bisectModel(plane);
    //qm.openModelFile("");
    //future.waitForFinished();


}

void GLModel::generateRLModel(){
    leftModel = new GLModel(parentModel->mainWindow, parentModel, lmesh, "", false);
    rightModel = new GLModel(parentModel->mainWindow, parentModel, rmesh, "", false);
    QVector3D tmp = leftModel->m_transform->translation();
    float zlength = mesh->z_max - mesh->z_min;
    leftModel->m_transform->setTranslation(QVector3D(tmp.x(),tmp.y(),zlength/2));
    tmp = rightModel->m_transform->translation();
    rightModel->m_transform->setTranslation(QVector3D(tmp.x(),tmp.y(),zlength/2));

}

void GLModel::modelCutFinished(){
    qDebug() << "modelcut finished";
    removePlane();
    //parentModel->leftModel->deleteLater();
    parentModel->rightModel->deleteLater();
    parentModel->deleteLater();
    deleteLater();

    // remove parent model and its shadow model
}

/*
void GLModel::modelCut(){
    ft->ct->removeCuttingPoints();
    ft->ct->generatePlane(parentModel);

    Plane plane;
    plane.push_back(ft->ct->cuttingPoints[ft->ct->cuttingPoints.size()-3]);
    plane.push_back(ft->ct->cuttingPoints[ft->ct->cuttingPoints.size()-2]);   //plane에 잘 들어감
    plane.push_back(ft->ct->cuttingPoints[ft->ct->cuttingPoints.size()-1]);

    ft->ct->bisectModel(mesh, plane, lmesh, rmesh);

    // select between two pieces
    GLModel* leftModel = new GLModel(mainWindow, parentModel, lmesh, "", false);
    //GLModel* rightModel = new GLModel(mainWindow, parentModel->parentModel, rmesh, "", false);

    shadowModel->deleteLater();
    deleteLater();

    qDebug() << "done model cut";

}*/


void GLModel::beforeInitialize(){
//    delete m_mesh;
    removeComponent(m_objectPicker);
    QObject::disconnect(m_objectPicker, SIGNAL(clicked(Qt3DRender::QPickEvent*)), this, SLOT(handlePickerClicked(Qt3DRender::QPickEvent*)));
    removeComponent(m_transform);
    removeComponent(m_geometryRenderer);
    removeComponent(m_meshMaterial);
    m_geometryRenderer->setGeometry(nullptr);
    m_geometry->removeAttribute(colorAttribute);
    m_geometry->removeAttribute(normalAttribute);
    m_geometry->removeAttribute(positionAttribute);
    colorAttribute->setBuffer(nullptr);
    delete colorAttribute;
    normalAttribute->setBuffer(nullptr);
    delete normalAttribute;
    positionAttribute->setBuffer(nullptr);
    delete positionAttribute;
    delete vertexColorBuffer;
    delete vertexNormalBuffer;
    delete vertexBuffer;
    delete m_geometry;
    delete m_geometryRenderer;
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

void GLModel::beforeAddVerticies(){
    //쓰지말것
    removeComponent(m_geometryRenderer);
    removeComponent(m_meshMaterial);
    m_geometryRenderer->setGeometry(nullptr);
    m_geometry->removeAttribute(positionAttribute);
    m_geometry->removeAttribute(normalAttribute);
    m_geometry->removeAttribute(colorAttribute);

    positionAttribute->setBuffer(nullptr);
//    delete vertexBuffer;
//    QByteArray vertexArray;
//    vertexArray.resize(mesh->faces.size()*3*(3)*sizeof(float));
//    vertexBuffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer,m_geometry);
//    vertexBuffer->setUsage(Qt3DRender::QBuffer::DynamicDraw);
//    vertexBuffer->setData(vertexArray);
//    positionAttribute->setBuffer(vertexBuffer);


//    normalAttribute->setBuffer(nullptr);
//    delete vertexNormalBuffer;
//    QByteArray vertexNormalArray;
//    vertexNormalArray.resize(mesh->faces.size()*3*(3)*sizeof(float));
//    vertexNormalBuffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer,m_geometry);
//    vertexNormalBuffer->setUsage(Qt3DRender::QBuffer::DynamicDraw);
//    vertexNormalBuffer->setData(vertexNormalArray);
//    normalAttribute->setBuffer(vertexNormalBuffer);

//    colorAttribute->setBuffer(nullptr);
//    delete vertexColorBuffer;
//    QByteArray vertexColorArray;
//    vertexColorArray.resize(mesh->faces.size()*3*(3)*sizeof(float));
//    vertexColorBuffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer,m_geometry);
//    vertexColorBuffer->setUsage(Qt3DRender::QBuffer::DynamicDraw);
//    vertexColorBuffer->setData(vertexColorArray);
//    colorAttribute->setBuffer(vertexColorBuffer);

//    m_geometry->addAttribute(positionAttribute);
//    m_geometry->addAttribute(normalAttribute);
//    m_geometry->addAttribute(colorAttribute);
//    m_geometryRenderer->setGeometry(m_geometry);
//    addComponent(m_geometryRenderer);
//    addComponent(m_meshMaterial);

}

void GLModel::engoo(){
    m_meshMaterial->setAmbient(QColor(10,200,10));
}

void GLModel::exgoo(){
    m_meshMaterial->setAmbient(QColor(81,200,242));
}
void GLModel::mgoo(Qt3DRender::QPickEvent* v)
{
    qDebug() << "Moved";

    /*
    QVector3D endpoint(v->localIntersection());

    qDebug() << endpoint;
    */

    // drawLine(endpoint);
}
void GLModel::pgoo(Qt3DRender::QPickEvent* v){
    lastpoint=v->localIntersection();
}

void GLModel::rgoo(Qt3DRender::QPickEvent* v){
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
    qDebug() << "cut mode selected" << type;
    parentModel->removeCuttingPoints();
    cutMode = type;
    return;
}

void GLModel::getSliderSignal(double value){
    QVector3D v1=cuttingPoints[cuttingPoints.size()-3];
    QVector3D v2=cuttingPoints[cuttingPoints.size()-2];
    QVector3D v3=cuttingPoints[cuttingPoints.size()-1];
    QVector3D world_origin(0,0,0);
    QVector3D original_normal(0,1,0);
    QVector3D desire_normal(QVector3D::normal(v1,v2,v3)); //size=1
    float angle = qAcos(QVector3D::dotProduct(original_normal,desire_normal))*180/M_PI+(value-1)*30;
    QVector3D crossproduct_vector(QVector3D::crossProduct(original_normal,desire_normal));
    for (int i=0;i<2;i++){
        planeTransform[i]->setRotation(QQuaternion::fromAxisAndAngle(crossproduct_vector, angle+180*i));
        planeTransform[i]->setTranslation(desire_normal*(-world_origin.distanceToPlane(v1,v2,v3)));
        planeEntity[i]->addComponent(planeTransform[i]);
    }
}


/** HELPER functions **/
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

    qDebug() << "mesh specification : " << mesh->faces.size() << jump << factor;

    Mesh* newMesh = new Mesh;

    if (jump == 1){ // if mesh face count is less than 30000
        newMesh->faces.reserve(mesh->faces.size());
        newMesh->vertices.reserve(mesh->vertices.size());
        foreach (MeshFace mf, mesh->faces){
            newMesh->addFace(mesh->idx2MV(mf.mesh_vertex[0]).position, mesh->idx2MV(mf.mesh_vertex[1]).position, mesh->idx2MV(mf.mesh_vertex[2]).position);
        }
    } else {
        newMesh->faces.reserve(mesh->faces.size()/5);
        newMesh->vertices.reserve(mesh->faces.size()/5);
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
                newMesh->addFace(point1,point2,point3);
            }
            i+=1;
        }
    }

    return newMesh;
}

void GLModel::getTextChanged(QString text, int contentWidth)
{
    qDebug() << "text:" << text;
    if (labellingTextPreview)
        labellingTextPreview->setText(text, contentWidth);
}

void GLModel::openLabelling()
{
    labelingActive = true;
}

void GLModel::closeLabelling()
{
    labelingActive = false;
    if (labellingTextPreview && labellingTextPreview->isEnabled())
        labellingTextPreview->setEnabled(false);

    if (auto* glmodel = qobject_cast<GLModel*>(parent()))
        glmodel->m_meshMaterial->setDiffuse(QColor(255,255,255));
}

void GLModel::getFontNameChanged(QString fontName)
{
    if (labellingTextPreview)
        labellingTextPreview->setFontName(fontName);
}

void GLModel::generateText3DMesh()
{
    if (!labellingTextPreview)
        return;

    QVector3D* originalVertices = reinterpret_cast<QVector3D*>(vertexBuffer->data().data());
    int originalVerticesSize = vertexBuffer->data().size() / sizeof(float) / 3;

    QVector3D* vertices;
    int verticesSize;
    unsigned int* indices;
    int indicesSize;
    float depth = 1.0f;
    float scale = labellingTextPreview->ratioY * labellingTextPreview->scaleY;
    QVector3D translation = labellingTextPreview->translation+ QVector3D(0,-0.3,0);

    Qt3DCore::QTransform transform, normalTransform;
    transform.setScale(scale);
    transform.setRotationX(90);
    transform.setTranslation(translation);

    /*auto axis = QVector3D::crossProduct(QVector3D(1, 0, 0), -labellingTextPreview->normal);
    axis.normalize();
    auto cos_t = QVector3D::dotProduct(QVector3D(1, 0, 0), -labellingTextPreview->normal);
    auto sin_t = sqrtf(1 - cos_t * cos_t);
    auto angle = atan2f(cos_t, sin_t) * 180 / M_PI;
    normalTransform.setRotation(QQuaternion::fromAxisAndAngle(axis, angle + 180));*/

    normalTransform.setRotationX(90);

    generateText3DGeometry(&vertices, &verticesSize,
                           &indices, &indicesSize,
                           QFont(labellingTextPreview->fontName),
                           labellingTextPreview->text,
                           depth,
                           originalVertices,
                           originalVerticesSize, QVector3D(0, 1, 0),
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

    if (GLModel* glmodel = qobject_cast<GLModel*>(parent())) {
        glmodel->addVertices(outVertices);
        glmodel->addNormalVertices(outNormals);
    }
}
