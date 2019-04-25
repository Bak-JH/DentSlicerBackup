#include "mesh.h"
#include "qmlmanager.h"
#include <QDebug>
#include <QCoreApplication>

Mesh::Mesh(size_t vertCount, size_t faceCount, const Mesh* origin):Mesh(vertCount, faceCount)
{
	prevMesh = origin->prevMesh;
	nextMesh = origin->nextMesh;
}
Mesh::Mesh(size_t vertCount, size_t faceCount)
{
	//this is no longer necessary as we don't use the pointer to vector elements anymore.
	//ie) memory does not have to be contiguous
	vertices.reserve(vertCount);
	faces.reserve(faceCount);

}

/********************** Mesh Edit Functions***********************/
void Mesh::vertexOffset(float factor){
    int numberofVertices = vertices.size();
    _x_min = 99999;
    _x_max = 99999;
    _y_min = 99999;
    _y_max = 99999;
    _z_min = 99999;
    _z_max = 99999;
    for (int i=0;i<numberofVertices;i++){
        if (i%100 == 0)
            QCoreApplication::processEvents();
        QVector3D tmp = vertices[i].position - vertices[i].vn*factor;
        vertices[i].position = tmp;
        updateMinMax(vertices[i].position);
    }
}

void Mesh::vertexMove(QVector3D direction){
    int numberofVertices = vertices.size();
    _x_min = 99999;
    _x_max = 99999;
    _y_min = 99999;
    _y_max = 99999;
    _z_min = 99999;
    _z_max = 99999;
    for (int i=0;i<numberofVertices;i++){
        if (i%100 == 0)
            QCoreApplication::processEvents();
        QVector3D tmp = direction+vertices[i].position;
        vertices[i].position = tmp;
        updateMinMax(vertices[i].position);
    }
}

Mesh* Mesh::vertexMoved(QVector3D direction)const
{
    Mesh* coppied = copyMesh();
    coppied->vertexMove(direction);
    return coppied;
}

void Mesh::centerMesh(){
    float x_center = (_x_max+_x_min)/2;
    float y_center = (_y_max+_y_min)/2;
    float z_center = (_z_max+_z_min)/2;
    vertexMove(-QVector3D(x_center, y_center, z_center));
    _x_max = _x_max - x_center;
    _x_min = _x_min - x_center;
    _y_max = _y_max - y_center;
    _y_min = _y_min - y_center;
    _z_max = _z_max - z_center;
    _z_min = _z_min - z_center;
}

void Mesh::vertexRotate(QMatrix4x4 tmpmatrix){
    int numberofVertices = vertices.size();
    int numberofFaces = faces.size();
    _x_min = 99999;
    _x_max = 99999;
    _y_min = 99999;
    _y_max = 99999;
    _z_min = 99999;
    _z_max = 99999;
    QVector4D tmpVertex;
    QVector3D tmpVertex2;
    for (int i=0;i<numberofVertices;i++){
        if (i%100 == 0)
            QCoreApplication::processEvents();
        tmpVertex =vertices[i].position.toVector4D();
        tmpVertex2.setX(QVector4D::dotProduct(tmpVertex,tmpmatrix.column(0)));
        tmpVertex2.setY(QVector4D::dotProduct(tmpVertex,tmpmatrix.column(1)));
        tmpVertex2.setZ(QVector4D::dotProduct(tmpVertex,tmpmatrix.column(2)));
        vertices[i].position = tmpVertex2;
        updateMinMax(vertices[i].position);
    }
    for (int i=0;i<numberofFaces;i++){
        if (i%100 == 0)
            QCoreApplication::processEvents();
        faces[i].fn = QVector3D::normal(vertices[faces[i].mesh_vertex[0]].position,
                                        vertices[faces[i].mesh_vertex[1]].position,
                                        vertices[faces[i].mesh_vertex[2]].position);
        faces[i].fn_unnorm = QVector3D::crossProduct(vertices[faces[i].mesh_vertex[1]].position-vertices[faces[i].mesh_vertex[0]].position,
                                                     vertices[faces[i].mesh_vertex[2]].position-vertices[faces[i].mesh_vertex[0]].position);
    }
    for (int i=0;i<numberofVertices;i++){
        if (i%100 == 0)
            QCoreApplication::processEvents();
        if (vertices[i].connected_faces.size()>=3){
            vertices[i].vn = QVector3D(faces[vertices[i].connected_faces[0]].fn + faces[vertices[i].connected_faces[1]].fn + faces[vertices[i].connected_faces[2]].fn).normalized();
        } else {
            vertices[i].vn = QVector3D(0,0,0);
        }
    }
}

void Mesh::vertexScale(float scaleX=1, float scaleY=1, float scaleZ=1, float centerX=0, float centerY=0){

    if(fabs(scaleX) < FZERO || fabs(scaleY) < FZERO || fabs(scaleZ) < FZERO) {
        qmlManager->openResultPopUp("","Scale cannot be 0", "");
        return;
    }

    int numberofVertices = vertices.size();
    _x_min = 99999;
    _x_max = 99999;
    _y_min = 99999;
    _y_max = 99999;
    _z_min = 99999;
    _z_max = 99999;

    /* need to fix center of the model */
    float fixCenterX = centerX - (centerX*scaleX);
    float fixCenterY = centerY - (centerY*scaleY);

    for (int i=0;i<numberofVertices;i++){
        if (i%100 == 0)
            QCoreApplication::processEvents();
        QVector3D tmp;
        tmp.setX(vertices[i].position.x() * scaleX + fixCenterX);
        tmp.setY(vertices[i].position.y() * scaleY + fixCenterY);
        tmp.setZ(vertices[i].position.z() * scaleZ);
        vertices[i].position = tmp;
        updateMinMax(vertices[i].position);
    }
}

Mesh* Mesh::copyMesh()const
{
    Mesh* copyMesh = new Mesh();
    copyMesh->faces.reserve(faces.size()*3);
    copyMesh->vertices.reserve(vertices.size()*3);

    // only need to copy faces, verticesHash, vertices
    foreach(MeshVertex mv, vertices){
        copyMesh->vertices.push_back(mv);
    }
    foreach(MeshFace mf, faces){
        copyMesh->faces.push_back(mf);
    }
    for (auto it = vertices_hash.begin(); it!=vertices_hash.end(); ++it){
        copyMesh->vertices_hash.insert(it.key(), it.value());
    }
    copyMesh->connectFaces();

    copyMesh->_x_max = _x_max;
    copyMesh->_x_min = _x_min;
    copyMesh->_y_max = _y_max;
    copyMesh->_y_min = _y_min;
    copyMesh->_z_max = _z_max;
    copyMesh->_z_min = _z_min;

    return copyMesh;
}

