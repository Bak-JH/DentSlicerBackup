#include "manualsupport.h"
#include "earcut.hpp"
#include <QtMath>
using namespace Hix::Engine3D;
void generateCustomCylinder(Mesh* mesh, const QVector3D& position, const QVector3D& positionTop, const float& radiusTop, const float& radiusBottom){
    //qDebug() << "generateCustomCylinder";
    qDebug() << "v1(" << position.x() << "," << position.y() << "," << position.z() << ")";
    qDebug() << "v2(" << positionTop.x() << "," << positionTop.y() << "," << positionTop.z() << ")";

    std::vector<QVector3D> top;
    std::vector<QVector3D> bottom;
    for( double i = 0.0 ; i <= 360.0 ; i += 10.0 ) {
        double t = i / 180.0 * M_PI;
        top.push_back(QVector3D(qCos(t) * radiusTop + positionTop.x(), qSin(t) * radiusTop + positionTop.y(), positionTop.z()));
        bottom.push_back(QVector3D(qCos(t) * radiusBottom + position.x(), qSin(t) * radiusBottom + position.y(), position.z()));
    }

    // top circle
    QVector3D *lastTop;
    for( auto iter = top.begin() ; iter != top.end() ; iter++ ) {
        if( iter != top.begin() ) {
            mesh->addFace(*lastTop, *iter, positionTop);
        }
        lastTop = &(*iter);
    }

    // bottom circle
    QVector3D *lastBottom;
    for( auto iter = bottom.begin() ; iter != bottom.end() ; iter++ ) {
        if( iter != bottom.begin() ) {
            mesh->addFace(*iter, *lastBottom, position);
        }
        lastBottom = &(*iter);
    }

    // pillar
    for( auto iterTop = top.begin(), iterBottom = bottom.begin(); iterTop != top.end() ; iterTop++, iterBottom++ ) {
        if( iterTop != top.begin() ) {
            mesh->addFace(*iterTop, *lastTop, *iterBottom);
            mesh->addFace(*lastTop, *lastBottom, *iterBottom);
        }
        lastTop = &(*iterTop);
        lastBottom = &(*iterBottom);
    }
    //mesh->connectFaces();
}

void generateCylinder(Mesh* mesh, const QVector3D& position, const QVector3D& positionTop, const float& radius) {
    generateCustomCylinder(mesh, position, positionTop, radius, radius);
}

/*void generateFace(Mesh* mesh, Path path, float z) {
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
}*/


/*void generateRaft(Mesh* mesh, Slices* slices){
    for (int s_idx=0; s_idx<slices->size(); s_idx++){

    }
}*/

/*void generateRaft(Mesh* mesh, OverhangPoint *point) {
    float bottom = (float)-3.0;//(point->position.Z - (int)point->height * 1000) / scfg->resolution;
    float radius = (float)scfg->raft_offset_radius / (float)scfg->resolution;
    QVector3D positionBottom = QVector3D((float)point->position.X / scfg->resolution,
            (float)point->position.Y / scfg->resolution,
            bottom);
    QVector3D positionMiddle = QVector3D((float)point->position.X / scfg->resolution,
                                         (float)point->position.Y / scfg->resolution,
                                         bottom/2);
    QVector3D positionTop = QVector3D((float)point->position.X / scfg->resolution,
            (float)point->position.Y / scfg->resolution,
            0);
    generateCustomCylinder(mesh, positionBottom, positionMiddle, radius, radius*2);
    generateCustomCylinder(mesh, positionMiddle, positionTop, radius*2, radius);
}*/

/*void generateInfill(Mesh* mesh, Slice* slice) {
    for( auto iter = slice->outershell.begin() ; iter != slice->outershell.end() ; iter++ ) {
        generateFace(mesh, (*iter), slice->z);
    }
}*/
