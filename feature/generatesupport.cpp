/* Clever Support: Efficient Support Structure Generation for Digital Fabrication (J. Vanek & J. A. G. Galicia & B. Benes) */

#include "generateSupport.h"
#include "DentEngine/src/configuration.h"
#include <QtMath>

OverhangPoint_::OverhangPoint_()
{
    position = QVector3D(99999, 99999, 99999);
}

OverhangPoint_::OverhangPoint_(QVector3D point)
{
    position = point;
}

OverhangPoint_::OverhangPoint_(QVector3D point, bool top) {
    position = point;
    topPoint = top;
}

OverhangPoint_::OverhangPoint_(QVector3D point, bool meshInt, bool supportInt) {
    position = point;
    meshInterPoint = meshInt;
    supportInterPoint = supportInt;
}

OverhangPoint_::OverhangPoint_(QVector3D point, float r) {
    position = point;
    radius = r;
}

GenerateSupport::GenerateSupport()
{

}

Mesh* GenerateSupport::generateSupport(Mesh* shellmesh) {
    Mesh* mesh = shellmesh;
    // 하나의 face에 여러 overhangpoint가 들어가면 reserve 값도 바꿔주어야 함
    overhangPoints.reserve((mesh->vertices.size() + mesh->faces.size())*2); //
    supportPoints.reserve((mesh->vertices.size() + mesh->faces.size())*2); //
    overhangDetect(mesh);

    Mesh* supportMesh = new Mesh();
    supportMesh->faces.reserve(overhangPoints.size()*50); // overhangpoints * 2 * 24
    supportMesh->vertices.reserve(overhangPoints.size()*50); //

    vector<OverhangPoint_>::iterator iter = overhangPoints.begin();
    while (iter != overhangPoints.end() - 1 && iter != overhangPoints.end()) {
        OverhangPoint_ *pt1 = &*iter;
        OverhangPoint_ *pt2 = &*(iter+1);
        /*while (pt1->topPoint && pt2->topPoint && iter != overhangPoints.end() - 1
               && (pt1->position - pt2->position).length() < 2) { // 2 -> variable
            iter++;
            pt2 = &*(iter+1);
        }*/ // sorting algorithm
        OverhangPoint_ intersection = coneNconeIntersection(mesh, *pt1, *pt2);
        OverhangPoint_ meshIntersection1 = coneNmeshIntersection(mesh, *pt1);
        OverhangPoint_ meshIntersection2 = coneNmeshIntersection(mesh, *pt2);
        if ((pt1->position - meshIntersection1.position).length() < (pt1->position - intersection.position).length()
            || intersection.position == QVector3D(99999,99999,99999)) {
            generateStem(supportMesh, pt1, &meshIntersection1); // connect with mesh or bed
            iter++;
            continue;
        }
        if ((pt2->position - meshIntersection2.position).length() < (pt2->position - intersection.position).length()) {
            generateStem(supportMesh, pt2, &meshIntersection2); // connect with mesh or bed
            *iter = *pt2;
            *(iter+1) = *pt1;
            iter++;
            continue;
        }
        if (intersection.position == pt1->position) {
            generateStem(supportMesh, pt2, pt1);
            overhangPoints.push_back(intersection);
        } else if (intersection.position == pt2->position) {
            generateStem(supportMesh, pt1, pt2);
            overhangPoints.push_back(intersection);
        /*} else if (intersection.position == QVector3D(99999, 99999, 99999)) {
            generateStem(glmodel->supportMesh, pt1, &meshIntersection1);
            generateStem(glmodel->supportMesh, pt2, &meshIntersection2);*/
        } else {
            generateBranch(supportMesh, pt1, pt2, &intersection);
            overhangPoints.push_back(intersection);
        }
        iter += 2;
    }
    while (iter != overhangPoints.end()) { // iter가 2씩 증가
        OverhangPoint_ meshIntersection = coneNmeshIntersection(mesh, *iter);
        generateStem(supportMesh, &*iter, &meshIntersection);
        iter++;
    }
    supportMesh->connectFaces();
    // mergeSupportMesh(glmodel);
    //emit glmodel->_updateModelMesh(true);
    qDebug() << "generateSupport Done";
    return supportMesh;
}

void GenerateSupport::overhangDetect(Mesh* mesh) {

    pointOverhangDetect(mesh);
    faceOverhangDetect(mesh);
    edgeOverhangDetect(mesh);

    sortOverhangPoints(); // 다른 알고리즘?
}

