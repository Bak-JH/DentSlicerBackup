#include "glmodel.h"



GLModel::GLModel(QNode *parent)
    : QEntity(parent)
    , x(0.0f)
    , y(0.0f)
    , z(0.0f)
    , m_count(0)
{
    m_planeMaterial = new QPhongAlphaMaterial();
    m_planeMaterial->setAmbient(QColor(100,100,100));
    m_planeMaterial->setDiffuse(QColor(255,255,255));
    m_planeMaterial->setAlpha(1.0f);

    m_parent = parent;

    QTimer *timer = new QTimer();
    QObject::connect(timer, &QTimer::timeout,this,&GLModel::onTimerUpdate);

    timer->start(100);
}


void GLModel::addPoint(){
    float* reVertexArray;

    //x += 5.0f;
    //y += 3.0f;
    x += 0.01f;
    y -= (qrand() % 2 - 1)*0.01;
    z = 0.01f;

    if (!(m_count % 100)) {
        //every 100-th timer tick:
        m_geometryRenderer = new QGeometryRenderer();
        QGeometry* meshGeometry = new QGeometry(m_geometryRenderer);

        QByteArray vertexArray;
        vertexArray.resize(200*3*sizeof(float));
        reVertexArray = reinterpret_cast<float*>(vertexArray.data());

        //coordinates of left vertex
        reVertexArray[0] = y-0.1f;
        reVertexArray[1] = z;
        reVertexArray[2] = x;

        //coordinates of right vertex
        reVertexArray[3] = y+0.1f;
        reVertexArray[4] = z;
        reVertexArray[5] = x;

        vertexBuffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer,meshGeometry);
        vertexBuffer->setUsage(Qt3DRender::QBuffer::DynamicDraw);
        vertexBuffer->setData(vertexArray);

        // Attributes
        positionAttribute = new QAttribute(meshGeometry);
        positionAttribute->setAttributeType(QAttribute::VertexAttribute);
        positionAttribute->setBuffer(vertexBuffer);
        positionAttribute->setDataType(QAttribute::Float);
        positionAttribute->setDataSize(3);
        positionAttribute->setByteOffset(0);
        positionAttribute->setByteStride(3*sizeof(float));
        positionAttribute->setCount(2);
        positionAttribute->setName(QAttribute::defaultPositionAttributeName());

        meshGeometry->addAttribute(positionAttribute);

        m_geometryRenderer->setInstanceCount(1);
        m_geometryRenderer->setFirstVertex(0);
        m_geometryRenderer->setFirstInstance(0);
        m_geometryRenderer->setPrimitiveType(QGeometryRenderer::TriangleStrip);
        m_geometryRenderer->setGeometry(meshGeometry);

        QEntity* entity = new QEntity(this);
        entity->addComponent(m_geometryRenderer);
        entity->addComponent(m_planeMaterial);

        return;
    }


    //update geometry
    QByteArray appendVertexArray;
    appendVertexArray.resize(2*3*sizeof(float));
    reVertexArray = reinterpret_cast<float*>(appendVertexArray.data());

    //coordinates of left vertex
    reVertexArray[0] = y-0.1f;
    reVertexArray[1] = z;
    reVertexArray[2] = x;

    //coordinates of right vertex
    reVertexArray[3] = y+0.1f;
    reVertexArray[4] = z+0.1f;
    reVertexArray[5] = x;

    uint vertexCount = positionAttribute->count();
    vertexBuffer->updateData(vertexCount*3*sizeof(float),appendVertexArray);
    positionAttribute->setCount(vertexCount+2);
}

void GLModel::onTimerUpdate()
{
    addPoint();

    qDebug() << m_count;
    //qDebug() << "x=" << x << "; y=" << y;
    //qDebug() << m_parent;

    m_count ++;
}

