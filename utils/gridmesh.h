#ifndef GRIDMESH_H
#define GRIDMESH_H

#include <QObject>
#include <QNode>
#include <QGeometryRenderer>
#include <QSize>

class GridMesh : public Qt3DRender::QGeometryRenderer
{
    Q_OBJECT
    Q_PROPERTY(QSize meshResolution READ meshResolution WRITE setMeshResolution NOTIFY meshResolutionChanged)
public:
    explicit GridMesh(Qt3DCore::QNode *parent = nullptr);
    ~GridMesh() = default;

    QSize meshResolution() const;

public slots:
    void setMeshResolution(const QSize &meshResolution);

signals:
    void meshResolutionChanged(const QSize &meshResolution);

private:
    QSize m_meshResolution;
};

#endif // GRIDMESH_H