/* Point overhang
 * : a point that is positioned lower than its neighboring points
 *   (local or global minimum)
 */
void GenerateSupport::pointOverhangDetect(Mesh* mesh) {
    qDebug() << "pointOverhangDetect";

    vector<MeshVertex> pointOverhang;
    for (size_t ver_idx = 0; ver_idx < mesh->vertices.size(); ver_idx++) {
        bool local_min = true;
        float z = mesh->vertices[ver_idx].position.z();
        for (size_t face_idx = 0; face_idx < mesh->vertices[ver_idx].connected_faces.size() && local_min; face_idx++) {
            for (size_t i = 0; i < 3; i++) {
                if ((mesh->idx2MV(mesh->vertices[ver_idx].connected_faces[face_idx]->mesh_vertex[i])).position.z() < z) {
                    local_min = false;
                    break;
                }
            }
        }
        if (local_min && (mesh->vertices[ver_idx].position.z() - mesh->z_min) >= minZ) { //
            pointOverhang.push_back(mesh->vertices[ver_idx]);
        }
    }

    // MeshVertex to QVector3D
    for (size_t i = 0; i < pointOverhang.size(); i++) {
        overhangPoints.push_back(OverhangPoint_(pointOverhang[i].position, true));
        //qDebug() << "pointOverhang" << pointOverhang[i].position;
    }

}

/* Face overhang
 * : a face s.t. the angle btw the face and the printing direction (z-axis) is higher than the critical angle
 */
void GenerateSupport::faceOverhangDetect(Mesh* mesh) {
    qDebug() << "faceOverhangDetect";

    vector<MeshFace> faceOverhang;
    QVector3D printingDirection = QVector3D(0,0,1);
    QVector3D faceNormal;
    for (size_t face_idx = 0; face_idx < mesh->faces.size(); face_idx++) {
        faceNormal = mesh->faces[face_idx].fn;
        if (faceNormal.z() > 0) continue;
        float cos = QVector3D::dotProduct(faceNormal, printingDirection);
        if (qAbs(cos) > qAbs(qCos(critical_angle_radian))) {
            faceOverhang.push_back(mesh->faces[face_idx]);
        }
    }

    // MeshFace to QVector3D
    for (size_t i = 0 ; i < faceOverhang.size(); i++) {
        QVector3D overhangPoint = ((mesh->idx2MV(faceOverhang[i].mesh_vertex[0])).position +
                                   (mesh->idx2MV(faceOverhang[i].mesh_vertex[1])).position +
                                   (mesh->idx2MV(faceOverhang[i].mesh_vertex[2])).position)/3;
        bool close = false;
        for (size_t idx = 0; idx < overhangPoints.size(); idx++) {
            if (abs(overhangPoints[idx].position.z() - overhangPoint.z()) <= 1 // 1->variable
                && (overhangPoints[idx].position - overhangPoint).length() <= 2) { // 2->variable
                close = true;
                break;
            }
        }
        if ((overhangPoint.z() - mesh->z_min) >= minZ && !close) //
            overhangPoints.push_back(OverhangPoint_(overhangPoint, true));
        //qDebug() << "faceOverhang" << overhangPoint;
    }
}

void GenerateSupport::edgeOverhangDetect(Mesh* mesh) {

}

void GenerateSupport::sortOverhangPoints() {
    OverhangPoint_ current;
    OverhangPoint_ next;
    size_t idx;
    for (size_t i = 1; i < overhangPoints.size() - 1; i++) {
        current = overhangPoints[i-1];
        next = overhangPoints[i];
        idx = i;
        for (size_t j = i + 1; j < overhangPoints.size(); j++) {
            if ((current.position - next.position).length() > (current.position - overhangPoints[j].position).length()) {
                next = overhangPoints[j];
                idx = j;
            }
        }
        overhangPoints[idx] = overhangPoints[i];
        overhangPoints[i] = next;
    }
}

