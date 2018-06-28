#include "supportview.h"
#include <QtMath>

void generateCylinder(Mesh* mesh, OverhangPoint *point, OverhangPoint *parent){

    qDebug() << point << point->branching_overhang_point << point->target_branching_overhang_point << parent;
    qDebug() << "height:" << point->height <<
                "radius:" << point->radius <<
                "branchable:" << point->branchable <<
                "v(" << point->position.X << "," << point->position.Y << "," << point->position.Z << ")";

    float height = point->height;
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

    if( point->target_branching_overhang_point != nullptr && parent == nullptr ) {
        generateCylinder(mesh, point->target_branching_overhang_point, point);
    }
}
