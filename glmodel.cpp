#include <QRenderSettings>
#include "glmodel.h"
#include <QString>
#include <QtMath>
#include <typeinfo>

GLModel::GLModel(QNode *parent, QString fname, bool isShadow)
    : QEntity(parent)
    , filename(fname)
    , x(0.0f)
    , y(0.0f)
    , z(0.0f)
    , v_cnt(0)
    , f_cnt(0)
    , m_transform(new Qt3DCore::QTransform())
    //, m_objectPicker(new Qt3DRender::QObjectPicker())
    , parentEntity(reinterpret_cast<Qt3DCore::QEntity *>(parent))
    , numPoints(0)
    , curveState(false)
    , flatState(false)
{

    if (isShadow){
        mesh = new Mesh();
        loadMeshSTL(mesh, filename.toStdString().c_str());
        sparseMesh =toSparse(mesh);

        initialize(sparseMesh);
        addVertices(sparseMesh);

        addComponent(m_transform);
        //m_objectPicker->setHoverEnabled(true);
        //m_objectPicker->setDragEnabled(true);

        m_objectPicker = new Qt3DRender::QObjectPicker();
        // add only m_objectPicker
        QObject::connect(m_objectPicker, SIGNAL(clicked(Qt3DRender::QPickEvent*)), this, SLOT(handlePickerClicked(Qt3DRender::QPickEvent*)));
        QObject::connect(m_objectPicker, SIGNAL(moved(Qt3DRender::QPickEvent*)), this, SLOT(mgoo(Qt3DRender::QPickEvent*)));
        QObject::connect(m_objectPicker, SIGNAL(pressed(Qt3DRender::QPickEvent*)), this, SLOT(pgoo(Qt3DRender::QPickEvent*)));
        QObject::connect(m_objectPicker, SIGNAL(released(Qt3DRender::QPickEvent*)), this, SLOT(rgoo(Qt3DRender::QPickEvent*)));
        QObject::connect(m_objectPicker, SIGNAL(entered()), this, SLOT(engoo()));
        QObject::connect(m_objectPicker, SIGNAL(exited()), this, SLOT(exgoo()));
        addComponent(m_objectPicker);


        /*m_planeMaterial = new QPhongAlphaMaterial();
        m_planeMaterial->setAmbient(QColor(0,200,242));
        m_planeMaterial->setDiffuse(QColor(255,255,255));
        m_planeMaterial->setSpecular(QColor(81,200,242));
        m_planeMaterial->setAlpha(1.0f);
        addComponent(m_planeMaterial);*/
        return;
    }

    /*Qt3DRender::QPickingSettings *settings = new Qt3DRender::QPickingSettings(m_objectPicker);

    settings->setFaceOrientationPickingMode(Qt3DRender::QPickingSettings::FrontFace);
    settings->setPickMethod(Qt3DRender::QPickingSettings::TrianglePicking);
    settings->setPickResultMode(Qt3DRender::QPickingSettings::NearestPick);*/

    m_planeMaterial = new QPhongAlphaMaterial();
    m_planeMaterial->setAmbient(QColor(81,200,242));
    m_planeMaterial->setDiffuse(QColor(255,255,255));
    m_planeMaterial->setSpecular(QColor(81,200,242));
    m_planeMaterial->setAlpha(1.0f);

    mesh = new Mesh();
    lmesh = new Mesh();
    rmesh = new Mesh();

    loadMeshSTL(mesh, filename.toStdString().c_str());//"C:/Users/diridiri/Desktop/DLP/partial2_flip.stl");

    sparseMesh = toSparse(mesh);
    //repairMesh(mesh);

    initialize(mesh);
    addVertices(mesh);
//    initialize(mesh);
//    addVertices(mesh);



    //* Arrange
    vector<Mesh> meshes_to_arrange;
    vector<XYArrangement> arng_result_set;
    meshes_to_arrange.push_back(*mesh);
    arng_result_set = arngMeshes(&meshes_to_arrange);
    vector<Qt3DCore::QTransform*> m_transform_set;
    m_transform_set.push_back(m_transform);
    arrangeQt3D(m_transform_set, arng_result_set);
    //arrangeSingleQt3D(m_transform, &arng_result_set[0]);
    //*/



    Qt3DExtras::QDiffuseMapMaterial *diffuseMapMaterial = new Qt3DExtras::QDiffuseMapMaterial();

    //addComponent(m_mesh);
    addComponent(m_transform);
    addComponent(m_planeMaterial);

    qDebug() << "created original model";

    // create shadow model to handle picking settings
    shadowModel = new GLModel(this, filename, true);

    qDebug() << "created shadow model";

    m_autoorientation = new autoorientation();
}

