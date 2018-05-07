#include "mesh.h"
#include <QDebug>
#include <QCoreApplication>

/********************** Mesh Edit Functions***********************/
void Mesh::vertexOffset(float factor){
    int numberofVertices = vertices.size();
    x_min = 99999;
    x_max = 99999;
    y_min = 99999;
    y_max = 99999;
    z_min = 99999;
    z_max = 99999;
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
    x_min = 99999;
    x_max = 99999;
    y_min = 99999;
    y_max = 99999;
    z_min = 99999;
    z_max = 99999;
    for (int i=0;i<numberofVertices;i++){
        if (i%100 == 0)
            QCoreApplication::processEvents();
        QVector3D tmp = direction+vertices[i].position;
        vertices[i].position = tmp;
        updateMinMax(vertices[i].position);
    }
}

void Mesh::vertexRotate(QMatrix4x4 tmpmatrix){
    int numberofVertices = vertices.size();
    int numberofFaces = faces.size();
    x_min = 99999;
    x_max = 99999;
    y_min = 99999;
    y_max = 99999;
    z_min = 99999;
    z_max = 99999;
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
            vertices[i].vn = QVector3D(vertices[i].connected_faces[0]->fn + vertices[i].connected_faces[1]->fn + vertices[i].connected_faces[2]->fn).normalized();
        } else {
            vertices[i].vn = QVector3D(0,0,0);
        }
    }
}

void Mesh::vertexScale(float scale){
    int numberofVertices = vertices.size();
    x_min = 99999;
    x_max = 99999;
    y_min = 99999;
    y_max = 99999;
    z_min = 99999;
    z_max = 99999;
    for (int i=0;i<numberofVertices;i++){
        if (i%100 == 0)
            QCoreApplication::processEvents();
        QVector3D tmp;
        tmp.setX(vertices[i].position.x() * scale);
        tmp.setY(vertices[i].position.y() * scale);
        tmp.setZ(vertices[i].position.z() * scale);
        vertices[i].position = tmp;
        updateMinMax(vertices[i].position);
    }
}
/********************** Mesh Generation Functions **********************/

void Mesh::addFace(QVector3D v0, QVector3D v1, QVector3D v2){
    addFace(v0,v1,v2,-1);
}

void Mesh::addFace(QVector3D v0, QVector3D v1, QVector3D v2, int parent_idx){
    int v0_idx = getVertexIdx(v0);
    int v1_idx = getVertexIdx(v1);
    int v2_idx = getVertexIdx(v2);
//
    MeshFace* mf = new MeshFace();

    vector<MeshFace*> nf1;
    vector<MeshFace*> nf2;
    vector<MeshFace*> nf3;
    mf->neighboring_faces.push_back(nf1);
    mf->neighboring_faces.push_back(nf2);
    mf->neighboring_faces.push_back(nf3);

    int new_idx = faces.size();
    mf->idx = new_idx;
    mf->parent_idx = parent_idx;
    mf->mesh_vertex[0] = v0_idx;
    mf->mesh_vertex[1] = v1_idx;
    mf->mesh_vertex[2] = v2_idx;

    mf->fn = QVector3D::normal(vertices[v0_idx].position, vertices[v1_idx].position, vertices[v2_idx].position);
    mf->fn_unnorm = QVector3D::crossProduct(vertices[v1_idx].position-vertices[v0_idx].position,vertices[v2_idx].position-vertices[v0_idx].position);
    faces.emplace_back(*mf);

    vertices[v0_idx].connected_faces.emplace_back(& faces[new_idx]);
    vertices[v1_idx].connected_faces.emplace_back(& faces[new_idx]);
    vertices[v2_idx].connected_faces.emplace_back(& faces[new_idx]);


    if (vertices[v0_idx].connected_faces.size()>=3){
        MeshVertex &mv = vertices[v0_idx];
        mv.vn = QVector3D(mv.connected_faces[0]->fn + mv.connected_faces[1]->fn + mv.connected_faces[2]->fn).normalized();
    } else {
        MeshVertex &mv = vertices[v0_idx];
        mv.vn = QVector3D(0,0,0);
    }

    if (vertices[v1_idx].connected_faces.size()>=3){
        MeshVertex &mv = vertices[v1_idx];
        mv.vn = QVector3D(mv.connected_faces[0]->fn + mv.connected_faces[1]->fn + mv.connected_faces[2]->fn).normalized();
    } else {
        MeshVertex &mv = vertices[v1_idx];
        mv.vn = QVector3D(0,0,0);
    }

    if (vertices[v2_idx].connected_faces.size()>=3){
        MeshVertex &mv = vertices[v2_idx];
        mv.vn = QVector3D(mv.connected_faces[0]->fn + mv.connected_faces[1]->fn + mv.connected_faces[2]->fn).normalized();
    } else {
        MeshVertex &mv = vertices[v2_idx];
        mv.vn = QVector3D(0,0,0);
    }
}