void Mesh::reverseFaces(){
    for (int i=0; i<faces.size(); i++){
        int last = faces[i].mesh_vertex[2];
        faces[i].mesh_vertex[2] = faces[i].mesh_vertex[0];
        faces[i].mesh_vertex[0] = last;
    }
}

void Mesh::setVertex(size_t index, const MeshVertex& value)
{
	vertices[index] = value;
}

/********************** Mesh Generation Functions **********************/

void Mesh::addFace(QVector3D v0, QVector3D v1, QVector3D v2){
    addFace(v0,v1,v2,-1);
}

void Mesh::addFace(QVector3D v0, QVector3D v1, QVector3D v2, int parent_idx){
    int v0_idx = addFaceVertex(v0);
    int v1_idx = addFaceVertex(v1);
    int v2_idx = addFaceVertex(v2);
//
    MeshFace* mf = new MeshFace();

    int new_idx = faces.size();
    mf->idx = new_idx;
    mf->parent_idx = parent_idx;
    mf->mesh_vertex[0] = v0_idx;
    mf->mesh_vertex[1] = v1_idx;
    mf->mesh_vertex[2] = v2_idx;

    mf->fn = QVector3D::normal(vertices[v0_idx].position, vertices[v1_idx].position, vertices[v2_idx].position);
    mf->fn_unnorm = QVector3D::crossProduct(vertices[v1_idx].position-vertices[v0_idx].position,vertices[v2_idx].position-vertices[v0_idx].position);
    faces.emplace_back(*mf);

    vertices[v0_idx].connected_faces.emplace_back(new_idx); //faces
    vertices[v1_idx].connected_faces.emplace_back(new_idx);
    vertices[v2_idx].connected_faces.emplace_back(new_idx);


    if (vertices[v0_idx].connected_faces.size()>=3){
        MeshVertex &mv = vertices[v0_idx];
        mv.vn = QVector3D(faces[mv.connected_faces[0]].fn +faces[mv.connected_faces[1]].fn +faces[mv.connected_faces[2]].fn).normalized();
    } else {
        MeshVertex &mv = vertices[v0_idx];
        mv.vn = QVector3D(0,0,0);
    }

    if (vertices[v1_idx].connected_faces.size()>=3){
        MeshVertex &mv = vertices[v1_idx];
        mv.vn = QVector3D(faces[mv.connected_faces[0]].fn +faces[mv.connected_faces[1]].fn +faces[mv.connected_faces[2]].fn).normalized();
    } else {
        MeshVertex &mv = vertices[v1_idx];
        mv.vn = QVector3D(0,0,0);
    }

    if (vertices[v2_idx].connected_faces.size()>=3){
        MeshVertex &mv = vertices[v2_idx];
        mv.vn = QVector3D(faces[mv.connected_faces[0]].fn +faces[mv.connected_faces[1]].fn +faces[mv.connected_faces[2]].fn).normalized();
    } else {
        MeshVertex &mv = vertices[v2_idx];
        mv.vn = QVector3D(0,0,0);
    }
}

void Mesh::removeFace(MeshFace* mf){
    for (std::vector<MeshFace>::iterator f_it=faces.begin(); f_it!=faces.end(); ++f_it){
        if (f_it->mesh_vertex == mf->mesh_vertex){
            f_it = removeFace(f_it);
        }
    }
    return;
}

std::vector<MeshFace>::iterator Mesh::removeFace(std::vector<MeshFace>::iterator f_it){
    MeshFace &mf = (*f_it);
    //MeshFace &mf = faces[f_idx];
    //std::vector<MeshFace>::iterator f_idx_it = faces.begin()+f_idx;

    MeshVertex &mv0 = vertices[mf.mesh_vertex[0]];
    MeshVertex &mv1 = vertices[mf.mesh_vertex[1]];
    MeshVertex &mv2 = vertices[mf.mesh_vertex[2]];

    // remove f_it face from its neighboring faces' neighboring faces list
    for (auto& nfs : mf.neighboring_faces){
		for (auto nfIndexItr = nfs.begin(); nfIndexItr != nfs.end();)
		{
			MeshFace* nf_ptr = &faces[*nfIndexItr];
			if (nf_ptr->neighboring_faces.size() != 3) { // already deleted meshface
				nfIndexItr = nfs.erase(nfIndexItr);
				continue;
			}
			for (std::vector<size_t> neighboring_faces : nf_ptr->neighboring_faces) {
				auto nf_nf_ptr_it = neighboring_faces.begin();
				while (nf_nf_ptr_it != neighboring_faces.end()) {
					size_t nfNfIndex = (*nf_nf_ptr_it);
					//jesus wtf
					if (&faces[nfNfIndex] == &mf) {
						nf_nf_ptr_it = neighboring_faces.erase(nf_nf_ptr_it);
						break;
					}
					else {
						nf_nf_ptr_it++;
					}
				}
			}
			++nfIndexItr;
		}
    }


    if (mf.mesh_vertex[0] == mf.mesh_vertex[1] && mf.mesh_vertex[1] == mf.mesh_vertex[2]){
		for (int c_idx = 0; c_idx < mv1.connected_faces.size(); c_idx++) {
			MeshFace& cf(faces[mv1.connected_faces[c_idx]]);
            for (int v_idx=0; v_idx<3; v_idx ++){
                if (cf.mesh_vertex[v_idx] == mf.mesh_vertex[1])
                    cf.mesh_vertex[v_idx] = mf.mesh_vertex[0];
            }
        }

        for (int c_idx=0; c_idx < mv2.connected_faces.size(); c_idx ++){
			MeshFace& cf(faces[mv2.connected_faces[c_idx]]);

            for (int v_idx=0; v_idx<3; v_idx ++){
                if (cf.mesh_vertex[v_idx] == mf.mesh_vertex[2])
                    cf.mesh_vertex[v_idx] = mf.mesh_vertex[0];
            }
        }
    } else if (mf.mesh_vertex[0] == mf.mesh_vertex[1]){ // replace 1 vertices in connected faces of 1 by 0
        for (int c_idx=0; c_idx < mv1.connected_faces.size(); c_idx ++){
			MeshFace& cf(faces[mv1.connected_faces[c_idx]]);
            for (int v_idx=0; v_idx<3; v_idx ++){
                if (cf.mesh_vertex[v_idx] == mf.mesh_vertex[1])
                    cf.mesh_vertex[v_idx] = mf.mesh_vertex[0];
            }
        }
    } else if (mf.mesh_vertex[1] == mf.mesh_vertex[2]){ // replace 2 by 1
        for (int c_idx=0; c_idx < mv2.connected_faces.size(); c_idx ++){
			MeshFace& cf(faces[mv2.connected_faces[c_idx]]);
            for (int v_idx=0; v_idx<3; v_idx ++){
                if (cf.mesh_vertex[v_idx] == mf.mesh_vertex[2])
                    cf.mesh_vertex[v_idx] = mf.mesh_vertex[1];
            }
        }
    } else if (mf.mesh_vertex[2] == mf.mesh_vertex[0]){ // replace 0 by 2
        for (int c_idx=0; c_idx < mv0.connected_faces.size(); c_idx ++){
			MeshFace& cf(faces[mv0.connected_faces[c_idx]]);
            for (int v_idx=0; v_idx<3; v_idx ++){
                if (cf.mesh_vertex[v_idx] == mf.mesh_vertex[0])
                    cf.mesh_vertex[v_idx] = mf.mesh_vertex[2];
            }
        }
    }
    return faces.erase(f_it);
    //faces.erase(f_idx_it);
}

