/* Clever Support: Efficient Support Structure Generation for Digital Fabrication (J. Vanek & J. A. G. Galicia & B. Benes) */

#include "generateSupport.h"
#include "DentEngine/src/configuration.h"

QDebug Hix::Debug::operator<< (QDebug d, const OverhangPoint& obj)
 {
	d << "position: " << obj.position;
	d << "topPoint: " << obj.topPoint;
	d << "meshInterPoint: " << obj.meshInterPoint;
	d << "supportInterPoint: " << obj.supportInterPoint;
	d << "radius: " << obj.radius;
	return d;
}

OverhangPoint::OverhangPoint()
{
    position = QVector3D(99999, 99999, 99999);
}

OverhangPoint::OverhangPoint(QVector3D point)
{
    position = point;
}

OverhangPoint::OverhangPoint(QVector3D point, bool top, bool supportInt, bool meshInt)
{
    position = point;
    topPoint = top;
    supportInterPoint = supportInt;
    meshInterPoint = meshInt;
}

OverhangPoint::OverhangPoint(QVector3D point, bool top, bool supportInt, bool meshInt, float r)
{
    position = point;
    topPoint = top;
    supportInterPoint = supportInt;
    meshInterPoint = meshInt;
    radius = r;
}

GenerateSupport::GenerateSupport()
{

}

float GenerateSupport::getSupportZMin(const Mesh* mesh)const
{
	return mesh->z_min() - scfg->support_base_height;
}

Mesh* GenerateSupport::generateStraightSupport(Mesh* shellmesh){
    Mesh* mesh = shellmesh;
    Mesh* supportMesh = new Mesh();

    z_min = getSupportZMin(mesh);

    overhangDetect(mesh, supportMesh);
    size_t idx = 0;
    findNearestPoint(idx);
    for (OverhangPoint op : overhangPoints){
        OverhangPoint* new_op = new OverhangPoint(QVector3D(op.position.x(), op.position.y(), z_min));
        generateStem(supportMesh, op, new_op);
    }

    supportMesh->connectFaces();
    return supportMesh;
}