OverhangPoint_ GenerateSupport::coneNconeIntersection(Mesh* mesh, OverhangPoint_ coneApex1, OverhangPoint_ coneApex2) {
    // qDebug() << "coneNconeIntersection" << coneApex1.position << coneApex2.position;
    float x1 = coneApex1.position.x();
    float y1 = coneApex1.position.y();
    float z1 = coneApex1.position.z() - mesh->z_min;
    float x2 = coneApex2.position.x();
    float y2 = coneApex2.position.y();
    float z2 = coneApex2.position.z() - mesh->z_min;
    double tan = qTan(critical_angle_radian);
    double xyDis = qSqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));

    if (xyDis == 0) {
        if (z1 <= z2) return coneApex1;
        else return coneApex2;
    }

    if (xyDis >= z1 * tan + z2 * tan) return QVector3D(99999, 99999, 99999);

    if (xyDis > qAbs((z1 - z2) * tan)) {
        float radius = 0.5 * ((z1 + z2) * tan - xyDis);
        float m = z1 * tan - radius;
        float n = z2 * tan - radius;
        float x = (n * x1 + m * x2) / (m + n);
        float y = (n * y1 + m * y2) / (m + n);
        float z = radius / tan;
        return QVector3D(x, y, z + mesh->z_min);
    } else {
        if (z1 <= z2) return coneApex1;
        else return coneApex2;
    }
}

OverhangPoint_ GenerateSupport::coneNmeshIntersection(Mesh *mesh, OverhangPoint_ coneApex) {
    // qDebug() << "coneNMeshIntersection";
    QVector3D nearest = QVector3D(99999, 99999, 99999);
    bool meshInt = false;
    bool supportInt = false;
    QVector3D tmp;
    size_t vertex;

    // mesh
    for (vertex = 0; vertex < mesh->vertices.size(); vertex++) {
        if (mesh->vertices[vertex].position.z() >= coneApex.position.z()) continue;
        tmp = mesh->vertices[vertex].position - coneApex.position;
        if (QVector3D::dotProduct(tmp, QVector3D(0, 0, -1)) / tmp.length() < qCos(critical_angle_radian)) continue;
        nearest = mesh->vertices[vertex].position;
        meshInt = true;
        break;
    }

    for (vertex++; vertex < mesh->vertices.size(); vertex++) {
        if (mesh->vertices[vertex].position.z() >= coneApex.position.z()) continue;
        tmp = mesh->vertices[vertex].position - coneApex.position;
        if (tmp.length() < (coneApex.position - nearest).length() &&
            QVector3D::dotProduct(tmp, QVector3D(0, 0, -1)) / tmp.length() >= qCos(critical_angle_radian)) {
            nearest = mesh->vertices[vertex].position;
            meshInt = true;
        }
    }

    // supportMesh
    for (vertex = 0; vertex < supportPoints.size(); vertex++) {
        if (supportPoints[vertex].position.z() >= coneApex.position.z()) continue;
        tmp = supportPoints[vertex].position - coneApex.position;
        if (tmp.length() < (coneApex.position - nearest).length() &&
            QVector3D::dotProduct(tmp, QVector3D(0, 0, -1)) / tmp.length() >= qCos(critical_angle_radian)) {
            nearest = supportPoints[vertex].position;
            meshInt = false;
            supportInt = true;
        }
    }

    // bed
    if ((coneApex.position.z() - mesh->z_min) < (nearest - coneApex.position).length()) {
        nearest = QVector3D(coneApex.position.x(), coneApex.position.y(), mesh->z_min);
        meshInt = false;
    }

    return OverhangPoint_(nearest, meshInt, supportInt);
}

float GenerateSupport::calculateRadius(float mesh_height, float bottom_height, float branch_length) {
    // float radius = 0.2 * branch_length * (1 - pow((bottom_height / mesh_height), 5));
    float radius = 0.2 * branch_length;
    // 계산식
    if (radius > supportRadiusMax) radius = supportRadiusMax;
    return radius;
}

/*
void GenerateSupport::generateTip(Mesh* mesh, OverhangPoint_ *position, float height) {
    QVector3D center = position->position - QVector3D(0, 0, height);
    float radius = calculateRadius(mesh->z_max - mesh->z_min, center.z() - mesh->z_min, height);
    position->radius = radius;
    if (height >= 0) {
        for (size_t i = 0; i < 6; i ++) {
            mesh->addFace(position->position, center + radius * QVector3D(qCos(M_PI * i / 3), qSin(M_PI * i / 3), 0),
                                center + radius * QVector3D(qCos(M_PI * (i+1) / 3), qSin(M_PI * (i+1) / 3), 0));
        }
    } else {
        for (size_t i = 0; i < 6; i ++) {
            mesh->addFace(position->position, center + radius * QVector3D(qCos(M_PI * (i+1) / 3), qSin(M_PI * (i+1) / 3), 0),
                                center + radius * QVector3D(qCos(M_PI * i / 3), qSin(M_PI * i / 3), 0));
        }
    }
}
*/

