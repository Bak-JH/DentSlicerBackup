/* Clever Support: Efficient Support Structure Generation for Digital Fabrication (J. Vanek & J. A. G. Galicia & B. Benes) */

#include "generateSupport.h"
#include "DentEngine/src/configuration.h"
#include <QtMath>

OverhangPoint::OverhangPoint()
{
    position = QVector3D(99999, 99999, 99999);
}

OverhangPoint::OverhangPoint(QVector3D point)
{
    position = point;
}

OverhangPoint::OverhangPoint(QVector3D point, bool top) {
    position = point;
    topPoint = top;
}

OverhangPoint::OverhangPoint(QVector3D point, bool meshInt, bool supportInt, float r) {
    position = point;
    meshInterPoint = meshInt;
    supportInterPoint = supportInt;
    radius = r;
}

OverhangPoint::OverhangPoint(QVector3D point, float r) {
    position = point;
    radius = r;
}

GenerateSupport::GenerateSupport()
{

}

Mesh* GenerateSupport::generateSupport(Mesh* shellmesh) {
    Mesh* mesh = shellmesh;
    // 하나의 face에 여러 overhangpoint가 들어가면 reserve 값도 바꿔주어야 함
    overhangPoints.reserve((mesh->vertices.size() + mesh->faces.size())*2);
    supportPoints.reserve((mesh->vertices.size() + mesh->faces.size())*2);
    Mesh* supportMesh = new Mesh();

    overhangDetect(mesh);

    supportMesh->faces.reserve(overhangPoints.size()*50); // overhangpoints * 2 * 24
    supportMesh->vertices.reserve(overhangPoints.size()*50);

    size_t idx = 0;
    findNearestPoint(idx);

    vector<OverhangPoint>::iterator iter = overhangPoints.begin();
    while (iter != overhangPoints.end() - 1 && iter != overhangPoints.end()) {
        OverhangPoint pt1 = *iter;
        OverhangPoint pt2 = *(iter+1);
        OverhangPoint intersection = coneNconeIntersection(mesh, pt1, pt2);
        OverhangPoint meshIntersection1 = coneNmeshIntersection(mesh, pt1);
        OverhangPoint meshIntersection2 = coneNmeshIntersection(mesh, pt2);

        if ((pt1.position - meshIntersection1.position).length() < (pt1.position - intersection.position).length()
            || intersection.position == QVector3D(99999,99999,99999)) {
            generateStem(supportMesh, pt1, &meshIntersection1); // connect with mesh or bed
            iter++;
            findNearestPoint(++idx);
        } else if ((pt2.position - meshIntersection2.position).length() < (pt2.position - intersection.position).length()) {
            generateStem(supportMesh, pt2, &meshIntersection2); // connect with mesh or bed
            *iter = pt2;
            *(iter+1) = pt1;
            iter++;
            findNearestPoint(++idx);
        } else if (intersection.position == pt1.position) {
            generateStem(supportMesh, pt2, &pt1);
            *iter = pt2;
            *(iter+1) = pt1;
            iter++;
            findNearestPoint(++idx);
        } else if (intersection.position == pt2.position) {
            generateStem(supportMesh, pt1, &pt2);
            iter++;
            findNearestPoint(++idx);
        } else {
            generateBranch(supportMesh, pt1, pt2, &intersection);
            overhangPoints.push_back(intersection);
            iter += 2;
            findNearestPoint(++idx);
            findNearestPoint(++idx);
        }
    }
    while (iter != overhangPoints.end()) {
        OverhangPoint meshIntersection = coneNmeshIntersection(mesh, *iter);
        generateStem(supportMesh, *iter, &meshIntersection);
        iter++;
    }

    supportMesh->connectFaces();

    qDebug() << "generateSupport Done";
    return supportMesh;
}

void GenerateSupport::overhangDetect(Mesh* mesh) {

    pointOverhangDetect(mesh);
    faceOverhangDetect(mesh);
    edgeOverhangDetect(mesh);

    // sortOverhangPoints(); // 다른 알고리즘?
}

/* Point overhang
 * : a point that is positioned lower than its neighboring points
 *   (local or global minimum)
 */