orientThread::orientThread(GLModel* glmodel){
    m_glmodel=glmodel;
    m_autoorientation=glmodel->m_autoorientation;
}

void orientThread::run(){
    if(m_glmodel->appropriately_rotated){
        qDebug()<<"already oriented";
        markPopup(false);
    }else{
        markPopup(true);
        rotateResult* rotateres=this->m_autoorientation->Tweak(m_glmodel->mesh,true,45,&m_glmodel->appropriately_rotated);\
        m_glmodel->m_transform->setMatrix(rotateres->R);
    }
}

void orientThread::markPopup(bool flag){
    if(flag){
        emit orientThread::loadPopup("progress_popup");
    }else{
        emit orientThread::loadPopup("result_orient");
    }
}
Qt3DRender::QAttribute *copyAttribute(
        Qt3DRender::QAttribute *oldAtt,
        QMap<Qt3DRender::QBuffer *, Qt3DRender::QBuffer *> &bufferMap)
{
    Qt3DRender::QAttribute *newAtt = nullptr;
    if (oldAtt) {
        newAtt = new Qt3DRender::QAttribute;

        newAtt->setName(oldAtt->name());
        newAtt->setDataType(oldAtt->vertexBaseType());
        newAtt->setDataSize(oldAtt->vertexSize());
        newAtt->setCount(oldAtt->count());
        newAtt->setByteStride(oldAtt->byteStride());
        newAtt->setByteOffset(oldAtt->byteOffset());
        newAtt->setDivisor(oldAtt->divisor());
        newAtt->setAttributeType(oldAtt->attributeType());

        Qt3DRender::QBuffer *oldBuf = oldAtt->buffer();
        if (oldBuf) {
            Qt3DRender::QBuffer *newBuf = bufferMap.value(oldBuf);
            if (!newBuf) {
                newBuf = new Qt3DRender::QBuffer;
                bufferMap.insert(oldBuf, newBuf);

                if (oldBuf->data().isEmpty())
                    newBuf->setData(oldBuf->dataGenerator()->operator()());
                else
                    newBuf->setData(oldBuf->data());
                newBuf->setType(oldBuf->type());
                newBuf->setUsage(oldBuf->usage());
                newBuf->setSyncData(oldBuf->isSyncData());
            }

            newAtt->setBuffer(newBuf);
        }
    }

    return newAtt;
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

void GLModel::addVertices(Mesh* mesh)
{
    /*foreach (MeshFace mf , mesh->faces){

        vector<int> result_vs;
        for (int fn=2; fn>=0; fn--){
            result_vs.push_back(mf.mesh_vertex[fn]);
        }
        addIndexes(result_vs);
    }*/
    foreach (MeshFace mf , mesh->faces){
        vector<QVector3D> result_vs;
        for (int fn=2; fn>=0; fn--){
            result_vs.push_back(mesh->idx2MV(mf.mesh_vertex[fn]).position);
        }

        addVertices(result_vs);   
    }
    foreach (MeshFace mf , mesh->faces){

        vector<QVector3D> result_vns;
        for (int fn=2; fn>=0; fn--){
            result_vns.push_back(mesh->idx2MV(mf.mesh_vertex[fn]).vn);
        }

        addNormalVertices(result_vns);
    }
    foreach (MeshFace mf , mesh->faces){

        vector<QVector3D> result_vcs;
        for (int fn=2; fn>=0; fn--){
            result_vcs.push_back(QVector3D(1.0f, 0.0f, 0.0f));
        }

        addColorVertices(result_vcs);
    }
}

void GLModel::addVertices(vector<QVector3D> vertices){

    int vertex_cnt = vertices.size();
    QByteArray appendVertexArray;
    appendVertexArray.resize(vertex_cnt*3*sizeof(float));
    float* reVertexArray = reinterpret_cast<float*>(appendVertexArray.data());

    for (int i=0; i<vertex_cnt; i++){
        //coordinates of left vertex
        reVertexArray[i*3+0] = vertices[i].x();
        reVertexArray[i*3+1] = vertices[i].y();
        reVertexArray[i*3+2] = vertices[i].z();
    }

    uint vertexCount = positionAttribute->count();
    vertexBuffer->updateData(vertexCount*3*sizeof(float),appendVertexArray);
    positionAttribute->setCount(vertexCount + vertex_cnt);
    return;
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
    vertexNormalBuffer->updateData(vertexNormalCount*3*sizeof(float), appendVertexArray);
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

// need to create new mesh object liek Mesh* leftMesh = new Mesh();
void GLModel::bisectModel(Mesh* mesh, Plane plane, Mesh* leftMesh, Mesh* rightMesh){
    // do bisecting mesh
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

    leftMesh->connectFaces();
    rightMesh->connectFaces();
}

bool GLModel::isLeftToPlane(Plane plane, QVector3D position){
    // determine if position is left to plane or not
    if(position.distanceToPlane(plane[0],plane[1],plane[2])>0)
        return false;
    return true;
}

void GLModel::handlePickerClicked(QPickEvent *pick)
{
    QPickTriangleEvent *trianglePick = static_cast<QPickTriangleEvent*>(pick);
    qDebug() << flatState << numPoints << sizeof(sphereEntity)/4;
    if (flatState&&numPoints< sizeof(sphereEntity)/4)
        {//qDebug() << pick->localIntersection()<<"pick";
        QVector3D v = pick->localIntersection();
        addCuttingPoint(v);
    }
    else
        return;
}


void GLModel::generatePlane(){

    if (cuttingPoints.size()<3){
        qDebug()<<"Error: There is not enough vectors to render a plane.";
        QCoreApplication::quit();
        return;
    }

    QVector3D v1;
    QVector3D v2;
    QVector3D v3;
    v1=cuttingPoints[cuttingPoints.size()-3];
    v2=cuttingPoints[cuttingPoints.size()-2];
    v3=cuttingPoints[cuttingPoints.size()-1];
        planeMaterial = new Qt3DExtras::QPhongMaterial();
        planeMaterial->setDiffuse(QColor(QRgb(0x00aaaa)));
        //To manipulate plane color, change the QRgb(0x~~~~~~).
        QVector3D world_origin(0,0,0);
        QVector3D original_normal(0,1,0);
        QVector3D desire_normal(QVector3D::normal(v1,v2,v3)); //size=1
        float angle = qAcos(QVector3D::dotProduct(original_normal,desire_normal))*180/M_PI;
        QVector3D crossproduct_vector(QVector3D::crossProduct(original_normal,desire_normal));

        for (int i=0;i<2;i++){
            clipPlane[i]=new Qt3DExtras::QPlaneMesh();
                clipPlane[i]->setHeight(30.0);
                clipPlane[i]->setWidth(30.0);

            planeTransform[i]=new Qt3DCore::QTransform();
                planeTransform[i]->setScale(2.0f);
                planeTransform[i]->setRotation(QQuaternion::fromAxisAndAngle(crossproduct_vector, angle+180*i));
                planeTransform[i]->setTranslation(desire_normal*(-world_origin.distanceToPlane(v1,v2,v3)));

            planeEntity[i] = new Qt3DCore::QEntity(parentEntity);
                planeEntity[i]->addComponent(clipPlane[i]);
                planeEntity[i]->addComponent(planeTransform[i]);
                planeEntity[i]->addComponent(planeMaterial);
        }
}

void GLModel::addCuttingPoint(QVector3D v){
    cuttingPoints.push_back(v);
    sphereMesh[numPoints] = new Qt3DExtras::QSphereMesh;
    sphereMesh[numPoints]->setRadius(1);
    sphereTransform[numPoints] = new Qt3DCore::QTransform;
    sphereTransform[numPoints]->setTranslation(v);

    sphereMaterial[numPoints] = new Qt3DExtras::QPhongMaterial();
    sphereMaterial[numPoints]->setDiffuse(QColor(QRgb(0xaa0000)));
    sphereEntity[numPoints] = new Qt3DCore::QEntity(parentEntity);
    sphereEntity[numPoints]->addComponent(sphereMesh[numPoints]);
    sphereEntity[numPoints]->addComponent(sphereTransform[numPoints]);
    sphereEntity[numPoints]->addComponent(sphereMaterial[numPoints]);
    numPoints++;
}

void GLModel::removeCuttingPoints(){

    for(int i=0;i<numPoints;i++)
    {
        sphereEntity[i]->removeComponent(sphereMesh[i]);
        sphereEntity[i]->removeComponent(sphereTransform[i]);
        sphereEntity[i]->removeComponent(sphereMaterial[i]);
    }
    delete [] sphereEntity;
    delete [] sphereMaterial;
    delete [] sphereMesh;
    delete [] sphereTransform;
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
    removeComponent(m_planeMaterial);
    delete m_geometry;
    delete m_geometryRenderer;
}

void GLModel::modelcut(){

    removeCuttingPoints();
    generatePlane();
//    beforeInitialize();
////
//    Plane plane;
//    plane.push_back(cuttingPoints[cuttingPoints.size()-3]);
//    plane.push_back(cuttingPoints[cuttingPoints.size()-2]);   //plane에 잘 들어감
//    plane.push_back(cuttingPoints[cuttingPoints.size()-1]);
////
//    initialize(mesh);
//    addComponent(m_objectPicker);
//    QObject::connect(m_objectPicker, SIGNAL(clicked(Qt3DRender::QPickEvent*)), this, SLOT(handlePickerClicked(Qt3DRender::QPickEvent*)));
//    addComponent(m_transform);

//    addVertices(mesh);
//    bisectModel(mesh, plane, lmesh, rmesh);
//    addVertices(rmesh);
}


void GLModel::beforeInitialize(){
//    delete m_mesh;
    removeComponent(m_objectPicker);
    QObject::disconnect(m_objectPicker, SIGNAL(clicked(Qt3DRender::QPickEvent*)), this, SLOT(handlePickerClicked(Qt3DRender::QPickEvent*)));
    removeComponent(m_transform);
    removeComponent(m_geometryRenderer);
    removeComponent(m_planeMaterial);
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
    delete m_transform;
    delete m_objectPicker;
    delete m_planeMaterial;
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
    delete m_geometryRenderer;
}

void GLModel::beforeAddVerticies(){
    //쓰지말것
    removeComponent(m_geometryRenderer);
    removeComponent(m_planeMaterial);
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
//    addComponent(m_planeMaterial);

}

void GLModel::engoo(){
    m_planeMaterial->setAmbient(QColor(10,200,10));
}

void GLModel::exgoo(){
    m_planeMaterial->setAmbient(QColor(81,200,242));
}
void GLModel::mgoo(Qt3DRender::QPickEvent* v)
{
    QVector3D endpoint(v->localIntersection());
    drawLine(endpoint);
}
void GLModel::pgoo(Qt3DRender::QPickEvent* v){
    lastpoint=v->localIntersection();
}

void GLModel::rgoo(Qt3DRender::QPickEvent* v){
}

void GLModel::drawLine(QVector3D endpoint)
{
    if (curveState){
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
        Qt3DCore::QEntity* lineEntity = new Qt3DCore::QEntity(parentEntity);
        lineEntity->addComponent(line);
        lineEntity->addComponent(lineTransform);
        lineEntity->addComponent(lineMaterial);
        lastpoint=endpoint;
    }
}

void GLModel::lineAccept(){
    qDebug() << "line Accept";
    if(!curveState){
        curveState=true;}
    else {
        curveState=false;}
}

void GLModel::pointAccept(){
    qDebug() << "point Accept";
    if(!flatState){
        flatState=true;}
    else {
        removeCuttingPoints();
        flatState=false;}
}

void GLModel::getSignal(double value){
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

QVector3D GLModel::spreadPoint(QVector3D endPoint,QVector3D startPoint,int factor){
    QVector3D standardVector = endPoint-startPoint;
    QVector3D finalVector=endPoint+standardVector*(factor-1);
    return finalVector;
}


Mesh* GLModel::toSparse(Mesh* mesh){
    int i=0, jump=30, factor=10;
    Mesh* newMesh = new Mesh;
    foreach (MeshFace mf , mesh->faces){
        if (i%jump==0){
            QVector3D point1 =mesh->idx2MV(mf.mesh_vertex[0]).position;
            QVector3D point2 =mesh->idx2MV(mf.mesh_vertex[1]).position;
            QVector3D point3 =mesh->idx2MV(mf.mesh_vertex[2]).position;
            QVector3D CenterOfMass = (point1+point2+point3)/3;
            point1=GLModel::spreadPoint(point1,CenterOfMass,factor);
            point2=GLModel::spreadPoint(point2,CenterOfMass,factor);
            point3=GLModel::spreadPoint(point3,CenterOfMass,factor);
            newMesh->addFace(mesh->idx2MV(mf.mesh_vertex[0]).position, mesh->idx2MV(mf.mesh_vertex[1]).position, mesh->idx2MV(mf.mesh_vertex[2]).position);
            newMesh->addFace(point1,point2,point3);
        }
        i+=1;
    }
return newMesh;
}
