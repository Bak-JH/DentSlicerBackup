#ifndef GLMODEL_H
#define GLMODEL_H
#include <QQuickItem>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>

class GlModelRenderer : public QObject, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    GlModelRenderer() : m_t(0), m_program(0) {}
    ~GlModelRenderer();

    void setT(qreal t) { m_t = t; }
    void setViewportSize(const QSize &size) { m_viewportSize = size; }
    void setWindow(QQuickWindow *window) { m_window = window; }

public slots:
    void paint();

private:
    QSize m_viewportSize;
    qreal m_t;
    QOpenGLShaderProgram *m_program;
    QQuickWindow *m_window;
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

};


#endif // GLMODEL_H