void GenerateSupport::generateBottomFace(Mesh* mesh, OverhangPoint_* center) {
    float radius = center->radius;
    for (size_t i = 0; i < 6; i ++) {
        mesh->addFace(center->position, center->position + radius * QVector3D(qCos(M_PI * (i+1) / 3), qSin(M_PI * (i+1) / 3), 0),
                              center->position + radius * QVector3D(qCos(M_PI * i / 3), qSin(M_PI * i / 3), 0));
    }
}

void GenerateSupport::generateBranch(Mesh* mesh, OverhangPoint_* leaf1, OverhangPoint_* leaf2, OverhangPoint_* stem) {
    // vertex 추가도
    /*
    if (leaf1->topTip) {
        generateTip(mesh, leaf1, supportTipHeight); // glmodel->supportMesh
        leaf1->position = leaf1->position - QVector3D(0,0,supportTipHeight);
    }
    if (leaf2->topTip) {
        generateTip(mesh, leaf2, supportTipHeight); // glmodel->supportMesh
        leaf2->position = leaf2->position - QVector3D(0,0,supportTipHeight);
    }
    */
    // branch1 (육각기둥)
    float radius1 = leaf1->radius;
    float radius2 = leaf2->radius;
    float bottomRadius = calculateRadius(mesh->z_max - mesh->z_min, stem->position.z() - mesh->z_min,
                         max((leaf1->position - stem->position).length(), (leaf2->position - stem->position).length()));
    if (bottomRadius < max(max(radius1, radius2), stem->radius)) bottomRadius = max(max(radius1, radius2), stem->radius);

    stem->radius = bottomRadius;
    for (size_t i = 0; i < 6; i++) {
        mesh->addFace(leaf1->position + radius1 * QVector3D(qCos(M_PI * (i+1) / 3), qSin(M_PI * (i+1) / 3), 0),
                      leaf1->position + radius1 * QVector3D(qCos(M_PI * i / 3), qSin(M_PI * i / 3), 0),
                      stem->position + bottomRadius * QVector3D(qCos(M_PI * i / 3), qSin(M_PI * i / 3), 0));
        mesh->addFace(stem->position + bottomRadius * QVector3D(qCos(M_PI * i / 3), qSin(M_PI * i / 3), 0),
                      stem->position + bottomRadius * QVector3D(qCos(M_PI * (i+1) / 3), qSin(M_PI * (i+1) / 3), 0),
                      leaf1->position + radius1 * QVector3D(qCos(M_PI * (i+1) / 3), qSin(M_PI * (i+1) / 3), 0));
    }

    // branch2 (육각기둥)
    for (size_t i = 0; i < 6; i++) {
        mesh->addFace(leaf2->position + radius2 * QVector3D(qCos(M_PI * (i+1) / 3), qSin(M_PI * (i+1) / 3), 0),
                      leaf2->position + radius2 * QVector3D(qCos(M_PI * i / 3), qSin(M_PI * i / 3), 0),
                      stem->position + bottomRadius * QVector3D(qCos(M_PI * i / 3), qSin(M_PI * i / 3), 0));
        mesh->addFace(stem->position + bottomRadius * QVector3D(qCos(M_PI * i / 3), qSin(M_PI * i / 3), 0),
                      stem->position + bottomRadius * QVector3D(qCos(M_PI * (i+1) / 3), qSin(M_PI * (i+1) / 3), 0),
                      leaf2->position + radius2 * QVector3D(qCos(M_PI * (i+1) / 3), qSin(M_PI * (i+1) / 3), 0));
    }

    supportPoints.push_back(*stem);
    if ((leaf1->position - stem->position).length() >= 8)
        supportPoints.push_back(OverhangPoint_((leaf1->position + stem->position)/2, (radius1 + bottomRadius)/2));
    if ((leaf2->position - stem->position).length() >= 8)
        supportPoints.push_back(OverhangPoint_((leaf2->position + stem->position)/2, (radius2 + bottomRadius)/2));
}

void GenerateSupport::generateNShapeBranch(Mesh* mesh, OverhangPoint_* pt1, OverhangPoint_* pt2, size_t diameter_mm, size_t thickness_mm) {

}

