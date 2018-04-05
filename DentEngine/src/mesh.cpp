#include "mesh.h"
#include <QDebug>


/********************** Mesh Edit Functions***********************/
void Mesh::vertexMove(QVector3D direction){
    int numberofVertices = vertices.size();
    x_min = 99999;
    x_max = 99999;
    y_min = 99999;
    y_max = 99999;
    z_min = 99999;
    z_max = 99999;
    for (int i=0;i<numberofVertices;i++){
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
        tmpVertex =vertices[i].position.toVector4D();
        tmpVertex2.setX(QVector4D::dotProduct(tmpVertex,tmpmatrix.column(0)));
        tmpVertex2.setY(QVector4D::dotProduct(tmpVertex,tmpmatrix.column(1)));
        tmpVertex2.setZ(QVector4D::dotProduct(tmpVertex,tmpmatrix.column(2)));
        vertices[i].position = tmpVertex2;
        updateMinMax(vertices[i].position);
    }
    for (int i=0;i<numberofFaces;i++){
        faces[i].fn = QVector3D::normal(vertices[faces[i].mesh_vertex[0]].position,
                                        vertices[faces[i].mesh_vertex[1]].position,
                                        vertices[faces[i].mesh_vertex[2]].position);
        faces[i].fn_unnorm = QVector3D::crossProduct(vertices[faces[i].mesh_vertex[1]].position-vertices[faces[i].mesh_vertex[0]].position,
                                                     vertices[faces[i].mesh_vertex[2]].position-vertices[faces[i].mesh_vertex[0]].position);
    }
    for (int i=0;i<numberofVertices;i++){
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
    vector<MeshFace*>::iterator mf_ptr_it;
    //for ()
    for (vector<MeshFace*> nfs : mf.neighboring_faces){
        for (MeshFace* nf_ptr : nfs){
            for (int i=0; i<3; i++){
                vector<MeshFace*>::iterator nf_nf_ptr_it = nf_ptr->neighboring_faces[i].begin();
                while (nf_nf_ptr_it != nf_ptr->neighboring_faces[i].end()){
                    MeshFace* nf_nf_ptr = (*nf_nf_ptr_it);
                    if (nf_nf_ptr == &mf) {
                        nf_nf_ptr_it = nf_ptr->neighboring_faces[i].erase(nf_nf_ptr_it);
                    } else {
                        nf_nf_ptr_it ++;
                    }
                }
            }
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

    for (vector<MeshFace>::iterator it = faces.begin(); it!= faces.end(); it++){
        MeshFace &mf = (*it);

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
    ip.X = round(x*cfg->resolution);
    ip.Y = round(y*cfg->resolution);
    path->push_back(ip);
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
    return (uint32_t((v.x() / Configuration::vertex_inbound_distance)) ^\
            (uint32_t((v.y() / Configuration::vertex_inbound_distance)) << 10) ^\
            (uint32_t((v.z() / Configuration::vertex_inbound_distance)) << 20));
}

int Mesh::getVertexIdx(QVector3D v){
    int vertex_idx = -1;
    uint32_t vertex_hash = vertexHash(v);

    QList<MeshVertex> hashed_points = vertices_hash.values(vertex_hash);
    for(unsigned int idx = 0; idx < hashed_points.size(); idx++)
    {

        if (vertexDistance(vertices[hashed_points.at(idx).idx].position, v)<=Configuration::vertex_inbound_distance*Configuration::vertex_inbound_distance)
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
    float face_z_min=1000;//cfg->max_buildsize_x;
    for (int i=0; i<3; i++){
        float temp_z_min = idx2MV(mf.mesh_vertex[i]).position.z();
        if (temp_z_min<face_z_min)
            face_z_min = temp_z_min;
    }
    return face_z_min;
}

float Mesh::getFaceZmax(MeshFace mf){
    float face_z_max=-1000;//-cfg->max_buildsize_x;
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

float sortIdx(){

}
