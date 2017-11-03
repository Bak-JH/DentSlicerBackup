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
{


    m_planeMaterial = new QPhongAlphaMaterial();
    m_planeMaterial->setAmbient(QColor(81,200,242));
    m_planeMaterial->setDiffuse(QColor(255,255,255));
    m_planeMaterial->setAlpha(1.0f);

    m_parent = parent;

    QTimer *timer = new QTimer();
    QObject::connect(timer, &QTimer::timeout,this,&GLModel::onTimerUpdate);

    /*mesh = new Mesh();
    loadMeshSTL(mesh, "C:/Users/diridiri/Desktop/DLP/partial1.stl");
    initialize();
    addVertices();*/
    //addVertices(mesh->vertices);

    //m_mesh->setGeometry(m_geometry);
    /*m_mesh->setSource(QUrl(QStringLiteral("file:///D:/Dev/DLPSlicer/DLPslicer/resource/mesh/lowerjaw.obj")));
    Qt3DExtras::QDiffuseMapMaterial *diffuseMapMaterial = new Qt3DExtras::QDiffuseMapMaterial();
    diffuseMapMaterial->setSpecular(QColor::fromRgbF(0.2f, 0.2f, 0.2f, 1.0f));
    diffuseMapMaterial->setShininess(2.0f);

    addComponent(diffuseMapMaterial);

    Qt3DExtras::QPhongMaterial *phongMaterial = new Qt3DExtras::QPhongMaterial();
    phongMaterial->setDiffuse(QColor(204, 205, 75)); // Safari Yellow #cccd4b
    phongMaterial->setSpecular(Qt::white);

    addComponent(phongMaterial);
    addComponent(m_mesh);
    addComponent(m_transform);*/
    //m_mesh->setGeometry(m_geometry);
    //m_mesh->geometry()

    //qDebug() << "done loading mesh";
    /*ModelLoader * mi = new ModelLoader(this);
    mi->loadModel("C:/Users/diridiri/Desktop/DLP/partial1.stl");

    //timer->start(20000);*/

}

void GLModel::initialize(){
    float* reVertexArray;
    m_geometryRenderer = new QGeometryRenderer();
    m_geometry = new QGeometry(m_geometryRenderer);
    QByteArray vertexArray;
    vertexArray.resize(mesh->vertices.size()*5*3*3*sizeof(float));
    reVertexArray = reinterpret_cast<float*>(vertexArray.data());

    vertexBuffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer,m_geometry);
    vertexBuffer->setUsage(Qt3DRender::QBuffer::DynamicDraw);
    vertexBuffer->setData(vertexArray);

    // Attributes
    positionAttribute = new QAttribute(m_geometry);
    positionAttribute->setAttributeType(QAttribute::VertexAttribute);
    positionAttribute->setBuffer(vertexBuffer);
    positionAttribute->setDataType(QAttribute::Float);
    positionAttribute->setDataSize(3);
    positionAttribute->setByteOffset(0);
    positionAttribute->setByteStride(3*sizeof(float));
    positionAttribute->setCount(2);
    positionAttribute->setName(QAttribute::defaultPositionAttributeName());

    m_geometry->addAttribute(positionAttribute);

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
    vertexBuffer->updateData(vertexCount*3*sizeof(float),appendVertexArray);
    positionAttribute->setCount(vertexCount+1);

    return;
}

void GLModel::addVertices(){
    foreach (MeshFace mf , mesh->faces){
        /*vector<MeshVertex> temp_face;
        for (int fn=0; fn<3; fn++){
            temp_face.push_back(mesh->vertices[mf.mesh_vertex[fn]].position);
        }
        //addVertices(temp_face);
        QVector3D norm_v = QVector3D::normal(temp_face[0].position, temp_face[1].position, temp_face[2].position);
        vector<QVector3D> result_vs;
        for (int i=0; i<3; i++){
            result_vs.push_back(temp_face[i].position);
        }
        result_vs.push_back(norm_v);*/

        vector<QVector3D> result_vs;
        for (int fn=0; fn<3; fn++){
            result_vs.push_back(mesh->vertices[mf.mesh_vertex[fn]].position);
        }
        //addVertices(temp_face);
        QVector3D norm_v = QVector3D::normal(result_vs[0], result_vs[1], result_vs[2]);

        result_vs.push_back(norm_v);
        addVertices(result_vs);
    }
}

void GLModel::addVertices(vector<QVector3D> vertices){

    int vertex_cnt = vertices.size();
    //update geometry
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
    positionAttribute->setCount(vertexCount+vertex_cnt);
    return;
}


void GLModel::onTimerUpdate()
{
    /*x += 1.0f;
    y += 1.0f;
    z += 1.0f;
    addVertex(QVector3D(x,y,z));*/
}