/*
#include <QQuickWindow>
#include <QOpenGLShaderProgram>
#include <QOpenGLContext>

static const char *vertexShaderSourceCore =
    "#version 150\n"
    "in vec4 vertex;\n"
    "in vec3 normal;\n"
    "out vec3 vert;\n"
    "out vec3 vertNormal;\n"
    "uniform mat4 projMatrix;\n"
    "uniform mat4 mvMatrix;\n"
    "uniform mat3 normalMatrix;\n"
    "void main() {\n"
    "   vert = vertex.xyz;\n"
    "   vertNormal = normalMatrix * normal;\n"
    "   gl_Position = projMatrix * mvMatrix * vertex;\n"
    "}\n";

static const char *fragmentShaderSourceCore =
    "#version 150\n"
    "in highp vec3 vert;\n"
    "in highp vec3 vertNormal;\n"
    "out highp vec4 fragColor;\n"
    "uniform highp vec3 lightPos;\n"
    "void main() {\n"
    "   highp vec3 L = normalize(lightPos - vert);\n"
    "   highp float NL = max(dot(normalize(vertNormal), L), 0.0);\n"
    "   highp vec3 color = vec3(0.39, 1.0, 0.0);\n"
    "   highp vec3 col = clamp(color * 0.2 + color * 0.8 * NL, 0.0, 1.0);\n"
    "   fragColor = vec4(col, 1.0);\n"
    "}\n";

static const char *vertexShaderSource =
    "attribute vec4 vertex;\n"
    "attribute vec3 normal;\n"
    "varying vec3 vert;\n"
    "varying vec3 vertNormal;\n"
    "uniform mat4 projMatrix;\n"
    "uniform mat4 mvMatrix;\n"
    "uniform mat3 normalMatrix;\n"
    "void main() {\n"
    "   vert = vertex.xyz;\n"
    "   vertNormal = normalMatrix * normal;\n"
    "   gl_Position = projMatrix * mvMatrix * vertex;\n"
    "}\n";

static const char *fragmentShaderSource =
    "varying highp vec3 vert;\n"
    "varying highp vec3 vertNormal;\n"
    "uniform highp vec3 lightPos;\n"
    "void main() {\n"
    "   highp vec3 L = normalize(lightPos - vert);\n"
    "   highp float NL = max(dot(normalize(vertNormal), L), 0.0);\n"
    "   highp vec3 color = vec3(0.39, 1.0, 0.0);\n"
    "   highp vec3 col = clamp(color * 0.2 + color * 0.8 * NL, 0.0, 1.0);\n"
    "   gl_FragColor = vec4(col, 1.0);\n"
    "}\n";


GlModel::GlModel()
    :m_t(0)
    , m_renderer(0)
{
    connect(this, &QQuickItem::windowChanged, this, &GlModel::handleWindowChanged);
}

void GlModel::handleWindowChanged(QQuickWindow *win)
{
    if (win) {
        connect(win, &QQuickWindow::beforeSynchronizing, this, &GlModel::sync, Qt::DirectConnection);
        connect(win, &QQuickWindow::sceneGraphInvalidated, this, &GlModel::cleanup, Qt::DirectConnection);
        win->setClearBeforeRendering(false);
   }
}

void GlModel::sync()
{
    if (!m_renderer) {
        m_renderer = new GlModelRenderer();
        connect(window(), &QQuickWindow::beforeRendering, m_renderer, &GlModelRenderer::paint, Qt::DirectConnection);
    }
    m_renderer->setViewportSize(window()->size() * window()->devicePixelRatio());
    m_renderer->setT(m_t);
    m_renderer->setWindow(window());
}

void GlModel::cleanup()
{
    if (m_renderer) {
        delete m_renderer;
        m_renderer = 0;
    }
}

GlModelRenderer::GlModelRenderer() : m_t(0), m_program(0){
    m_core = QCoreApplication::arguments().contains(QStringLiteral("--coreprofile"));
    // --transparent causes the clear color to be transparent. Therefore, on systems that
    // support it, the widget will become transparent apart from the logo.
    m_transparent = QCoreApplication::arguments().contains(QStringLiteral("--transparent"));
    if (m_transparent) {
        QSurfaceFormat fmt = format();
        fmt.setAlphaBufferSize(8);
        setFormat(fmt);
    }
}

GlModelRenderer::~GlModelRenderer()
{
    delete m_program;
}

void GlModel::setT(qreal t)
{
    if (t == m_t)
        return;
    m_t = t;
    emit tChanged();
    if (window())
        window()->update();
}

void GlModelRenderer::paint()
{
    if (!m_program) {
        initializeOpenGLFunctions();

        m_program = new QOpenGLShaderProgram();
        m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, m_core ? vertexShaderSourceCore : vertexShaderSource);
        m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, m_core ? fragmentShaderSourceCore : fragmentShaderSource);
        m_program->bindAttributeLocation("vertex", 0);
        m_program->bindAttributeLocation("normal", 1);
        m_program->link();

    }

    m_program->bind();

    m_program->enableAttributeArray(0);

    float values[] = {
        -1, -1,
        1, -1,
        -1, 1,
        1, 1
    };
    m_program->setAttributeArray(0, GL_FLOAT, values, 2);
    m_program->setUniformValue("t", (float) m_t);


    m_vao.create();

    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);

    // Setup our vertex buffer object.
    m_textVbo.create();
    m_textVbo.bind();
    m_textVbo.allocate(m_text3D.constData(), m_text3D.count() * sizeof(GLfloat));


    glViewport(0, 0, m_viewportSize.width(), m_viewportSize.height());

    glDisable(GL_DEPTH_TEST);

    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    //glDrawArrays(GL_TRIANGLE_FAN, , );
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    m_program->disableAttributeArray(0);
    m_program->release();

    // Not strictly needed for this example, but generally useful for when
    // mixing with raw OpenGL.
    m_window->resetOpenGLState();
}

void GlModelRenderer::initializeGL(){

}

void GlModelRenderer::paintGL(){

}

void GlModelRenderer::resizeGL(int width, int height){

}

void GlModelRenderer::mousePressEvent(QMouseEvent * event){

}

void GlModelRenderer::mouseMoveEvent(QMouseEvent * event){

}

*/