void GenerateSupport::generateStem(Mesh* mesh, OverhangPoint_* top, OverhangPoint_* bottom) {
    /*
    if (top->topTip) {
        generateTip(mesh, top, supportTipHeight); // glmodel->supportMesh
        top->position = top->position - QVector3D(0,0,supportTipHeight);
    }
    */

    if (top->topPoint && bottom->meshInterPoint && (top->position - bottom->position).length() < minLength) return;

    OverhangPoint_ origin_bottom = *bottom;
    if (bottom->meshInterPoint) {
        if ((top->position - bottom->position).length() <= 5) *bottom = OverhangPoint_(internalDiv(*top, *bottom, 1, 1), bottom->radius);
        else *bottom = OverhangPoint_(internalDiv(*top, *bottom, 3, 1), bottom->radius);
    }

    float topRadius = top->radius;
    float bottomRadius = calculateRadius(mesh->z_max - mesh->z_min, bottom->position.z() - mesh->z_min,
                                         top->position.z() - bottom->position.z());
    if (bottomRadius < max(topRadius, bottom->radius)) bottomRadius = max(topRadius, bottom->radius);
    bottom->radius = bottomRadius;

    if (origin_bottom.meshInterPoint) {
        supportPoints.push_back(*bottom);
        if ((top->position - bottom->position).length() >= 8)
            supportPoints.push_back(OverhangPoint_((top->position + bottom->position)/2, (topRadius + bottomRadius)/2));
    }

    for (size_t i = 0; i < 6; i++) {
        mesh->addFace(top->position + topRadius * QVector3D(qCos(M_PI * (i+1) / 3), qSin(M_PI * (i+1) / 3), 0),
                      top->position + topRadius * QVector3D(qCos(M_PI * i / 3), qSin(M_PI * i / 3), 0),
                      bottom->position + bottomRadius * QVector3D(qCos(M_PI * i / 3), qSin(M_PI * i / 3), 0));
        mesh->addFace(bottom->position + bottomRadius * QVector3D(qCos(M_PI * i / 3), qSin(M_PI * i / 3), 0),
                      bottom->position + bottomRadius * QVector3D(qCos(M_PI * (i+1) / 3), qSin(M_PI * (i+1) / 3), 0),
                      top->position + topRadius * QVector3D(qCos(M_PI * (i+1) / 3), qSin(M_PI * (i+1) / 3), 0));
    }

    if (origin_bottom.meshInterPoint) { // generate tip
        for (size_t i = 0; i < 6; i++) {
            mesh->addFace(bottom->position + bottomRadius * QVector3D(qCos(M_PI * (i+1) / 3), qSin(M_PI * (i+1) / 3), 0),
                          bottom->position + bottomRadius * QVector3D(qCos(M_PI * i / 3), qSin(M_PI * i / 3), 0),
                          origin_bottom.position + origin_bottom.radius * QVector3D(qCos(M_PI * i / 3), qSin(M_PI * i / 3), 0));
            mesh->addFace(origin_bottom.position + origin_bottom.radius * QVector3D(qCos(M_PI * i / 3), qSin(M_PI * i / 3), 0),
                          origin_bottom.position + origin_bottom.radius * QVector3D(qCos(M_PI * (i+1) / 3), qSin(M_PI * (i+1) / 3), 0),
                          bottom->position + bottomRadius * QVector3D(qCos(M_PI * (i+1) / 3), qSin(M_PI * (i+1) / 3), 0));
        }
    } else if (!bottom->supportInterPoint) generateBottomFace(mesh, bottom);
}

/*
void GenerateSupport::generateStem(Mesh* mesh, OverhangPoint_* stem) {
    OverhangPoint_ bottom = OverhangPoint_(stem->position.x(), stem->position.y(), mesh->z_min);
    generateStem(mesh, stem, &bottom);
}


void GenerateSupport::mergeSupportMesh(GLModel* glmodel) {
    qDebug() << "mergeSupportMesh";
    // vertex
    MeshFace newFace;
    for (vector<MeshFace>::iterator it = glmodel->supportMesh->faces.begin(); it != glmodel->supportMesh->faces.end(); ++it) {
        newFace = *it;
        QVector3D v1 = glmodel->supportMesh->idx2MV(newFace.mesh_vertex[0]).position;
        QVector3D v2 = glmodel->supportMesh->idx2MV(newFace.mesh_vertex[1]).position;
        QVector3D v3 = glmodel->supportMesh->idx2MV(newFace.mesh_vertex[2]).position;
        glmodel->mesh->addFace(v1,v2,v3);
    }
    glmodel->mesh->connectFaces();
}
*/

QVector3D GenerateSupport::internalDiv(OverhangPoint_ a, OverhangPoint_ b, float m, float n) {
    return (m * b.position + n * a.position) / (m + n);
}