void GenerateSupport::pointOverhangDetect(Mesh* mesh) {
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
        if (local_min && (mesh->vertices[ver_idx].position.z() - mesh->z_min()) >= minZ) { //
            bool close = false;
            for (size_t idx = 0; idx < overhangPoints.size(); idx++) {
                if (abs(overhangPoints[idx].position.z() - mesh->vertices[ver_idx].position.z()) <= 1 // 1->variable
                    && (overhangPoints[idx].position - mesh->vertices[ver_idx].position).length() <= 2) { // 2->variable
                    close = true;
                    break;
                }
            }
            if (!close) pointOverhang.push_back(mesh->vertices[ver_idx]);
        }
    }

    // MeshVertex to OverhangPoint
    for (size_t i = 0; i < pointOverhang.size(); i++) {
        QVector3D overhangPoint = pointOverhang[i].position;
        bool close = false;
        for (size_t idx = 0; idx < overhangPoints.size(); idx++) {
            if (abs(overhangPoints[idx].position.z() - overhangPoint.z()) <= 1 // 1->variable
                && (overhangPoints[idx].position - overhangPoint).length() <= 2) { // 2->variable
                close = true;
                break;
            }
        }
        if ((overhangPoint.z() - mesh->z_min()) >= minZ && !close) //
            overhangPoints.push_back(OverhangPoint(overhangPoint, true));
    }
}

/* Face overhang
 * : a face s.t. the angle btw the face and the printing direction (z-axis) is higher than the critical angle
 */
void GenerateSupport::faceOverhangDetect(Mesh* mesh) {
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

    // MeshFace to OverhangPoint
    for (size_t i = 0 ; i < faceOverhang.size(); i++) {
        QVector3D v0 = mesh->idx2MV(faceOverhang[i].mesh_vertex[0]).position;
        QVector3D v1 = mesh->idx2MV(faceOverhang[i].mesh_vertex[1]).position;
        QVector3D v2 = mesh->idx2MV(faceOverhang[i].mesh_vertex[2]).position;
        faceOverhangPoint(mesh, v0, v1, v2);
    }
}

void GenerateSupport::faceOverhangPoint(Mesh* mesh, QVector3D v0, QVector3D v1, QVector3D v2) {
    // area subdivision (recursive case)
    if (QVector3D::crossProduct(v1-v0, v2-v0).length()/2.0 >= 10) { // 10->variable
        faceOverhangPoint(mesh, (v0+v1)/2, (v1+v2)/2, (v2+v0)/2);
        faceOverhangPoint(mesh, v0, (v0+v1)/2, (v0+v2)/2);
        faceOverhangPoint(mesh, (v0+v1)/2, v1, (v1+v2)/2);
        faceOverhangPoint(mesh, (v0+v2)/2, (v1+v2)/2, v2);
        return;
    }

    // (base case)
    QVector3D overhangPoint = (v0 + v1 + v2)/3;
    bool close = false;
    for (size_t idx = 0; idx < overhangPoints.size(); idx++) {
        if (abs(overhangPoints[idx].position.z() - overhangPoint.z()) <= 1 // 1->variable
            && (overhangPoints[idx].position - overhangPoint).length() <= 2) { // 2->variable
            close = true;
            break;
        }
    }
    if ((overhangPoint.z() - mesh->z_min()) >= minZ && !close) //
        overhangPoints.push_back(OverhangPoint(overhangPoint, true));
}

void GenerateSupport::edgeOverhangDetect(Mesh* mesh) {

}

/*
void GenerateSupport::sortOverhangPoints() {
    OverhangPoint current;
    OverhangPoint next;
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
*/

void GenerateSupport::findNearestPoint(size_t index) {
    if (index >= overhangPoints.size() - 2) return;

    QVector3D pt = overhangPoints[index].position;
    size_t nearest = index + 1;
    for (size_t i = index + 2; i < overhangPoints.size(); i++) {
        if ((overhangPoints[i].position - pt).length() < (overhangPoints[nearest].position - pt).length()) {
            nearest = i;
        }
    }
    swap(overhangPoints[index+1], overhangPoints[nearest]);
}

