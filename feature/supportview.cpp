#include "supportview.h"
#include "earcut.hpp"
#include <QtMath>

void generateCylinder(Mesh* mesh, const QVector3D& position, const QVector3D& positionTop, const float& radius) {
    qDebug() << "v1(" << position.x() << "," << position.y() << "," << position.z() << ")";
    qDebug() << "v2(" << positionTop.x() << "," << positionTop.y() << "," << positionTop.z() << ")";

    vector<QVector3D> top;
    vector<QVector3D> bottom;
    for( float i = 0.0f ; i <= 360.0f ; i += 10.0f ) {
        float t = i / 180.0f * M_PI;
        top.push_back(QVector3D(qCos(t) * radius + positionTop.x(), qSin(t) * radius + positionTop.y(), positionTop.z()));
        bottom.push_back(QVector3D(qCos(t) * radius + position.x(), qSin(t) * radius + position.y(), position.z()));
    }

    // top circle
    QVector3D *lastTop;
    for( auto iter = top.begin() ; iter != top.end() ; iter++ ) {
        if( iter != top.begin() ) {
            mesh->addFace(*lastTop, (*iter), positionTop);
        }
        lastTop = &(*iter);
    }

    // bottom circle
    QVector3D *lastBottom;
    for( auto iter = bottom.begin() ; iter != bottom.end() ; iter++ ) {
        if( iter != bottom.begin() ) {
            mesh->addFace((*iter), *lastBottom, position);
        }
        lastBottom = &(*iter);
    }

    // pillar
    for( auto iterTop = top.begin(), iterBottom = bottom.begin(); iterTop != top.end() ; iterTop++, iterBottom++ ) {
        if( iterTop != top.begin() ) {
            mesh->addFace((*iterTop), *lastTop, (*iterBottom));
            mesh->addFace(*lastTop, *lastBottom, (*iterBottom));
        }
        lastTop = &(*iterTop);
        lastBottom = &(*iterBottom);
    }
}

void generateFace(Mesh* mesh, Path path, float z) {
    // Create array
    std::vector<std::vector<std::array<float, 2>>> polygon;
    std::vector<std::array<float, 2>> points;
    for( auto point : path ) {
        points.push_back({(float)point.X / scfg->resolution, (float)point.Y / scfg->resolution});
    }
    polygon.push_back(points);
    // Run tessellation
    // Returns array of indices that refer to the vertices of the input polygon.
    // e.g: the index 6 would refer to {25, 75} in this example.
    // Three subsequent indices form a triangle. Output triangles are clockwise.
    std::vector<uint32_t> indices = mapbox::earcut<uint32_t>(polygon);
    for (auto iter = indices.begin() ; iter != indices.end() ; ) {
        int n1 = (*iter); iter++;
        int n2 = (*iter); iter++;
        int n3 = (*iter); iter++;
        mesh->addFace(QVector3D(points.at(n3).at(0), points.at(n3).at(1), z),
                      QVector3D(points.at(n2).at(0), points.at(n2).at(1), z),
                      QVector3D(points.at(n1).at(0), points.at(n1).at(1), z));
    }
}

void generateSupporter(Mesh* mesh, OverhangPoint *point, OverhangPoint *parent, vector<OverhangPoint *> *points) {

    qDebug() << point << point->target_branching_overhang_point << parent;
    qDebug() << "height:" << point->height <<
                "radius:" << point->radius <<
                "branchable:" << point->branchable <<
                "v(" << point->position.X << "," << point->position.Y << "," << point->position.Z << ")";

    float radius = (float)point->radius / scfg->resolution;
    QVector3D position = QVector3D((float)point->position.X / scfg->resolution,
            (float)point->position.Y / scfg->resolution,
            (float)(point->position.Z - (int)point->height * 1000) / scfg->resolution);
    QVector3D positionTop = QVector3D((float)point->position.X / scfg->resolution,
            (float)point->position.Y / scfg->resolution,
            (float)point->position.Z / scfg->resolution);

    if( parent != nullptr ) {
        if( parent->position.Z > point->position.Z ) {
            position = QVector3D((float)point->position.X / scfg->resolution,
                    (float)point->position.Y / scfg->resolution,
                    (float)point->position.Z / scfg->resolution);
            positionTop = QVector3D((float)parent->position.X / scfg->resolution,
                    (float)parent->position.Y / scfg->resolution,
                    (float)parent->position.Z / scfg->resolution);
        } else {
            position = QVector3D((float)parent->position.X / scfg->resolution,
                    (float)parent->position.Y / scfg->resolution,
                    (float)parent->position.Z / scfg->resolution);
            positionTop = QVector3D((float)point->position.X / scfg->resolution,
                    (float)point->position.Y / scfg->resolution,
                    (float)point->position.Z / scfg->resolution);
        }
    }

    generateCylinder(mesh, position, positionTop, radius);

    if( point->target_branching_overhang_point != nullptr ) {

        if( parent == nullptr ) {
            points = new vector<OverhangPoint *>();
        }

        points->push_back(point);

        if( std::find(points->begin(), points->end(), point) == points->end() ) {
            generateSupporter(mesh, point->target_branching_overhang_point, point, points);
        }

        if( parent == nullptr ) {
            delete points;
        }
    }
}

void generateRaft(Mesh* mesh, OverhangPoint *point) {
    float bottom = (float)(point->position.Z - (int)point->height * 1000) / scfg->resolution;
    float radius = (float)scfg->raft_offset_radius / (float)scfg->resolution;
    QVector3D position = QVector3D((float)point->position.X / scfg->resolution,
            (float)point->position.Y / scfg->resolution,
            bottom - scfg->raft_thickness);
    QVector3D positionTop = QVector3D((float)point->position.X / scfg->resolution,
            (float)point->position.Y / scfg->resolution,
            bottom);

    generateCylinder(mesh, position, positionTop, radius);
}

void generateInfill(Mesh* mesh, Slice* slice) {
    for( auto iter = slice->outershell.begin() ; iter != slice->outershell.end() ; iter++ ) {
        generateFace(mesh, (*iter), slice->z);
    }
}