Mesh* GenerateSupport::generateSupport(Mesh* shellmesh) {

    Mesh* mesh = shellmesh;
    Mesh* supportMesh = new Mesh();
	z_min = getSupportZMin(mesh);

    overhangDetect(mesh, supportMesh);

    size_t idx = 0;
    findNearestPoint(idx);

    std::vector<OverhangPoint>::iterator iter = overhangPoints.begin();
    while (iter < overhangPoints.end() - 1) {

        OverhangPoint pt1 = *iter;
        if (pt1.topPoint) {
            iter++;
            findNearestPoint(++idx);
            continue;
        }

        OverhangPoint pt2 = *(iter+1);
        if (pt2.topPoint) {
            *iter = pt2;
            *(iter+1) = pt1;
            iter++;
            findNearestPoint(++idx);
            continue;
        }

        OverhangPoint intersection = coneNconeIntersection(pt1, pt2);
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
            int index = iter - overhangPoints.begin();
            overhangPoints.push_back(intersection);
			//iterator might be invalidated due to resizing
            iter = overhangPoints.begin() + index + 2;
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

void GenerateSupport::overhangDetect(Mesh* mesh, Mesh *support_mesh) {

    pointOverhangDetect(mesh, support_mesh);
    faceOverhangDetect(mesh, support_mesh);

    // sortOverhangPoints(); // 다른 알고리즘?
}

/* Point overhang
 * : a point that is positioned lower than its neighboring points
 *   (local or global minimum)
 */
void GenerateSupport::pointOverhangDetect(Mesh* mesh, Mesh* support_mesh) {
	bool local_min;
	const auto& vertices = mesh->getVertices();
	for (const auto& vertex : vertices)
	{
		local_min = true;
		float z = vertex.position.z();
		auto vtxConnectedFaces = vertex.connectedFaces();
		for (size_t face_idx = 0; face_idx < vtxConnectedFaces.size() && local_min; face_idx++) {
            const MeshFace* connectedFace = vtxConnectedFaces[face_idx].operator->();
			auto meshVertices = connectedFace->meshVertices();

			for (size_t i = 0; i < 3; i++) {
				if (meshVertices[i]->position.z() < z)
				{
					local_min = false;
					break;
				}
			}
		}
        if (local_min && (vertex.position.z() - z_min) >= z_min_minimal_diff) { // is local minima but not global min
			bool close = false;
			for (size_t idx = 0; idx < overhangPoints.size(); idx++) {
                if (std::abs(overhangPoints[idx].position.z() - vertex.position.z()) <= near_z
                    && (overhangPoints[idx].position - vertex.position).length() <= near_dis) {
					close = true;
					break;
				}
			}
            if (!close) {
                generateTip(mesh, support_mesh, vertex.position);
            }
		}
	}
}

/* Face overhang
 * : a face s.t. the angle btw the face and the printing direction (z-axis) is higher than the critical angle
 */
void GenerateSupport::faceOverhangDetect(Mesh* mesh, Mesh* support_mesh) {
    QVector3D printingDirection = QVector3D(0,0,1);
    QVector3D faceNormal;

	const auto& faces(mesh->getFaces());
	for (const MeshFace& face : faces)
	{
		faceNormal = face.fn;
		if (faceNormal.z() > 0) continue;
        double cos = double(QVector3D::dotProduct(faceNormal, printingDirection));
        if (std::abs(cos) > std::abs(std::cos(critical_angle_radian))) {
            auto meshVertices = face.meshVertices();
            QVector3D v0 = meshVertices[0]->position;
            QVector3D v1 = meshVertices[1]->position;
            QVector3D v2 = meshVertices[2]->position;
            faceOverhangPoint(mesh, support_mesh, v0, v1, v2);
		}
	}
}

void GenerateSupport::faceOverhangPoint(Mesh* mesh, Mesh *support_mesh, QVector3D v0, QVector3D v1, QVector3D v2) {
    // area subdivision (recursive case)
    if (QVector3D::crossProduct(v1-v0, v2-v0).length()/2.0f >= area_subdiv) {
        faceOverhangPoint(mesh, support_mesh, (v0+v1)/2, (v1+v2)/2, (v2+v0)/2);
        faceOverhangPoint(mesh, support_mesh, v0, (v0+v1)/2, (v0+v2)/2);
        faceOverhangPoint(mesh, support_mesh, (v0+v1)/2, v1, (v1+v2)/2);
        faceOverhangPoint(mesh, support_mesh, (v0+v2)/2, (v1+v2)/2, v2);
        return;
    }

    // (base case)
    QVector3D overhangPoint = (v0 + v1 + v2)/3;
    bool close = false;
    for (size_t idx = 0; idx < overhangPoints.size(); idx++) {
        if (abs(overhangPoints[idx].position.z() - overhangPoint.z()) <= near_z
            && (overhangPoints[idx].position - overhangPoint).length() <= near_dis) {
            close = true;
            break;
        }
    }
    if ((overhangPoint.z() - z_min) >= z_min_minimal_diff && !close)
        generateTip(mesh, support_mesh, overhangPoint);
}

void GenerateSupport::generateTip(Mesh* mesh, Mesh *support_mesh, QVector3D point) {
    OverhangPoint overhangPoint = OverhangPoint(point, true, false, false);
    overhangPoints.push_back(overhangPoint);
    generateTopFace(support_mesh, overhangPoint);

    OverhangPoint meshInt = coneNmeshIntersection(mesh, overhangPoint);
    if (!meshInt.topPoint && !meshInt.supportInterPoint && !meshInt.meshInterPoint)
        meshInt.radius = (supportRadiusMax + supportRadiusMin) / 2;

    if ((meshInt.topPoint || meshInt.supportInterPoint || meshInt.meshInterPoint)
        && (point - meshInt.position).length() < minLength)
        return;

    if ((point - meshInt.position).length() < tip_len) {
        generateFaces(support_mesh, overhangPoint, meshInt);
        generateBottomFace(support_mesh, meshInt);
    } else {
        OverhangPoint supportPoint = OverhangPoint(overhangPoint.position - QVector3D(0,0,tip_len), false, true, false, supportRadiusMin + 0.1f);
        generateFaces(support_mesh, overhangPoint, supportPoint);
        overhangPoints.push_back(supportPoint);
    }
}

void GenerateSupport::findNearestPoint(size_t index) {
    if (index >= overhangPoints.size() - 2) return;

    QVector3D pt = overhangPoints[index].position;
    size_t nearest = index + 1;
    for (size_t i = index + 2; i < overhangPoints.size(); i++) {
        if ((overhangPoints[i].position - pt).length() < (overhangPoints[nearest].position - pt).length()) {
            nearest = i;
        }
    }
    std::swap(overhangPoints[index+1], overhangPoints[nearest]);
}

OverhangPoint GenerateSupport::coneNconeIntersection(OverhangPoint coneApex1, OverhangPoint coneApex2) {
    float x1 = coneApex1.position.x();
    float y1 = coneApex1.position.y();
    float z1 = coneApex1.position.z() - z_min;
    float x2 = coneApex2.position.x();
    float y2 = coneApex2.position.y();
    float z2 = coneApex2.position.z() - z_min;
    float tan = float(std::tan(critical_angle_radian));
    float xyDis = std::sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));

    if (xyDis == 0) {
        if (z1 <= z2) return coneApex1;
        else return coneApex2;
    }

    if (xyDis >= z1 * tan + z2 * tan) return OverhangPoint(); // intersection under ground

    if (xyDis > std::abs((z1 - z2) * tan)) {
        float radius = 0.5f * ((z1 + z2) * tan - xyDis);
        float m = z1 * tan - radius;
        float n = z2 * tan - radius;
        float x = (n * x1 + m * x2) / (m + n);
        float y = (n * y1 + m * y2) / (m + n);
        float z = radius / tan;
        return OverhangPoint(QVector3D(x, y, z + z_min), false, true, false);
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
    float radius = supportRadiusMin;

    // mesh vertex
	const auto& vertices = mesh->getVertices();
	for (const MeshVertex& vertex : vertices){
		if (vertex.position.z() >= coneApex.position.z()) continue;
        tmp = vertex.position - coneApex.position;
        if (tmp.length() < (coneApex.position - nearest).length() &&
            QVector3D::dotProduct(tmp, QVector3D(0, 0, -1)) / tmp.length() >= std::cos(critical_angle_mesh_radian)) {
            nearest = vertex.position;
            meshInt = true;
        }
    }

    /*
    // mesh face
    const auto& faces(mesh->getFaces());
    for (const MeshFace& face : faces)
    {
        auto meshVertices = face.meshVertices();
        QVector3D v0 = meshVertices[0]->position;
        QVector3D v1 = meshVertices[1]->position;
        QVector3D v2 = meshVertices[2]->position;
        if (QVector3D::crossProduct(v1-v0, v2-v0).length()/2.0f >= area_subdiv) {
            QVector3D point = (v0 + v1 + v2) / 3;
            if (point.z() >= coneApex.position.z()) continue;
            tmp = point - coneApex.position;
            if (tmp.length() < (coneApex.position - nearest).length() &&
                QVector3D::dotProduct(tmp, QVector3D(0, 0, -1)) / tmp.length() >= std::cos(critical_angle_mesh_radian)) {
                nearest = point;
                meshInt = true;
            }
        }
    }
    */

    // supportMesh
    for (size_t vertexId = 0; vertexId < supportPoints.size(); vertexId++) {
        if (supportPoints[vertexId].position.z() >= coneApex.position.z()) continue;
        tmp = supportPoints[vertexId].position - coneApex.position;
        if (tmp.length() < (coneApex.position - nearest).length() &&
            QVector3D::dotProduct(tmp, QVector3D(0, 0, -1)) / tmp.length() >= std::cos(critical_angle_radian)) {
            nearest = supportPoints[vertexId].position;
            radius = supportPoints[vertexId].radius;
            supportInt = true;
            meshInt = false;
        }
    }

    // bed
    if ((coneApex.position.z() - z_min) < (nearest - coneApex.position).length()) {
        nearest = QVector3D(coneApex.position.x(), coneApex.position.y(), z_min);
        supportInt = false;
        meshInt = false;
    }

    return OverhangPoint(nearest, false, supportInt, meshInt, radius);
}

float GenerateSupport::calculateRadius(/*float mesh_height, float bottom_height,*/ float branch_length) {
    // float radius = 0.2 * branch_length * (1 - pow((bottom_height / mesh_height), 5));
    float radius = 0.2f * branch_length;
    if (radius > supportRadiusMax) radius = supportRadiusMax;
    if (radius < supportRadiusMin) radius = supportRadiusMin;
    return radius;
}


void GenerateSupport::generateTopFace(Mesh* mesh, OverhangPoint center) {
    float radius = center.radius;
    for (size_t i = 0; i < 6; i ++) {
        mesh->addFace(center.position, center.position + radius * QVector3D(float(std::cos(M_PI * i / 3)), float(std::sin(M_PI * i / 3)), 0),
                      center.position + radius * QVector3D(float(std::cos(M_PI * (i+1) / 3)), float(std::sin(M_PI * (i+1) / 3)), 0));
    }
}

void GenerateSupport::generateBottomFace(Mesh* mesh, OverhangPoint center) {
    float radius = center.radius;
    for (size_t i = 0; i < 6; i ++) {
        mesh->addFace(center.position, center.position + radius * QVector3D(float(std::cos(M_PI * (i+1) / 3)), float(std::sin(M_PI * (i+1) / 3)), 0),
                              center.position + radius * QVector3D(float(std::cos(M_PI * i / 3)), float(std::sin(M_PI * i / 3)), 0));
    }
}

void GenerateSupport::generateFaces(Mesh* mesh, OverhangPoint top, OverhangPoint bottom) {
    for (size_t i = 0; i < 6; i++) {
        mesh->addFace(top.position + top.radius * QVector3D(float(std::cos(M_PI * (i+1) / 3)), float(std::sin(M_PI * (i+1) / 3)), 0),
                      top.position + top.radius * QVector3D(float(std::cos(M_PI * i / 3)), float(std::sin(M_PI * i / 3)), 0),
                      bottom.position + bottom.radius * QVector3D(float(std::cos(M_PI * i / 3)), float(std::sin(M_PI * i / 3)), 0));
        mesh->addFace(bottom.position + bottom.radius * QVector3D(float(std::cos(M_PI * i / 3)), float(std::sin(M_PI * i / 3)), 0),
                      bottom.position + bottom.radius * QVector3D(float(std::cos(M_PI * (i+1) / 3)), float(std::sin(M_PI * (i+1) / 3)), 0),
                      top.position + top.radius * QVector3D(float(std::cos(M_PI * (i+1) / 3)), float(std::sin(M_PI * (i+1) / 3)), 0));
    }
}

void GenerateSupport::generateBranch(Mesh* mesh, OverhangPoint leaf1, OverhangPoint leaf2, OverhangPoint* stem) {
    float bottomRadius = calculateRadius(/*mesh->z_max() - z_min, stem->position.z() - z_min,*/
                         std::max((leaf1.position - stem->position).length(), (leaf2.position - stem->position).length()));
    if (bottomRadius < std::max(std::max(leaf1.radius, leaf2.radius), stem->radius))
        bottomRadius = std::max(std::max(leaf1.radius, leaf2.radius), stem->radius);

    stem->radius = bottomRadius;
    generateFaces(mesh, leaf1, *stem);
    generateFaces(mesh, leaf2, *stem);

    supportPoints.push_back(*stem);

    if ((leaf1.position - stem->position).length() >= 8)
        supportPoints.push_back(OverhangPoint((leaf1.position + stem->position)/2, false, true, false, (leaf1.radius + bottomRadius)/2));
    if ((leaf2.position - stem->position).length() >= 8)
        supportPoints.push_back(OverhangPoint((leaf2.position + stem->position)/2, false, true, false, (leaf2.radius + bottomRadius)/2));
}

void GenerateSupport::generateStem(Mesh* mesh, OverhangPoint top, OverhangPoint* bottom) {
    if (top.topPoint && (bottom->meshInterPoint || bottom->supportInterPoint)
            && (top.position - bottom->position).length() < minLength) return;

    OverhangPoint origin_bottom = *bottom;
    if (origin_bottom.meshInterPoint)
        *bottom = OverhangPoint(lowerZInternalDiv(top, origin_bottom, bottom_tip_height), false, true, false, origin_bottom.radius);

    float bottomRadius = calculateRadius(/*mesh->z_max() - z_min, bottom->position.z() - z_min,*/
                                         top.position.z() - bottom->position.z());
    if (bottomRadius < std::max(top.radius, bottom->radius))
        bottomRadius = std::max(top.radius, bottom->radius);
    if (origin_bottom.supportInterPoint && (origin_bottom.radius > supportRadiusMin) && (bottomRadius > origin_bottom.radius))
         bottomRadius = origin_bottom.radius;
    bottom->radius = bottomRadius;

    if (top.topPoint && (top.position - bottom->position).length() >= 8) { // generate top tip // 8->variable
        OverhangPoint tempTop = OverhangPoint(top.position + QVector3D(0,0,scfg->layer_height), true, false, false);
        OverhangPoint tip = OverhangPoint(internalDiv(top, *bottom, 1, 5), false, true, false, bottom->radius);
        generateFaces(mesh, tempTop, tip);
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
        supportPoints.push_back(OverhangPoint((top.position + bottom->position)/2, false, true, false, (top.radius + bottom->radius)/2));
}

QVector3D GenerateSupport::internalDiv(OverhangPoint a, OverhangPoint b, float m, float n) {
    return (m * b.position + n * a.position) / (m + n);
}

QVector3D GenerateSupport::lowerZInternalDiv(OverhangPoint upper, OverhangPoint lower, float z) {
    float z_diff = upper.position.z() - lower.position.z();
    if (z_diff <= z) return internalDiv(upper, lower, 1, 1);
    return internalDiv(upper, lower, z_diff - z, z);
}