// add connected face idx to each meshes
void Mesh::connectFaces(){
    //for (int i=0; i<faces.size(); i++){
    int cnt = 0;
    for (std::vector<MeshFace>::iterator it = faces.begin(); it!= faces.end(); it++){
        if (cnt % 100 == 0){
            QCoreApplication::processEvents();
        }
        cnt ++;

        MeshFace &mf = (*it);

        // clear neighboring faces
        mf.neighboring_faces[0].clear();
        mf.neighboring_faces[1].clear();
        mf.neighboring_faces[2].clear();

        std::vector<size_t> faces1 = findFaceWith2Vertices(mf.mesh_vertex[0], mf.mesh_vertex[1], mf);
        std::vector<size_t> faces2 = findFaceWith2Vertices(mf.mesh_vertex[1], mf.mesh_vertex[2], mf);
        std::vector<size_t> faces3 = findFaceWith2Vertices(mf.mesh_vertex[2], mf.mesh_vertex[0], mf);

        mf.neighboring_faces[0].insert(mf.neighboring_faces[0].end(), faces1.begin(), faces1.end());
        mf.neighboring_faces[1].insert(mf.neighboring_faces[1].end(), faces2.begin(), faces2.end());
        mf.neighboring_faces[2].insert(mf.neighboring_faces[2].end(), faces3.begin(), faces3.end());
    }
}


/********************** Path Generation Functions **********************/

// converts float point to int in microns
void Mesh::addPoint(float x, float y, Path *path)
{
    IntPoint ip;
    ip.X = round(x*scfg->resolution);
    ip.Y = round(y*scfg->resolution);
    //qDebug() << "addPoint called with x " << x << " y " << y << " rounding " << ip.X;
    path->push_back(ip);
}

float minDistanceToContour(QVector3D from, Path contour){
    float min_distance = 0;
    for (int i=0; i<contour.size()-1; i++){
        Path temp_path;
        temp_path.push_back(contour[i]);
        temp_path.push_back(contour[i+1]);
        QVector3D int2qv3 = QVector3D(((float)contour[i].X)/scfg->resolution, ((float)contour[i].Y)/scfg->resolution, from.z());
        IntPoint directionInt = contour[i+1] - contour[i];
        QVector3D direction = QVector3D(directionInt.X/scfg->resolution, directionInt.Y/scfg->resolution, 0);
        float cur_distance = from.distanceToLine(int2qv3, direction);
        if (abs(min_distance) > cur_distance){
            min_distance = cur_distance;
        }
    }
    return min_distance;
}

/*
Paths3D Mesh::intersectionPaths(Path contour, Plane target_plane) {
    Path3D p;

    std::vector<QVector3D> upper;
    std::vector<QVector3D> lower;
    for (int i=0; i<3; i++){
        if (PointInPolygon(IntPoint(target_plane[i].x(),target_plane[i].y()), contour)){
            upper.push_back(target_plane[i]);
        } else {
            lower.push_back(target_plane[i]);
        }
    }

    std::vector<QVector3D> majority;
    std::vector<QVector3D> minority;

    bool flip = false;
    if (upper.size() == 2){
        majority = upper;
        minority = lower;
    } else if (lower.size() == 2){
        flip = true;
        majority = lower;
        minority = upper;
    } else {
        qDebug() << "wrong faces";
        // size is 0
        return p;
    }

    float minority_distance = abs(minDistanceToContour(minority[0], contour)); //abs(minority[0].distanceToPlane(base_plane[0],base_plane[1],base_plane[2]));
    float majority1_distance = abs(minDistanceToContour(majority[0], contour));//abs(majority[0].distanceToPlane(base_plane[0],base_plane[1],base_plane[2]));
    float majority2_distance = abs(minDistanceToContour(majority[1], contour));//abs(majority[1].distanceToPlane(base_plane[0],base_plane[1],base_plane[2]));

    // calculate intersection points
    MeshVertex mv1, mv2;
    mv1.position = minority[0] + (majority[0] - minority[0])*(minority_distance/(majority1_distance+minority_distance));
    mv2.position = minority[0] + (majority[1] - minority[0])*(minority_distance/(majority2_distance+minority_distance));

    if (flip){
        p.push_back(mv1);
        p.push_back(mv2);
    } else {
        p.push_back(mv2);
        p.push_back(mv1);
    }
    return p;
}*/

Path3D Mesh::intersectionPath(Plane base_plane, Plane target_plane)const
{
    Path3D p;

    std::vector<QVector3D> upper;
    std::vector<QVector3D> lower;
    for (int i=0; i<3; i++){
        if (target_plane[i].distanceToPlane(base_plane[0],base_plane[1],base_plane[2]) >0){
            upper.push_back(target_plane[i]);
        } else {
            lower.push_back(target_plane[i]);
        }
    }

    std::vector<QVector3D> majority;
    std::vector<QVector3D> minority;

    bool flip = false;
    if (upper.size() == 2){
        majority = upper;
        minority = lower;
    } else if (lower.size() == 2){
        flip = true;
        majority = lower;
        minority = upper;
    } else {
        qDebug() << "wrong faces";
        // size is 0
        return p;
    }

    float minority_distance = abs(minority[0].distanceToPlane(base_plane[0],base_plane[1],base_plane[2]));
    float majority1_distance = abs(majority[0].distanceToPlane(base_plane[0],base_plane[1],base_plane[2]));
    float majority2_distance = abs(majority[1].distanceToPlane(base_plane[0],base_plane[1],base_plane[2]));

    // calculate intersection points
    MeshVertex mv1, mv2;
    mv1.position = minority[0] + (majority[0] - minority[0])*(minority_distance/(majority1_distance+minority_distance));
    mv2.position = minority[0] + (majority[1] - minority[0])*(minority_distance/(majority2_distance+minority_distance));

    if (flip){
        p.push_back(mv1);
        p.push_back(mv2);
    } else {
        p.push_back(mv2);
        p.push_back(mv1);
    }
    return p;
}

