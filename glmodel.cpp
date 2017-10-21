#include "glmodel.h"

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