vector<MeshFace>::iterator Mesh::removeFace(vector<MeshFace>::iterator f_it){
    MeshFace &mf = (*f_it);
    //MeshFace &mf = faces[f_idx];
    //vector<MeshFace>::iterator f_idx_it = faces.begin()+f_idx;

    MeshVertex &mv0 = vertices[mf.mesh_vertex[0]];
    MeshVertex &mv1 = vertices[mf.mesh_vertex[1]];
    MeshVertex &mv2 = vertices[mf.mesh_vertex[2]];

    // remove f_it face from its neighboring faces' neighboring faces list
    for (vector<MeshFace*> nfs : mf.neighboring_faces){
        for (vector<MeshFace*>::iterator nf_ptr_it = nfs.begin(); nf_ptr_it != nfs.end();){
        //for (MeshFace* nf_ptr : nfs){
            MeshFace* nf_ptr = (*nf_ptr_it);
            if (nf_ptr->neighboring_faces.size() != 3){ // already deleted meshface
                nf_ptr_it = nfs.erase(nf_ptr_it);
                continue;
            }
            for (int i=0; i<3; i++){
                vector<MeshFace*>::iterator nf_nf_ptr_it = nf_ptr->neighboring_faces[i].begin();
                while (nf_nf_ptr_it != nf_ptr->neighboring_faces[i].end()){
                    MeshFace* nf_nf_ptr = (*nf_nf_ptr_it);
                    if (nf_nf_ptr == &mf) {
                        nf_nf_ptr_it = nf_ptr->neighboring_faces[i].erase(nf_nf_ptr_it);
                        break;
                    } else {
                        nf_nf_ptr_it ++;
                    }
                }
            }
            ++nf_ptr_it;
        }
    }


    if (mf.mesh_vertex[0] == mf.mesh_vertex[1] && mf.mesh_vertex[1] == mf.mesh_vertex[2]){
        for (int c_idx=0; c_idx < mv1.connected_faces.size(); c_idx ++){
            MeshFace &cf = (*mv1.connected_faces[c_idx]);
            for (int v_idx=0; v_idx<3; v_idx ++){
                if (cf.mesh_vertex[v_idx] == mf.mesh_vertex[1])
                    cf.mesh_vertex[v_idx] = mf.mesh_vertex[0];
            }
        }

        for (int c_idx=0; c_idx < mv2.connected_faces.size(); c_idx ++){
            MeshFace &cf = (*mv2.connected_faces[c_idx]);
            for (int v_idx=0; v_idx<3; v_idx ++){
                if (cf.mesh_vertex[v_idx] == mf.mesh_vertex[2])
                    cf.mesh_vertex[v_idx] = mf.mesh_vertex[0];
            }
        }
    } else if (mf.mesh_vertex[0] == mf.mesh_vertex[1]){ // replace 1 vertices in connected faces of 1 by 0
        for (int c_idx=0; c_idx < mv1.connected_faces.size(); c_idx ++){
            MeshFace &cf = (*mv1.connected_faces[c_idx]);
            for (int v_idx=0; v_idx<3; v_idx ++){
                if (cf.mesh_vertex[v_idx] == mf.mesh_vertex[1])
                    cf.mesh_vertex[v_idx] = mf.mesh_vertex[0];
            }
        }
    } else if (mf.mesh_vertex[1] == mf.mesh_vertex[2]){ // replace 2 by 1
        for (int c_idx=0; c_idx < mv2.connected_faces.size(); c_idx ++){
            MeshFace &cf = (*mv2.connected_faces[c_idx]);
            for (int v_idx=0; v_idx<3; v_idx ++){
                if (cf.mesh_vertex[v_idx] == mf.mesh_vertex[2])
                    cf.mesh_vertex[v_idx] = mf.mesh_vertex[1];
            }
        }
    } else if (mf.mesh_vertex[2] == mf.mesh_vertex[0]){ // replace 0 by 2
        for (int c_idx=0; c_idx < mv0.connected_faces.size(); c_idx ++){
            MeshFace &cf = (*mv0.connected_faces[c_idx]);
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
    for (vector<MeshFace>::iterator it = faces.begin(); it!= faces.end(); it++){
        if (cnt % 100 == 0){
            QCoreApplication::processEvents();
        }
        cnt ++;

        MeshFace &mf = (*it);

        // clear neighboring faces
        mf.neighboring_faces[0].clear();
        mf.neighboring_faces[1].clear();
        mf.neighboring_faces[2].clear();

        vector<MeshFace*> faces1 = findFaceWith2Vertices(mf.mesh_vertex[0], mf.mesh_vertex[1], mf);
        vector<MeshFace*> faces2 = findFaceWith2Vertices(mf.mesh_vertex[1], mf.mesh_vertex[2], mf);
        vector<MeshFace*> faces3 = findFaceWith2Vertices(mf.mesh_vertex[2], mf.mesh_vertex[0], mf);

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
    path->push_back(ip);
}

Path3D Mesh::intersectionPath(Plane base_plane, Plane target_plane) {
    Path3D p;

    vector<QVector3D> upper;
    vector<QVector3D> lower;
    for (int i=0; i<3; i++){
        if (target_plane[i].distanceToPlane(base_plane[0],base_plane[1],base_plane[2]) >0){
            upper.push_back(target_plane[i]);
        } else {
            lower.push_back(target_plane[i]);
        }
    }

    vector<QVector3D> majority;
    vector<QVector3D> minority;

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

Path Mesh::intersectionPath(MeshFace mf, float z){
    Path p;

    vector<MeshVertex> upper;
    vector<MeshVertex> lower;
    for (int i=0; i<3; i++){
        if (idx2MV(mf.mesh_vertex[i]).position.z() >= z)
            upper.push_back(idx2MV(mf.mesh_vertex[i]));
        else
            lower.push_back(idx2MV(mf.mesh_vertex[i]));
    }

    vector<MeshVertex> majority;
    vector<MeshVertex> minority;

    if (upper.size() == 2){
        majority = upper;
        minority = lower;
    } else if (lower.size() == 2){
        majority = lower;
        minority = upper;
    } else{
        if (getFaceZmin(mf) != z)
            qDebug() << "intersection error at layer "<< getFaceZmax(mf) << getFaceZmin(mf) << z<< upper.size() << lower.size();

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

uint32_t vertexHash(QVector3D v) // max build size = 1000mm, resolution = 1 micron
{
    return (uint32_t((v.x() / SlicingConfiguration::vertex_inbound_distance)) ^\
            (uint32_t((v.y() / SlicingConfiguration::vertex_inbound_distance)) << 10) ^\
            (uint32_t((v.z() / SlicingConfiguration::vertex_inbound_distance)) << 20));
}

int Mesh::getVertexIdx(QVector3D v){
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

// updates mesh's min max
void Mesh::updateMinMax(QVector3D v){
    if (v.x() > x_max || x_max == 99999)
        x_max = v.x();
    if (v.x() < x_min || x_min == 99999)
        x_min = v.x();
    if (v.y() > y_max || y_max == 99999)
        y_max = v.y();
    if (v.y() < y_min || y_min == 99999)
        y_min = v.y();
    if (v.z() > z_max || z_max == 99999)
        z_max = v.z();
    if (v.z() < z_min || z_min == 99999)
        z_min = v.z();
}

// find face containing 2 vertices presented as arguments
vector<MeshFace*> Mesh::findFaceWith2Vertices(int v0_idx, int v1_idx, MeshFace self_f){
    vector<MeshFace*> candidates;
    foreach (MeshFace* f, vertices[v0_idx].connected_faces){
        if (f->mesh_vertex[0] == self_f.mesh_vertex[0] && f->mesh_vertex[1] == self_f.mesh_vertex[1] && f->mesh_vertex[2] == self_f.mesh_vertex[2]){
            continue;
        }
        if (f->mesh_vertex[0] == v1_idx || f->mesh_vertex[1] == v1_idx || f->mesh_vertex[2] == v1_idx){
            candidates.emplace_back(f);
        }
    }
    return candidates;
}

float Mesh::getFaceZmin(MeshFace mf){
    float face_z_min=1000;//scfg->max_buildsize_x;
    for (int i=0; i<3; i++){
        float temp_z_min = idx2MV(mf.mesh_vertex[i]).position.z();
        if (temp_z_min<face_z_min)
            face_z_min = temp_z_min;
    }
    return face_z_min;
}

float Mesh::getFaceZmax(MeshFace mf){
    float face_z_max=-1000;//-scfg->max_buildsize_x;
    for (int i=0; i<3; i++){
        float temp_z_max = idx2MV(mf.mesh_vertex[i]).position.z();
        if (temp_z_max>face_z_max)
            face_z_max = temp_z_max;
    }
    return face_z_max;
}

MeshFace Mesh::idx2MF(int idx){
    return faces[idx];
}

MeshVertex Mesh::idx2MV(int idx){
    return vertices[idx];
}

float vertexDistance(QVector3D a, QVector3D b){
    QVector3D dv = a-b;
    float distance = dv.x()*dv.x() + dv.y()*dv.y() + dv.z()*dv.z();
    return distance;
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

    for (int i=0; i<pathList.size(); i++){
        Path p = pathList[i];
        //insertPathHash(pathHash, p[0], p[1]); // inserts opposite too

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

    // Build Polygons
    while(pathHash.size() >0){
        Path contour;
        IntPoint start, pj_prev, pj, pj_next, last;

        pj_prev = pathHash.begin().value()[0];
        start = pj_prev;
        contour.push_back(pj_prev);
        vector<IntPoint>* dest = &(pathHash.begin().value());

        if (dest->size() == 0|| dest->size() == 1){
            pathHash.remove(intPoint2Hash(pj_prev));
            continue;
        } else if (dest->size() ==2){
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
                pj_next = last;
                contour.push_back(pj);
            }
        }
        while(pj_next != last){
            contour.push_back(pj);
            dest = &(pathHash[intPoint2Hash(pj)]);

            if (dest->size() == 0|| dest->size() == 1){
                pathHash.remove(intPoint2Hash(pj));
                break;
            } else if (dest->size() == 2){
                start = (*dest)[0]; // itself
                /*uint32_t endHash = intPoint2Hash((*dest)[1]);
                if (pathHash.contains(endHash))
                    pathHash.remove(endHash); // maybe needless*/

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

void findAndDeleteHash(vector<uint32_t>* hashList, uint32_t hash){
    for (vector<uint32_t>::iterator h_it = hashList->begin(); h_it != hashList->end();){
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

bool listContains(vector<uint32_t>* hashList, uint32_t hash){
    for (vector<uint32_t>::iterator h_it = hashList->begin(); h_it != hashList->end();){
        if (*h_it == hash){
            return true;
        }
        h_it ++;
    }
    return false;
}

MeshVertex findAvailableMeshVertex(
        QHash<uint32_t, Path3D>* pathHash,
        vector<uint32_t>* hashList,
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

MeshVertex findAvailableMeshVertexFromContour(QHash<uint32_t, Path3D>* pathHash, vector<uint32_t>* hashList, Path3D* contour){
    for (MeshVertex mv : *contour){
        if ((*pathHash)[meshVertex2Hash(mv)].size()>=3){
            return (*pathHash)[meshVertex2Hash(mv)].at(1);
        }
    }
    MeshVertex temp_mv;
    temp_mv.idx = -1;
    return temp_mv;
}


// construct closed contour using segments created from identify step
Paths3D contourConstruct(Paths3D hole_edges){
    int iter = 0;
    vector<Paths3D::iterator> checked_its;
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
        for (hole_edge1_it = hole_edges.begin(); hole_edge1_it != hole_edges.end();){
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
    vector<uint32_t> hashList;
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

bool cpIncp(containmentPath target, containmentPath in){
    for (IntPoint ip : target.projection){
        if (PointInPolygon(ip, in.projection)==0){ // if ip is not contained in in
            return false;
        }
    }
    return true;
}

vector<std::array<QVector3D, 3>> interpolate(Path3D from, Path3D to){
    vector<std::array<QVector3D, 3>> result_faces;
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
            array<QVector3D,3> temp_face1;
            temp_face1[0] = to[i].position;
            temp_face1[1] = from[i-1].position;
            temp_face1[2] = from[i].position;
            result_faces.push_back(temp_face1);
            array<QVector3D,3> temp_face2;
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
            array<QVector3D,3> temp_face1;
            temp_face1[0] = to[i].position;
            temp_face1[1] = from[i-1].position;
            temp_face1[2] = from[i].position;
            result_faces.push_back(temp_face1);
            array<QVector3D,3> temp_face2;
            temp_face2[0] = to[i].position;
            temp_face2[1] = from[i].position;
            temp_face2[2] = from[i+1].position;
            result_faces.push_back(temp_face2);
        }
    }
    qDebug() << "reserved even number faces" << result_faces.size();

    // add right left new face
    array<QVector3D,3> temp_face1;
    temp_face1[0] = to[from.size()-1].position;
    temp_face1[1] = from[from.size()-2].position;
    temp_face1[2] = from[from.size()-1].position;
    result_faces.push_back(temp_face1);
    array<QVector3D,3> temp_face2;
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