Path Mesh::intersectionPath(MeshFace mf, float z) const
{
    Path p;

    std::vector<MeshVertex> upper;
    std::vector<MeshVertex> lower;
    std::vector<MeshVertex> middle;

    for (int i=0; i<3; i++){
        if (idx2MV(mf.mesh_vertex[i]).position.z() > z){
            upper.push_back(idx2MV(mf.mesh_vertex[i]));
        } else if (doubleAreSame(idx2MV(mf.mesh_vertex[i]).position.z(),z)){
            middle.push_back(idx2MV(mf.mesh_vertex[i]));
        } else
            lower.push_back(idx2MV(mf.mesh_vertex[i]));
    }

    std::vector<MeshVertex> majority;
    std::vector<MeshVertex> minority;

    if (upper.size() == 2 && lower.size() == 1){
        majority = upper;
        minority = lower;
    } else if (lower.size() == 2 && upper.size() == 1){
        majority = lower;
        minority = upper;
    } else{
        if (lower.size() == 2 && middle.size() == 1){
            //qDebug() << "intersection error 1 in z line";
            return p;
        } else if (upper.size() == 1 && lower.size() == 1 && middle.size() == 1){
            //qDebug() << "intersection error 1 in z line";
            addPoint(middle[0].position.x(), middle[0].position.y(), &p);

            //add intermediate position
            float x_0, y_0;
            x_0 = ((upper[0].position.z()-z)*lower[0].position.x() + (z-lower[0].position.z())*upper[0].position.x())/(upper[0].position.z()-lower[0].position.z());
            y_0 = ((upper[0].position.z()-z)*lower[0].position.y() + (z-lower[0].position.z())*upper[0].position.y())/(upper[0].position.z()-lower[0].position.z());

            addPoint(x_0,y_0,&p);

            return p;
        } else if (middle.size() == 2){
            //qDebug() << "intersection error 2 in z line";
            addPoint(middle[0].position.x(), middle[0].position.y(), &p);
            addPoint(middle[1].position.x(), middle[1].position.y(), &p);
            return p;
        } else if (middle.size() == 3){
            //qDebug() << "intersection error all in one";
            return p;
        }

        /*if (doubleAreSame(getFaceZmin(mf),z))
        //if (getFaceZmin(mf) != z)
            qDebug() << "intersection error at layer "<< getFaceZmax(mf) << getFaceZmin(mf) << z<< upper.size() << lower.size();
        */
        return p;
    }

    float x_0, y_0, x_1, y_1;
    x_0 = (minority[0].position.x() - majority[0].position.x()) \
            * ((z-majority[0].position.z())/(minority[0].position.z()-majority[0].position.z())) \
            + majority[0].position.x();
    y_0 = (minority[0].position.y() - majority[0].position.y()) \
            * ((z-majority[0].position.z())/(minority[0].position.z()-majority[0].position.z())) \
            + majority[0].position.y();
    x_1 = (minority[0].position.x() - majority[1].position.x()) \
            * ((z-majority[1].position.z())/(minority[0].position.z()-majority[1].position.z())) \
            + majority[1].position.x();
    y_1 = (minority[0].position.y() - majority[1].position.y()) \
            * ((z-majority[1].position.z())/(minority[0].position.z()-majority[1].position.z())) \
            + majority[1].position.y();

    addPoint(x_0,y_0, &p);
    addPoint(x_1,y_1, &p);
    return p;
}

/********************** Helper Functions **********************/

float round(float num, int precision)
{
    return floorf(num * pow(10.0f,precision) + .5f)/pow(10.0f,precision);
}

bool doubleAreSame(double a, double b) {
    return std::fabs(a - b) < std::numeric_limits<double>::epsilon();
}

uint32_t vertexHash(QVector3D v) // max build size = 1000mm, resolution = 1 micron
{
    //qDebug() << "vertexHash" << v.x() << v.x()/SlicingConfiguration::vertex_inbound_distance;
    return (uint32_t(((v.x()+SlicingConfiguration::vertex_inbound_distance/2) / SlicingConfiguration::vertex_inbound_distance)) ^\
            (uint32_t(((v.y()+SlicingConfiguration::vertex_inbound_distance/2) / SlicingConfiguration::vertex_inbound_distance)) << 10) ^\
            (uint32_t(((v.z()+SlicingConfiguration::vertex_inbound_distance/2) / SlicingConfiguration::vertex_inbound_distance)) << 20));
}


int Mesh::addFaceVertex(QVector3D v){
    int vertex_idx = -1;
    uint32_t vertex_hash = vertexHash(v);

    QList<MeshVertex> hashed_points = vertices_hash.values(vertex_hash);
    for(unsigned int idx = 0; idx < hashed_points.size(); idx++)
    {

        if (vertexDistance(vertices[hashed_points.at(idx).idx].position, v)<=SlicingConfiguration::vertex_inbound_distance*SlicingConfiguration::vertex_inbound_distance)
        {
            return hashed_points.at(idx).idx;
        }
    }

    MeshVertex mv = MeshVertex(v);
    mv.idx = vertices.size();
    vertices.emplace_back(mv);
    vertices_hash.insert(vertex_hash, mv);
    vertex_idx = mv.idx;
    updateMinMax(v);
    return vertex_idx;
}


std::array<float,6> Mesh::calculateMinMax(QMatrix4x4 rotmatrix)const {
    qDebug()<< "calculate minmax";
    std::array<float,6> minmax{99999};
    int numberofVertices = vertices.size();
    int numberofFaces = faces.size();
    QVector4D tmpVertex;
    QVector3D tmpVertex2;
    for (int i=0;i<numberofVertices;i++){
        if (i%100 == 0)
            QCoreApplication::processEvents();
        tmpVertex =vertices[i].position.toVector4D();
        QVector3D v = QVector3D(QVector4D::dotProduct(tmpVertex,rotmatrix.column(0)), QVector4D::dotProduct(tmpVertex,rotmatrix.column(1)), QVector4D::dotProduct(tmpVertex,rotmatrix.column(2)));
        updateMinMax(v, minmax);
    }
    return minmax;
}

