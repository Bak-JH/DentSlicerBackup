#include "mesh.h"
#include <QDebug>


/********************** Mesh Generation Functions **********************/

void Mesh::addFace(QVector3D v0, QVector3D v1, QVector3D v2){
    int v0_idx = getVertexIdx(v0);
    int v1_idx = getVertexIdx(v1);
    int v2_idx = getVertexIdx(v2);
    //*qDebug() << "Face add (" << v0_idx << "," << v1_idx << "," << v2_idx << ")";

    MeshFace mf;
    mf.idx = faces.size();
    mf.mesh_vertex[0] = v0_idx;
    mf.mesh_vertex[1] = v1_idx;
    mf.mesh_vertex[2] = v2_idx;

    mf.fn = QVector3D::normal(vertices[v0_idx].position, vertices[v1_idx].position, vertices[v2_idx].position);

    vertices[v0_idx].connected_faces.emplace_back(mf.idx);
    vertices[v1_idx].connected_faces.emplace_back(mf.idx);
    vertices[v2_idx].connected_faces.emplace_back(mf.idx);
    if (vertices[v0_idx].connected_faces.size()>=3){
        MeshVertex &mv = vertices[v0_idx];
        mv.vn = QVector3D(idx2MF(mv.connected_faces[0]).fn + idx2MF(mv.connected_faces[1]).fn + idx2MF(mv.connected_faces[2]).fn).normalized();
    } else {
        MeshVertex &mv = vertices[v0_idx];
        mv.vn = QVector3D(0,0,0);
    }
    if (vertices[v1_idx].connected_faces.size()>=3){
        MeshVertex &mv = vertices[v1_idx];
        vertices[v1_idx].vn = QVector3D(idx2MF(mv.connected_faces[0]).fn + idx2MF(mv.connected_faces[1]).fn + idx2MF(mv.connected_faces[2]).fn).normalized();
    } else {
        MeshVertex &mv = vertices[v1_idx];
        vertices[v1_idx].vn = QVector3D(0,0,0);
    }
    if (vertices[v2_idx].connected_faces.size()>=3){
        MeshVertex &mv = vertices[v2_idx];
        mv.vn = QVector3D(idx2MF(mv.connected_faces[0]).fn + idx2MF(mv.connected_faces[1]).fn + idx2MF(mv.connected_faces[2]).fn).normalized();
    } else {
        MeshVertex &mv = vertices[v2_idx];
        mv.vn = QVector3D(0,0,0);
    }

    faces.emplace_back(mf);
}

// add connected face idx to each meshes
void Mesh::connectFaces(){
    for (int i=0; i<faces.size(); i++){
        MeshFace &mf = faces[i];
        //*qDebug() << "Connecting face : " << i << "(" << mf.mesh_vertex[0] << "," << mf.mesh_vertex[1] << "," << mf.mesh_vertex[2] << ")";
        mf.connected_face_idx[0] = findFaceWith2Vertices(mf.mesh_vertex[0], mf.mesh_vertex[1], i); // connected to vertex 0 1
        //*qDebug() << " neighbor of" << i << ":" << mf.connected_face_idx[0];
        mf.connected_face_idx[1] = findFaceWith2Vertices(mf.mesh_vertex[1], mf.mesh_vertex[2], i); // connected to vertex 0 1
        //*qDebug() << " neighbor of" << i << ":" << mf.connected_face_idx[1];
        mf.connected_face_idx[2] = findFaceWith2Vertices(mf.mesh_vertex[2], mf.mesh_vertex[0], i); // connected to vertex 0 1
        //*qDebug() << " neighbor of" << i << ":" << mf.connected_face_idx[2];
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
        //qDebug() << idx2MV(mf.mesh_vertex[i]).position.z();
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
            //qDebug() << "intersection error at layer "<< getFaceZmax(mf) << getFaceZmin(mf) << z<< upper.size() << lower.size();

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

    /*if (vertices_hash.contains(vertex_hash)){ // if vertex exists
        vertex_idx = vertices_hash.value(vertex_hash).idx; // if multiple, hash collision
    } else { // if vertex doesn't exists
        MeshVertex mv = MeshVertex(v);
        mv.idx = vertices.size();
        vertices.emplace_back(mv);
        vertices_hash.insert(vertex_hash, mv);
        vertex_idx = mv.idx;
        updateMinMax(v);
    }*/

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
int Mesh::findFaceWith2Vertices(int v0_idx, int v1_idx, int self_idx){
    std::vector<int> candidates;
    //*qDebug() << " with vrtx" << v0_idx << "," << v1_idx;
    for (int f: vertices[v0_idx].connected_faces){
        if (f == self_idx){
            //*qDebug() << " candidate" << f << "(itself)";
            continue;
        }
        if (faces[f].mesh_vertex[0] == v1_idx || faces[f].mesh_vertex[1] == v1_idx || faces[f].mesh_vertex[2] == v1_idx){
            candidates.emplace_back(f);
            //*qDebug() << " candidate" << f;
        }
    }
    if (candidates.size() == 0){
        //qDebug() << "no candidates";
        return -1;
    }
    if (candidates.size() == 1){
        //qDebug() << "found face";
        return candidates[0];
    } else {
        //qDebug() << candidates.size() << "multiple faces are connected so outputting first connected face" << candidates[0];
        return candidates[0];
    }
}

float Mesh::getFaceZmin(MeshFace mf){
    float face_z_min=cfg->max_buildsize_x;
    for (int i=0; i<3; i++){
        float temp_z_min = idx2MV(mf.mesh_vertex[i]).position.z();
        if (temp_z_min<face_z_min)
            face_z_min = temp_z_min;
    }
    return face_z_min;
}

float Mesh::getFaceZmax(MeshFace mf){
    float face_z_max=-cfg->max_buildsize_x;
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