OverhangPoint GenerateSupport::coneNconeIntersection(Mesh* mesh, OverhangPoint coneApex1, OverhangPoint coneApex2) {
    float x1 = coneApex1.position.x();
    float y1 = coneApex1.position.y();
    float z1 = coneApex1.position.z() - mesh->z_min();
    float x2 = coneApex2.position.x();
    float y2 = coneApex2.position.y();
    float z2 = coneApex2.position.z() - mesh->z_min();
    double tan = qTan(critical_angle_radian);
    double xyDis = qSqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));

    if (xyDis == 0) {
        if (z1 <= z2) return coneApex1;
        else return coneApex2;
    }

    if (xyDis >= z1 * tan + z2 * tan) return QVector3D(99999, 99999, 99999); // intersection under ground

    if (xyDis > qAbs((z1 - z2) * tan)) {
        float radius = 0.5 * ((z1 + z2) * tan - xyDis);
        float m = z1 * tan - radius;
        float n = z2 * tan - radius;
        float x = (n * x1 + m * x2) / (m + n);
        float y = (n * y1 + m * y2) / (m + n);
        float z = radius / tan;
        return QVector3D(x, y, z + mesh->z_min());
    } else {
        if (z1 <= z2) return coneApex1;
        else return coneApex2;
    }
}

OverhangPoint GenerateSupport::coneNmeshIntersection(Mesh *mesh, OverhangPoint coneApex) {
    QVector3D nearest = QVector3D(99999, 99999, 99999);
    bool meshInt = false;
    bool supportInt = false;
    QVector3D tmp;
    size_t vertex;
    float radius = radiusMin;

    // mesh
    for (vertex = 0; vertex < mesh->vertices.size(); vertex++) {
        if (mesh->vertices[vertex].position.z() >= coneApex.position.z()) continue;
        tmp = mesh->vertices[vertex].position - coneApex.position;
        if (QVector3D::dotProduct(tmp, QVector3D(0, 0, -1)) / tmp.length() < qCos(critical_angle_mesh_radian)) continue;
        nearest = mesh->vertices[vertex].position;
        meshInt = true;
        break;
    }

    for (vertex++; vertex < mesh->vertices.size(); vertex++) {
        if (mesh->vertices[vertex].position.z() >= coneApex.position.z()) continue;
        tmp = mesh->vertices[vertex].position - coneApex.position;
        if (tmp.length() < (coneApex.position - nearest).length() &&
            QVector3D::dotProduct(tmp, QVector3D(0, 0, -1)) / tmp.length() >= qCos(critical_angle_mesh_radian)) {
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
            radius = supportPoints[vertex].radius;
            meshInt = false;
            supportInt = true;
        }
    }

    // bed
    if ((coneApex.position.z() - mesh->z_min()) < (nearest - coneApex.position).length()) {
        nearest = QVector3D(coneApex.position.x(), coneApex.position.y(), mesh->z_min());
        meshInt = false;
    }

    return OverhangPoint(nearest, meshInt, supportInt, radius);
}

float GenerateSupport::calculateRadius(float mesh_height, float bottom_height, float branch_length) {
    // float radius = 0.2 * branch_length * (1 - pow((bottom_height / mesh_height), 5));
    float radius = 0.2f * branch_length;
    if (radius > supportRadiusMax) radius = supportRadiusMax;
    return radius;
}

void GenerateSupport::generateBottomFace(Mesh* mesh, OverhangPoint center) {
    float radius = center.radius;
    for (size_t i = 0; i < 6; i ++) {
        mesh->addFace(center.position, center.position + radius * QVector3D(qCos(M_PI * (i+1) / 3), qSin(M_PI * (i+1) / 3), 0),
                              center.position + radius * QVector3D(qCos(M_PI * i / 3), qSin(M_PI * i / 3), 0));
    }
}

void GenerateSupport::generateFaces(Mesh* mesh, OverhangPoint top, OverhangPoint bottom) {
    for (size_t i = 0; i < 6; i++) {
        mesh->addFace(top.position + top.radius * QVector3D(qCos(M_PI * (i+1) / 3), qSin(M_PI * (i+1) / 3), 0),
                      top.position + top.radius * QVector3D(qCos(M_PI * i / 3), qSin(M_PI * i / 3), 0),
                      bottom.position + bottom.radius * QVector3D(qCos(M_PI * i / 3), qSin(M_PI * i / 3), 0));
        mesh->addFace(bottom.position + bottom.radius * QVector3D(qCos(M_PI * i / 3), qSin(M_PI * i / 3), 0),
                      bottom.position + bottom.radius * QVector3D(qCos(M_PI * (i+1) / 3), qSin(M_PI * (i+1) / 3), 0),
                      top.position + top.radius * QVector3D(qCos(M_PI * (i+1) / 3), qSin(M_PI * (i+1) / 3), 0));
    }
}