void Mesh::updateMinMax(QVector3D v)
{
    if (v.x() > _x_max || _x_max == 99999)
        _x_max = v.x();
    if (v.x() < _x_min || _x_min == 99999)
        _x_min = v.x();
    if (v.y() > _y_max || _y_max == 99999)
        _y_max = v.y();
    if (v.y() < _y_min || _y_min == 99999)
        _y_min = v.y();
    if (v.z() > _z_max || _z_max == 99999)
        _z_max = v.z();
    if (v.z() < _z_min || _z_min == 99999)
        _z_min = v.z();
}


void Mesh::updateMinMax(QVector3D v, std::array<float,6>& minMax){
//array order: _x_min, _x_max...._z_min, _z_max
    if (v.x() < minMax[0] || minMax[0] == 99999)
        minMax[0] = v.x();
    if (v.x() > minMax[1] || minMax[1] == 99999)
        minMax[1] = v.x();
    if (v.y() < minMax[2] || minMax[2] == 99999)
        minMax[2] = v.y();
    if (v.y() > minMax[3] || minMax[3] == 99999)
        minMax[3] = v.y();
    if (v.z() < minMax[4] || minMax[4] == 99999)
        minMax[4] = v.z();
    if (v.z() > minMax[5] || minMax[5] == 99999)
        minMax[5] = v.z();
}

// find face containing 2 vertices presented as arguments
std::vector<size_t> Mesh::findFaceWith2Vertices(size_t v0_idx, size_t v1_idx, MeshFace self_f){
    std::vector<size_t> candidates;
    for (size_t index: vertices[v0_idx].connected_faces){
		MeshFace* f = &(faces[index]);
        if (f->mesh_vertex[0] == self_f.mesh_vertex[0] && f->mesh_vertex[1] == self_f.mesh_vertex[1] && f->mesh_vertex[2] == self_f.mesh_vertex[2]){
            continue;
        }
        if (f->mesh_vertex[0] == v1_idx || f->mesh_vertex[1] == v1_idx || f->mesh_vertex[2] == v1_idx){
            candidates.emplace_back(index);
        }
    }
    return candidates;
}

float Mesh::getFaceZmin(MeshFace mf)const{
    float face__z_min=1000;//scfg->max_buildsize_x;
    for (int i=0; i<3; i++){
        float temp__z_min = idx2MV(mf.mesh_vertex[i]).position.z();
        if (temp__z_min<face__z_min)
            face__z_min = temp__z_min;
    }
    return face__z_min;
}

float Mesh::getFaceZmax(MeshFace mf)const{
    float face__z_max=-1000;//-scfg->max_buildsize_x;
    for (int i=0; i<3; i++){
        float temp__z_max = idx2MV(mf.mesh_vertex[i]).position.z();
        if (temp__z_max>face__z_max)
            face__z_max = temp__z_max;
    }
    return face__z_max;
}
QTime Mesh::getTime()const
{
    return time;
}

const Mesh* Mesh::getPrev()const
{
    return prevMesh;
}
const Mesh* Mesh::getNext()const
{
    return nextMesh;
}

MeshFace Mesh::idx2MF(int idx)const{
    return faces[idx];
}

MeshVertex Mesh::idx2MV(int idx)const{
    return vertices[idx];
}

float vertexDistance(QVector3D a, QVector3D b){
    QVector3D dv = a-b;
    float distance = dv.x()*dv.x() + dv.y()*dv.y() + dv.z()*dv.z();
    return distance;
}

QHash<uint32_t, Path>::iterator findSmallestPathHash(QHash<uint32_t, Path> pathHash){
    //QHashIterator<uint32_t, Path> j(pathHash);
    //QHashIterator<uint32_t, Path> prev_j(pathHash);
    QHash<uint32_t, Path>::iterator i = pathHash.begin();
    QHash<uint32_t, Path>::iterator biggest_i = pathHash.begin();
    for (i = pathHash.begin(); i!= pathHash.end(); ++i){
        if (biggest_i.value().size() >= i.value().size())
            biggest_i = i;
    }
    return biggest_i;
}

