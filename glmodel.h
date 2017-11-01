#ifndef GLMODEL_H
#define GLMODEL_H

#include <QObject>
#include <Qt3DCore>
#include <Qt3DRender>
#include <Qt3DExtras>
#include <Qt3DInput>
#include <vector>
#include "fileloader.h"

#define MAX_BUF_LEN 100000

using namespace Qt3DCore;
using namespace Qt3DRender;
using namespace Qt3DExtras;
using namespace std;

class GLModel : public QEntity
{
    Q_OBJECT
public:
    GLModel(QNode* parent = nullptr);


private:

    QPhongAlphaMaterial *m_planeMaterial;
    Qt3DRender::QBuffer *vertexBuffer;
    QAttribute *positionAttribute;
    QGeometry* m_geometry;
    QGeometryRenderer* m_geometryRenderer;

    QString filename;
    float x,y,z;
    int v_cnt;
    int f_cnt;

    Mesh* mesh;
    QNode* m_parent;

    Qt3DRender::QMesh *m_mesh;
    Qt3DCore::QTransform *m_transform;

    void initialize();
    void addVertex(QVector3D vertex);
    void addVertices(vector<MeshVertex> vertices);
    void onTimerUpdate();
};


/*
#include <QQuickItem>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLWidget>
#include "text3d.h"

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)

class GlModelRenderer : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    GlModelRenderer();
    ~GlModelRenderer();

    void setT(qreal t) { m_t = t; }
    void setViewportSize(const QSize &size) { m_viewportSize = size; }
    void setWindow(QQuickWindow *window) { m_window = window; }

public slots:
    void paint();

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    QSize m_viewportSize;
    qreal m_t;
    QOpenGLShaderProgram *m_program;
    QQuickWindow *m_window;
    QOpenGLVertexArrayObject m_vao;
    QOpenGLBuffer m_textVbo;
    text3D m_text3D;
    bool m_transparent;
    bool m_core;
};

class GlModel : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(qreal t READ t WRITE setT NOTIFY tChanged)

public:
    GlModel();

    qreal t() const { return m_t; }
    void setT(qreal t);

signals:
    void tChanged();

public slots:
    void sync();
    void cleanup();

private slots:
    void handleWindowChanged(QQuickWindow *win);

private:
    qreal m_t;
    GlModelRenderer *m_renderer;

};*/


#endif // GLMODEL_H