void GenerateSupport::generateBranch(Mesh* mesh, OverhangPoint leaf1, OverhangPoint leaf2, OverhangPoint* stem) {
    float bottomRadius = calculateRadius(mesh->z_max() - mesh->z_min(), stem->position.z() - mesh->z_min(),
                         max((leaf1.position - stem->position).length(), (leaf2.position - stem->position).length()));
    if (bottomRadius < max(max(leaf1.radius, leaf2.radius), stem->radius))
        bottomRadius = max(max(leaf1.radius, leaf2.radius), stem->radius);

    // generate top tip

    stem->radius = bottomRadius;
    generateFaces(mesh, leaf1, *stem);
    generateFaces(mesh, leaf2, *stem);

    supportPoints.push_back(*stem);

    if ((leaf1.position - stem->position).length() >= 8)
        supportPoints.push_back(OverhangPoint((leaf1.position + stem->position)/2, (leaf1.radius + bottomRadius)/2));
    if ((leaf2.position - stem->position).length() >= 8)
        supportPoints.push_back(OverhangPoint((leaf2.position + stem->position)/2, (leaf2.radius + bottomRadius)/2));
}

void GenerateSupport::generateNShapeBranch(Mesh* mesh, OverhangPoint* pt1, OverhangPoint* pt2, size_t diameter_mm, size_t thickness_mm) {

}

void GenerateSupport::generateStem(Mesh* mesh, OverhangPoint top, OverhangPoint* bottom) {
    if (top.topPoint && (bottom->meshInterPoint || bottom->supportInterPoint)
            && (top.position - bottom->position).length() < minLength) return;

    OverhangPoint origin_bottom = *bottom;
    if (bottom->meshInterPoint) {
        if ((top.position - bottom->position).length() <= 5)
            *bottom = OverhangPoint(internalDiv(top, origin_bottom, 1, 1), origin_bottom.radius);
        else if ((top.position - bottom->position).length() <= 10)
            *bottom = OverhangPoint(internalDiv(top, origin_bottom, 3, 1), origin_bottom.radius);
        else *bottom = OverhangPoint(internalDiv(top, origin_bottom, 5, 1), origin_bottom.radius);
    }

    float bottomRadius = calculateRadius(mesh->z_max() - mesh->z_min(), bottom->position.z() - mesh->z_min(),
                                         top.position.z() - bottom->position.z());
    if (bottomRadius < max(top.radius, origin_bottom.radius))
        bottomRadius = max(top.radius, bottom->radius);
    if (origin_bottom.supportInterPoint && (origin_bottom.radius > radiusMin) && (bottomRadius > origin_bottom.radius))
         bottomRadius = origin_bottom.radius;
    bottom->radius = bottomRadius;

    if (top.topPoint && (top.position - bottom->position).length() >= 8) { // generate top tip // 8->variable
        OverhangPoint tip = OverhangPoint(internalDiv(top, *bottom, 1, 5), bottom->radius);
        generateFaces(mesh, top, tip);
        top = tip;
    }

    generateFaces(mesh, top, *bottom);

    if (origin_bottom.meshInterPoint) // generate bottom tip
        generateFaces(mesh, *bottom, origin_bottom);
    else if (!bottom->supportInterPoint)
        generateBottomFace(mesh, *bottom);

    if (origin_bottom.meshInterPoint)
        supportPoints.push_back(*bottom);

    if ((top.position - bottom->position).length() >= 8)
        supportPoints.push_back(OverhangPoint((top.position + bottom->position)/2, (top.radius + bottom->radius)/2));
}

QVector3D GenerateSupport::internalDiv(OverhangPoint a, OverhangPoint b, float m, float n) {
    return (m * b.position + n * a.position) / (m + n);
}