// construct closed contour using segments created from meshSlice step
Paths contourConstruct(Paths pathList){
    Paths contourList;

    QHash<uint32_t, Path> pathHash;

    if (pathList.size() == 0)
        return contourList;

    int pathCnt = 0;
    for (int i=0; i<pathList.size(); i++){
        pathCnt += pathList[i].size();
    }
    qDebug() << pathCnt;
    pathHash.reserve(pathCnt*10);

    int debug_count=0;

    // pathHash Construction
    for (int i=0; i<pathList.size(); i++){
        Path p = pathList[i];
        //insertPathHash(pathHash, p[0], p[1]); // inserts opposite too

        if (p[0] == p[1])
            continue;

        uint32_t path_hash_u = intPoint2Hash(p[0]);
        uint32_t path_hash_v = intPoint2Hash(p[1]);

        if (! pathHash.contains(path_hash_u)){
            debug_count ++;
            pathHash[path_hash_u].push_back(p[0]);
        }
        if (! pathHash.contains(path_hash_v)){
            debug_count ++;
            pathHash[path_hash_v].push_back(p[1]);
        }
        pathHash[path_hash_u].push_back(p[1]);
        pathHash[path_hash_v].push_back(p[0]);
    }

    // remove duplicate IntPoint Loop xyzzw (5) / zww (3)
    QHashIterator<uint32_t, Path> i(pathHash);
    while (i.hasNext()) {
        i.next();
        if (i.value().size() == 3 && i.value()[1] == i.value()[2]){
            qDebug() << "same one found ";
            for (IntPoint ip : i.value()){
                qDebug() << "ip value : " << ip.X << ip.Y;
            }
            std::vector<IntPoint>* dest = &(pathHash[intPoint2Hash(i.value()[1])]);
            std::vector<IntPoint>::iterator dest_it = dest->begin();
            while ( dest_it != dest->end()){
                if ((*dest_it) == i.value()[0])
                    dest_it = dest->erase(dest_it);
                else
                    ++dest_it;
            }

            if (dest->size() == 1){
                pathHash.remove(intPoint2Hash(*(dest->begin())));
            }
            pathHash.remove(intPoint2Hash(i.value()[0]));
        }
    }

    // Build Polygons
    while(pathHash.size() >0){
        Path contour;
        IntPoint start, pj_prev, pj, pj_next, last;

        QHash<uint32_t, Path>::iterator smallestPathHash = pathHash.begin();//findSmallestPathHash(pathHash);
        if (smallestPathHash.value().size() == 0){
            pathHash.erase(smallestPathHash);
            break;
        }
        pj_prev = smallestPathHash.value()[0];
        start = pj_prev;
        contour.push_back(pj_prev);
        std::vector<IntPoint>* dest = &(smallestPathHash.value());

        if (dest->size() == 0){
            qDebug() << "dest->size() == 0";
            pathHash.remove(intPoint2Hash(pj_prev));
            continue;
        } else if (dest->size() == 1) {
            qDebug() << "dest->size() == 1";
            pathHash.remove(intPoint2Hash(pj_prev));
            continue;
        } else if (dest->size() ==2){
            qDebug() << "dest->size() == 2";
            pj = (*dest)[1];
            last = (*dest)[0]; // pj_prev itself
            pathHash.remove(intPoint2Hash(pj_prev));
        } else {
            pj = (*dest)[1];
            last = (*dest)[2];

            dest->erase(dest->begin()+1);
            dest->erase(dest->begin()+1);
            if (dest->size() == 1){
                pathHash.remove(intPoint2Hash(pj_prev));
                //pj_next = last;
                //contour.push_back(pj);
            }
        }
        while(pj_next != last){
            contour.push_back(pj);
            dest = &(pathHash[intPoint2Hash(pj)]);

            if (dest->size() == 0){
                qDebug() << "dest->size() == 0 from loop";
                pathHash.remove(intPoint2Hash(pj));
                break;
            } else if (dest->size() == 1){
                qDebug() << "dest->size() == 1 from loop";
                pathHash.remove(intPoint2Hash(pj));
                break;
            } else if (dest->size() == 2){
                qDebug() << "dest->size() == 2 from loop";
                start = (*dest)[0]; // itself
                /*uint32_t endHash = intPoint2Hash((*dest)[1]);
                if (pathHash.contains(endHash))
                    pathHash.remove(endHash);*/ // maybe needless

                pathHash.remove(intPoint2Hash(pj));
                pj_next = last;
                pj = pj_next;
                pj_prev = contour[0];
                last = start;
                reverse(contour.begin(), contour.end());
                continue;
            }

            // find pj_prev and choose another pj_next and remove pj_prev, pj_next from H[pj]
            for (int d=1; d<dest->size(); d++){
                if ((*dest)[d] == pj_prev){
                    dest->erase(dest->begin()+d);
                    break;
                }
            }

            pj_next = (*dest)[1];
            dest->erase(dest->begin()+1);
            if (dest->size() == 1)
                pathHash.remove(intPoint2Hash(pj));


            pj_prev = pj;
            pj = pj_next;
        }

        contour.push_back(last);
        contour.push_back(start);

        // collect last vertex's connectedness
        uint32_t last_hash = intPoint2Hash(last);
        if (pathHash.contains(last_hash)){
            dest = &(pathHash[last_hash]);
            for (int d=1; d<dest->size(); d++){
                if ((*dest)[d] == pj_prev){
                    dest->erase(dest->begin()+d);
                    break;
                }
            }
            for (int d=1; d<dest->size(); d++){
                if ((*dest)[d] == start){
                    dest->erase(dest->begin()+d);
                    break;
                }
            }
            if (dest->size() == 1)
                pathHash.remove(last_hash);
        }

        // remove 2-vertices-contours
        if (contour.size() == 2)
            continue;

        /*if (Orientation(contour)){
            ReversePath(contour);
        }*/

        contourList.push_back(contour);
    }


    return contourList;
}

void findAndDeleteHash(std::vector<uint32_t>* hashList, uint32_t hash){
    for (std::vector<uint32_t>::iterator h_it = hashList->begin(); h_it != hashList->end();){
        if (*h_it == hash){
            hashList->erase(h_it);
            break;
        }
        h_it ++;
    }
}

bool contourContains(Path3D contour, MeshVertex mv){
    for (MeshVertex cmv : contour){
        if (cmv == mv)
            return true;
    }
    return false;
}

bool listContains(std::vector<uint32_t>* hashList, uint32_t hash){
    for (std::vector<uint32_t>::iterator h_it = hashList->begin(); h_it != hashList->end();){
        if (*h_it == hash){
            return true;
        }
        h_it ++;
    }
    return false;
}

MeshVertex findAvailableMeshVertex(
        QHash<uint32_t, Path3D>* pathHash,
        std::vector<uint32_t>* hashList,
        MeshVertex start
        ){
    int pj_idx = 1;
    MeshVertex pj = (*pathHash)[meshVertex2Hash(start)].at(pj_idx);

    /*qDebug() << "finding availableMeshVertex from : " << (*pathHash)[meshVertex2Hash(start)].size()-1;

    // choose from only available vertex ( in the hash list )
    for (Path3D::iterator p_it = (*pathHash)[meshVertex2Hash(start)].begin(); p_it != (*pathHash)[meshVertex2Hash(start)].end();){

    }

    while (listContains(hashList, meshVertex2Hash(pj))){
        qDebug() << "searching available vertex " << pj.position << meshVertex2Hash(pj);
        if (pj_idx > (*pathHash)[meshVertex2Hash(start)].size()-1){
            qDebug() << "no new pj available";
            pj.idx = -1;
            return pj;
        } else {
            (*pathHash)[meshVertex2Hash(start)].erase((*pathHash)[meshVertex2Hash(start)].begin()+pj_idx);
            return pj;
        }
        pj_idx ++;
        pj = (*pathHash)[meshVertex2Hash(start)].at(pj_idx);
    }*/

    (*pathHash)[meshVertex2Hash(start)].erase((*pathHash)[meshVertex2Hash(start)].begin()+pj_idx);
    return pj;
}

MeshVertex findAvailableMeshVertexFromContour(QHash<uint32_t, Path3D>* pathHash, std::vector<uint32_t>* hashList, Path3D* contour){
    for (auto& mv : *contour){
        if ((*pathHash)[meshVertex2Hash(mv)].size()>=3){
            return (*pathHash)[meshVertex2Hash(mv)].at(1);
        }
    }
    MeshVertex temp_mv;
    temp_mv.idx = -1;
    return temp_mv;
}


