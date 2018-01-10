
#include "glmodel.h"
#include <QString>

GLModel::GLModel(QNode *parent)
    : QEntity(parent)
    , x(0.0f)
    , y(0.0f)
    , z(0.0f)
    , v_cnt(0)
    , f_cnt(0)
    , m_mesh(new Qt3DRender::QMesh())
    , m_transform(new Qt3DCore::QTransform())
    , m_objectPicker(new Qt3DRender::QObjectPicker())
    , motherEntity(reinterpret_cast<Qt3DCore::QEntity *>(parent))


{
    Qt3DRender::QPickingSettings *settings = new Qt3DRender::QPickingSettings(m_objectPicker);

    settings->setFaceOrientationPickingMode(Qt3DRender::QPickingSettings::FrontFace);
    settings->setPickMethod(Qt3DRender::QPickingSettings::TrianglePicking);
    settings->setPickResultMode(Qt3DRender::QPickingSettings::NearestPick);

    /*Qt3DRender::QRenderSettings *settings = (Qt3DRender::QRenderSettings*) QRenderSettings::activeFrameGraph()->parent();
    if (settings)
        settings->pickingSettings()->setPickMethod(Qt3DRender::QPickingSettings::TrianglePicking);*/


    m_planeMaterial = new QPhongAlphaMaterial();
    m_planeMaterial->setAmbient(QColor(81,200,242));
    m_planeMaterial->setDiffuse(QColor(255,255,255));
    m_planeMaterial->setAlpha(1.0f);



    //m_parent = parent;

    QTimer *timer = new QTimer();
    QObject::connect(timer, &QTimer::timeout,this,&GLModel::onTimerUpdate);

  //  connect(m_objectPicker,SIGNAL(clicked(Qt3DRender::QPickEvent*)),this,SLOT(addUserPoint(Qt3DRender::QPickEvent*)));

    mesh = new Mesh();
    qDebug() << "loading mesh";
    loadMeshSTL(mesh, "C:/Users/diridiri/Desktop/DLP/partial2_flip.stl");


    qDebug() << "loaded mesh";
    initialize(mesh);
    m_mesh->setGeometry(m_geometry);



//    Plane plane;
//    plane.push_back(QVector3D(7,0,0));
//    plane.push_back(QVector3D(0,7,0));
//    plane.push_back(QVector3D(0,0,7));

    lmesh = new Mesh();
    rmesh = new Mesh();

//    bisectModel(mesh, plane, lmesh, rmesh);
    addVertices(mesh);


    //m_mesh->setSource(QUrl(QStringLiteral("file:///D:/Dev/DLPSlicer/DLPslicer/resource/mesh/lowerjaw.obj")));

    Qt3DExtras::QDiffuseMapMaterial *diffuseMapMaterial = new Qt3DExtras::QDiffuseMapMaterial();
//    diffuseMapMaterial->setAmbient(QColor(QRgb(0xdd0000)));
//    diffuseMapMaterial->setSpecular(QColor::fromRgbF(0.9f, 0.9f, 0.9f, 1.0f));
////    diffuseMapMaterial->setShininess(10.0f);
////    Qt3DExtras::QPhongMaterial *phongMaterial = new Qt3DExtras::QPhongMaterial();
//    phongMaterial->setAmbiant(QColor(QRgb(0x92bcc4)));
//    phongMaterial->setDiffuse(QColor(QRgb(0x92bcc4)));

    addComponent(m_mesh);
//    addComponent(diffuseMapMaterial);
    //addComponent(phongMaterial);
    addComponent(m_transform);

    m_objectPicker->setHoverEnabled(true);
    QObject::connect(m_objectPicker, SIGNAL(clicked(Qt3DRender::QPickEvent*)), this, SLOT(handlePickerClicked(Qt3DRender::QPickEvent*)));

    addComponent(m_objectPicker);

    auto attributes = m_geometry->attributes();

    int vertexIndex = 0;

    qDebug() << "attributes size : " << attributes.count();
    for (auto i = 0; i < attributes.count(); ++i)
    {
        if (attributes.at(i)->name() == QAttribute::VertexAttribute)//QAttribute::defaultPositionAttributeName())
        {
            /*QAttribute *attribute = attributes.at(i);
            Qt3DRender::QBuffer *buffer = attribute->buffer();
            const QByteArray &data = buffer->data();

            int vertexOffset = vertexIndex * attribute->byteStride();
            int offset = vertexOffset + attribute->byteOffset();

            char *rawData = (char*)&(data.constData()[offset]);

            // replace float with your data type
            float *value = reinterpret_cast<float*>(rawData);
            qDebug() << value;
            break;*/
        }
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



void GLModel::initialize(Mesh* mesh){

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
    addComponent(m_planeMaterial);




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
    //if (QVector3D::dotProduct(QVector3D::normal(plane[0], plane[1], plane[2]), position)>0)
    if(position.distanceToPlane(plane[0],plane[1],plane[2])>0)
        return false;
    return true;
}

void GLModel::onTimerUpdate()
{
    /*x += 1.0f;
    y += 1.0f;
    z += 1.0f;
    addVertex(QVector3D(x,y,z));*/
}

//void GLModel::addUserPoint(QPickEvent *pick){
//plane.push_back(pick->worldIntersection());
//}

void GLModel::handlePickerClicked(QPickEvent *pick)
{
    QPickTriangleEvent *trianglePick = static_cast<QPickTriangleEvent*>(pick);

    qDebug() << trianglePick->localIntersection()<<"aaa";
    vector_set.push_back(trianglePick->localIntersection());

    QGeometry *geometry = m_mesh->geometry();
    auto attributes = geometry->attributes();
    int vertexIndex = trianglePick->vertex1Index();
    for (auto i = 0; i < attributes.count(); ++i)
    {
  //      qDebug() << i;
        if (attributes.at(i)->name() == QAttribute::defaultPositionAttributeName())
        {
            QAttribute *attribute = attributes.at(i);
            Qt3DRender::QBuffer *buffer = attribute->buffer();
            const QByteArray &data = buffer->data();
 
            int vertexOffset = vertexIndex * attribute->byteStride();
            int offset = vertexOffset + attribute->byteOffset();

            char *rawData = (char*)&(data.constData()[offset]);

            // replace float with your data type
            float *value = reinterpret_cast<float*>(rawData);

            break;
        }
    }
}


void GLModel::makePlane(){

    if (vector_set.size()<3){
        qDebug()<<"Error: There is not enough vectors to render a plane.";
        QCoreApplication::quit();
        return;
    }


    QVector3D v1;
    QVector3D v2;
    QVector3D v3;
    v1=vector_set[vector_set.size()-3];
    v2=vector_set[vector_set.size()-2];
    v3=vector_set[vector_set.size()-1];

        planeMaterial = new Qt3DExtras::QPhongMaterial();
            planeMaterial->setDiffuse(QColor(QRgb(0x00aaaa)));
            //To manipulate plane color, change the QRgb(0x~~~~~~).
        QVector3D world_origin(0,0,0);
        QVector3D original_normal(0,1,0);
        QVector3D desire_normal(QVector3D::normal(v1,v2,v3)); //size=1
        float angle = qAcos(QVector3D::dotProduct(original_normal,desire_normal))*180/M_PI;
        QVector3D crossproduct_vector(QVector3D::crossProduct(original_normal,desire_normal));

        for (int i=0;i<2;i++){
            ClipPlane[i]=new Qt3DExtras::QPlaneMesh();
                ClipPlane[i]->setHeight(30.0);
                ClipPlane[i]->setWidth(30.0);

            planeTransform[i]=new Qt3DCore::QTransform();
                planeTransform[i]->setScale(2.0f);
                planeTransform[i]->setRotation(QQuaternion::fromAxisAndAngle(crossproduct_vector, angle+180*i));
                planeTransform[i]->setTranslation(desire_normal*(-world_origin.distanceToPlane(v1,v2,v3)));


                qDebug()<<"vector:"<<desire_normal;
                qDebug()<<"factor:"<<world_origin.distanceToPlane(v1,v2,v3);
                qDebug()<<"output:"<<desire_normal*world_origin.distanceToPlane(v1,v2,v3);



            planeEntity[i] = new Qt3DCore::QEntity(motherEntity);
                planeEntity[i]->addComponent(ClipPlane[i]);
                planeEntity[i]->addComponent(planeTransform[i]);
                planeEntity[i]->addComponent(planeMaterial);
        }

}

void GLModel::delModel(){
    delete positionAttribute;
    delete normalAttribute;
    delete colorAttribute;
    delete vertexBuffer;
    delete vertexNormalBuffer;
    delete vertexColorBuffer;
    delete m_geometry;
    delete m_geometryRenderer;
}

void GLModel::modelcut(){
    makePlane();
//    delModel();
    before_initiate();

    Plane plane;
    plane.push_back(vector_set[vector_set.size()-3]);
    plane.push_back(vector_set[vector_set.size()-2]);
    plane.push_back(vector_set[vector_set.size()-1]);
    bisectModel(mesh, plane, lmesh, rmesh);

    initialize(rmesh);
    delete mesh;
    delete lmesh;
    //m_mesh->setGeometry(m_geometry);
    addVertices(rmesh);
    /*qDebug()<<"dddd";
    addComponent(m_mesh);*/

}


void GLModel::before_initiate(){


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