// construct closed contour using segments created from identify step
Paths3D contourConstruct3D(Paths3D hole_edges){
    int iter = 0;
    std::vector<Paths3D::iterator> checked_its;
    bool dirty = true; // checks if iteration erased some contour

    while (dirty){
        dirty = false;
        qDebug() << iter;
        Paths3D::iterator hole_edge1_it;
        Paths3D::iterator hole_edge2_it;

        qDebug() << "hole edge size : " << hole_edges.size() << int(hole_edges.end() - hole_edges.begin());
        /*int cnt1 =0, cnt2 =0;
        for (hole_edge1_it = hole_edges.begin(); hole_edge1_it != hole_edges.end(); ++hole_edge1_it){
            cnt2 = 0;
            for (hole_edge2_it = hole_edges.begin(); hole_edge2_it != hole_edges.end(); ++hole_edge2_it){
                qDebug() << "checking" << cnt1 << cnt2;
                cnt2 ++;
            }
            cnt1 ++;
        }*/
        int edge_lookup = 0;
        for (hole_edge1_it = hole_edges.begin(); hole_edge1_it != hole_edges.end();){
            if (edge_lookup %50 ==0)
                QCoreApplication::processEvents();
            edge_lookup ++;

            bool checked = false;
            for (Paths3D::iterator checked_it : checked_its){
                if (checked_it == hole_edge1_it){
                    checked = true;
                }
            }
            if (checked){
                hole_edge1_it ++;
                continue;
            }


            for (hole_edge2_it = hole_edges.begin(); hole_edge2_it != hole_edges.end();){
                checked = false;
                for (Paths3D::iterator checked_it : checked_its){
                    if (checked_it == hole_edge2_it){
                        checked = true;
                    }
                }
                if (checked){
                    hole_edge2_it ++;
                    continue;
                }

                if (hole_edges.size() == 1)
                    return hole_edges;
                if (hole_edge1_it == hole_edge2_it){
                    //qDebug() << "same edge it";
                    hole_edge2_it ++;
                    continue;
                }
                // prolong hole_edge 1 if end and start matches
                if ((hole_edge1_it->end()-1)->position.distanceToPoint(hole_edge2_it->begin()->position) < scfg->vertex_inbound_distance*0.05/scfg->resolution){
                //if (meshVertex2Hash(*(hole_edge1_it->end()-1)) == meshVertex2Hash(*hole_edge2_it->begin())){
                    //qDebug() << "erase";
                    dirty = true;
                    hole_edge1_it->insert(hole_edge1_it->end(), hole_edge2_it->begin()+1, hole_edge2_it->end());
                    checked_its.push_back(hole_edge2_it);
                    //hole_edge2_it = hole_edges.erase(hole_edge2_it);
                    //qDebug() << "erased";
                } else if ((hole_edge1_it->end()-1)->position.distanceToPoint((hole_edge2_it->end()-1)->position) < scfg->vertex_inbound_distance*0.05/scfg->resolution){
                //} else if (meshVertex2Hash(*(hole_edge1_it->end()-1)) == meshVertex2Hash(*(hole_edge2_it->end()-1))){
                    //qDebug() << "erase";
                    dirty = true;
                    std::reverse(hole_edge2_it->begin(), hole_edge2_it->end());

                    hole_edge1_it->insert(hole_edge1_it->end(), hole_edge2_it->begin()+1, hole_edge2_it->end());
                    checked_its.push_back(hole_edge2_it);
                    //hole_edge2_it = hole_edges.erase(hole_edge2_it);
                    //qDebug() << "erased";
                }
                hole_edge2_it ++;
            }
            hole_edge1_it ++;
        }
        qDebug() << "hole edges size " << hole_edges.size();
    }

    // select contour if size > 2
    Paths3D result_edges;

    for (Paths3D::iterator hole_edge_it = hole_edges.begin(); hole_edge_it != hole_edges.end();){
        bool checked = false;
        for (Paths3D::iterator checked_it : checked_its){
            if (checked_it == hole_edge_it){
                checked = true;
            }
        }
        if (!checked){
            result_edges.push_back(*hole_edge_it);
            qDebug() << "result_edge : " << hole_edge_it->begin()->position << (hole_edge_it->end()-1)->position;
        }
        hole_edge_it ++;
    }

    for (Path3D result_edge : result_edges){
        qDebug() << "hole_edge size : " << result_edge.size();
    }

    /*for (Path3D hole_edge : hole_edges){
        if (hole_edge.size() > 2){
            result_edges.push_back(hole_edge);
            qDebug() << "hole_edge size : " << hole_edge.size();
        }
    }*/

    qDebug() << "result edges : " << result_edges.size();

    return result_edges;
    /*// new trial if there's 분기점
    Paths3D contourList;

    QHash<uint32_t, Path3D> pathHash;
    std::vector<uint32_t> hashList;
    hashList.reserve(hole_edges.size());

    if (hole_edges.size() == 0)
        return contourList;

    int pathCnt = 0;
    for (int i=0; i<hole_edges.size(); i++){
        pathCnt += hole_edges[i].size();
    }
    qDebug() << pathCnt;
    pathHash.reserve(pathCnt*10);

    int debug_count=0;

    for (int i=0; i<hole_edges.size(); i++){
        Path3D p = hole_edges[i];
        //insertPathHash(pathHash, p[0], p[1]); // inserts opposite too

        uint32_t path_hash_u = meshVertex2Hash(p[0]);
        uint32_t path_hash_v = meshVertex2Hash(p[1]);

        if (! pathHash.contains(path_hash_u)){
            debug_count ++;
            pathHash[path_hash_u].reserve(10);
            pathHash[path_hash_u].push_back(p[0]);
            hashList.push_back(path_hash_u);
        }
        if (! pathHash.contains(path_hash_v)){
            debug_count ++;
            pathHash[path_hash_v].reserve(10);
            pathHash[path_hash_v].push_back(p[1]);
            hashList.push_back(path_hash_v);
        }
        pathHash[path_hash_u].push_back(p[1]);
        pathHash[path_hash_v].push_back(p[0]);
    }

    for (uint32_t hash : hashList){
        qDebug() << "hash " <<hash << " path size : "<< pathHash[hash].size();
    }

    qDebug() << "hashList.size : " << hashList.size();

    while (pathHash.size() > 0){
        qDebug() << "new contour start" << "chosen from "<< hashList.size();
        MeshVertex start, pj_prev, pj, pj_next, last, u, v;
        Path3D contour;
        start = pathHash[hashList[0]][0];
        contour.push_back(start);
        qDebug() << "inserted pj : " << start.position;
        pj_prev = start;
        Path3D* dest = &(pathHash[hashList[0]]);
        qDebug() << "new contour dest size : " << dest->size();

        if (pathHash[meshVertex2Hash(start)].size() <= 2){
            pathHash.remove(meshVertex2Hash(start));
            findAndDeleteHash(&hashList, meshVertex2Hash(start));
            continue;
        }

        pj = findAvailableMeshVertex(&pathHash, &hashList, start);
        last = findAvailableMeshVertex(&pathHash, &hashList, start);

        qDebug() << "current selected pj : " << pj.position;
        qDebug() << "current selected last : " << last.position;

        if (pathHash[meshVertex2Hash(start)].size() <= 2){
            pathHash.remove(meshVertex2Hash(start));
            findAndDeleteHash(&hashList, meshVertex2Hash(start));
        }

        //while (!contourContains(contour, pj)){
        while (pj != last){
            contour.push_back(pj);
            qDebug() << "inserted pj : " << pj.position;
            qDebug() << "current contour size :" << contour.size();
            for (MeshVertex mv : pathHash[meshVertex2Hash(pj)]){
                qDebug() << "current adding meshvertex neighbors : "<< mv.position;
            }

            if (pathHash[meshVertex2Hash(pj)].size() <= 2){
                pathHash.remove(meshVertex2Hash(pj));
                findAndDeleteHash(&hashList, meshVertex2Hash(pj));
                break;
            }

            u = findAvailableMeshVertex(&pathHash, &hashList, pj);
            v = findAvailableMeshVertex(&pathHash, &hashList, pj);

            if (pathHash[meshVertex2Hash(pj)].size() <= 2){
                pathHash.remove(meshVertex2Hash(pj));
                findAndDeleteHash(&hashList, meshVertex2Hash(pj));
            }

            qDebug() << "current selected u : " << u.position;
            qDebug() << "current selected v : " << v.position;

            if (u == pj_prev){
                pj_next = v;
            } else {
                pj_next = u;
            }
            pj_prev = pj;
            pj = pj_next;

            if (contourContains(contour, pj)){
                Path3D new_contour;
                Path3D::iterator new_start;

                for (Path3D::iterator p_it = contour.begin(); p_it != contour.end();){
                    if (*p_it == pj){
                        new_start = p_it;
                        break;
                    }
                    p_it ++;
                }

                new_contour.insert(new_contour.end(), new_start, contour.end());
                new_contour.push_back(pj);
                contourList.push_back(new_contour);
                contour = Path3D(contour.begin(), new_start);
            }
        }

        uint32_t lastHash = meshVertex2Hash(pj);
        for (Path3D::iterator mv_it = pathHash[lastHash].begin(); mv_it != pathHash[lastHash].end();){
            if (*mv_it == pj_prev || *mv_it == start)
                mv_it = pathHash[lastHash].erase(mv_it);
            else
                mv_it ++;
        }

        if (pathHash[meshVertex2Hash(pj)].size() <= 2){
            pathHash.remove(meshVertex2Hash(pj));
            findAndDeleteHash(&hashList, meshVertex2Hash(pj));
        }

        contour.push_back(pj);
        contourList.push_back(contour);
    }

    return contourList;*/
}

bool intPointInPath(IntPoint ip, Path p){
    for (IntPoint i : p){
        if ((ip.X == i.X) && (ip.Y == i.Y)){
            return true;
        }
    }
    return false;
};

bool pathInpath(Path3D target, Path3D in){
    for (IntPoint ip : target.projection){
        if (PointInPolygon(ip, in.projection)==0){ // if ip is not contained in in
            return false;
        }
    }
    return true;
}

std::vector<std::array<QVector3D, 3>> interpolate(Path3D from, Path3D to){
    std::vector<std::array<QVector3D, 3>> result_faces;
    if (from.size() != to.size()){
        qDebug() << "from and to size differs";
        return result_faces;
    }
    if (from.size() <3){
        qDebug() << "from path size small :" << from.size();
        return result_faces;
    }

    result_faces.reserve(from.size()*2);

    qDebug() << from.size() << to.size();

    // add odd number faces
    for (int i=1; i<from.size()-1; i++){
        if (i%2 != 0){
            // add right left new face
            std::array<QVector3D,3> temp_face1;
            temp_face1[0] = to[i].position;
            temp_face1[1] = from[i-1].position;
            temp_face1[2] = from[i].position;
            result_faces.push_back(temp_face1);
            std::array<QVector3D,3> temp_face2;
            temp_face2[0] = to[i].position;
            temp_face2[1] = from[i].position;
            temp_face2[2] = from[i+1].position;
            result_faces.push_back(temp_face2);
        }
    }
    qDebug() << "reserved odd number faces" << result_faces.size();

    // add even number faces
    for (int i=2; i<from.size()-1; i++){
        if (i%2 == 0){
            // add right left new face
            std::array<QVector3D,3> temp_face1;
            temp_face1[0] = to[i].position;
            temp_face1[1] = from[i-1].position;
            temp_face1[2] = from[i].position;
            result_faces.push_back(temp_face1);
            std::array<QVector3D,3> temp_face2;
            temp_face2[0] = to[i].position;
            temp_face2[1] = from[i].position;
            temp_face2[2] = from[i+1].position;
            result_faces.push_back(temp_face2);
        }
    }
    qDebug() << "reserved even number faces" << result_faces.size();

    // add right left new face
    std::array<QVector3D,3> temp_face1;
    temp_face1[0] = to[from.size()-1].position;
    temp_face1[1] = from[from.size()-2].position;
    temp_face1[2] = from[from.size()-1].position;
    result_faces.push_back(temp_face1);
    std::array<QVector3D,3> temp_face2;
    temp_face2[0] = to[from.size()-1].position;
    temp_face2[1] = from[from.size()-1].position;
    temp_face2[2] = from[0].position;
    result_faces.push_back(temp_face2);

    return result_faces;
}

/************** Helper Functions *****************/

uint32_t intPoint2Hash(IntPoint u){
    QVector3D u_qv3 = QVector3D(u.X, u.Y, 0);
    uint32_t path_hash_u = vertexHash(u_qv3);
    return path_hash_u;
}

uint32_t meshVertex2Hash(MeshVertex u){
    uint32_t path_hash_u = vertexHash(u.position);
    return path_hash_u;
}


const std::vector<MeshVertex>* Mesh::getVertices()const
{
    return &vertices;
}
const std::vector<MeshFace>* Mesh::getFaces()const
{
    return &faces;
}

float Mesh::x_min()const
{
    return _x_min;
}
float Mesh::x_max()const
{
    return _x_max;

}
float Mesh::y_min()const
{
    return _y_min;

}
float Mesh::y_max()const
{
    return _y_max;

}
float Mesh::z_min()const
{
    return _z_min;
}
float Mesh::z_max()const
{
    return _z_max;

}
