#include "meshrepair.h"

void repairMesh(Mesh* mesh){

    qDebug() << "mesh repair start from mesh size :" << mesh->faces.size();
    // remove Unconnected
    removeUnconnected(mesh);
    qDebug() << "removed unconnected";

    // remove degenerate
    removeDegenerate(mesh);
    qDebug() << "removed degenerate";

    // find holes
    identifyHoles(mesh);

    // fill holes
    fillHoles(mesh);
    qDebug() << "filled holes";

    // fix normal orientations
    fixNormalOrientations(mesh);
    qDebug() << "fixed normal orientations";

    // remove gaps
    removeGaps(mesh);
    qDebug() << "removed gaps";
}


// no need to remove unused vertices since it iterates faces
// removes totally unconnected faces
// if 2 edges are unconnected or 1 edge is unconnected, goto fillhole
void removeUnconnected(Mesh* mesh){
    int unconnected_cnt = 0;
    int face_idx = 0;

    vector<MeshFace>::iterator mf_it = mesh->faces.begin();

    while (mf_it != mesh->faces.end()){
        face_idx ++;
        if (face_idx %100 ==0)
            QCoreApplication::processEvents();
        MeshFace& mf = (*mf_it);
        int neighbor_cnt = 0;

        for (int i=0; i<3; i++){
            neighbor_cnt += mf.neighboring_faces[i].size();
        }

        if (neighbor_cnt == 0){
            qDebug() << "unconnected face";
            mf_it = mesh->removeFace(mf_it);
            unconnected_cnt ++;
        } else {
            mf_it ++;
        }
    }
    qDebug() << unconnected_cnt << "unconnected faces found, faces cnt :" << mesh->faces.size();
}

// removes zero area triangles
void removeDegenerate(Mesh* mesh){
    int degenerate_cnt = 0;
    int face_idx = 0;

    vector<MeshFace>::iterator mf_it = mesh->faces.begin();
    while (mf_it != mesh->faces.end()){
        face_idx ++;
        if (face_idx %100 ==0)
            QCoreApplication::processEvents();
        MeshFace &mf = (*mf_it);

        // one vertice && 2 vertices case
        if (mf.mesh_vertex[0] == mf.mesh_vertex[1] || mf.mesh_vertex[1] == mf.mesh_vertex[2] || mf.mesh_vertex[2] == mf.mesh_vertex[0]){
            //remove face
            mf_it = mesh->removeFace(mf_it);
            degenerate_cnt ++;
            // remove one vertice and change all faces that uses erased vertice.
        } else {
            mf_it ++;
        }
    }
    qDebug() << degenerate_cnt << "degenerate faces found, faces cnt :" << mesh->faces.size();
}

void identifyHoles(Mesh* mesh){
    int face_idx = 0;
    Paths3D hole_edges;

    for (MeshFace &mf : mesh->faces){
        face_idx ++;
        if (face_idx %100 ==0)
            QCoreApplication::processEvents();
        if (mf.neighboring_faces[0].size() == 0){ // edge 0 is unconnected
            Path3D temp_edge;
            temp_edge.push_back(mesh->idx2MV(mf.mesh_vertex[0]));
            temp_edge.push_back(mesh->idx2MV(mf.mesh_vertex[1]));
            hole_edges.push_back(temp_edge);
        }
        if (mf.neighboring_faces[1].size() == 0){ // edge 1 is unconnected
            Path3D temp_edge;
            temp_edge.push_back(mesh->idx2MV(mf.mesh_vertex[1]));
            temp_edge.push_back(mesh->idx2MV(mf.mesh_vertex[2]));
            hole_edges.push_back(temp_edge);
        }
        if (mf.neighboring_faces[2].size() == 0){ // edge 2 is unconnected
            Path3D temp_edge;
            temp_edge.push_back(mesh->idx2MV(mf.mesh_vertex[2]));
            temp_edge.push_back(mesh->idx2MV(mf.mesh_vertex[0]));
            hole_edges.push_back(temp_edge);
        }
    }

    // get closed contour from hole_edges
    mesh->holes = contourConstruct(hole_edges);
    qDebug() << "mesh hole count :" << mesh->holes.size();
    return;
}


// detects hole and remove them
void fillHoles(Mesh* mesh){
    for (Path3D hole : mesh->holes){
        if (hole.size() <= 2) { // if edge is less than 3 (no hole)
            continue;
        }
        Path3D prev_path;
        Path3D cur_path;

        for (Path3D::iterator vert_it = hole.begin()+1; vert_it != hole.end()-1; ++vert_it){
            MeshVertex prev_vert = (*(vert_it-1));
            MeshVertex cur_vert = (*vert_it);
            MeshVertex next_vert = (*(vert_it+1));

            prev_path.clear();
            prev_path.push_back(prev_vert);
            prev_path.push_back(cur_vert);
            cur_path.clear();
            cur_path.push_back(cur_vert);
            cur_path.push_back(next_vert);
            reverse(cur_path.begin(), cur_path.end());

            int div_cnt = suggestDivisionCnt(prev_path, cur_path);
            for ( ; div_cnt>=0 ; div_cnt--){
                // do something abount division
            }
        }
    }
}



int suggestDivisionCnt(Path3D e1,Path3D e2){
    QVector3D e1_diff = e1[1].position-e1[0].position;
    QVector3D e2_diff = e2[1].position-e2[0].position;

    float norm_prod = e1_diff.length() * e2_diff.length();
    float dot_prod = QVector3D::dotProduct(e1_diff, e2_diff);

    float cos = dot_prod/norm_prod;


    if (cos >= cos50){
        return 0;
    } else if (cos100<=cos<cos50){
        return 1;
    } else if (cos150<=cos<cos100){
        return 2;
    } else {
        return 3;
    }
    return 4;
}

// detects orientation defects, which induces normal vector errors and render errors
void fixNormalOrientations(Mesh* mesh){

}

// future works
void removeGaps(Mesh* mesh){

}


/*---------------- helper functions -----------------*/

uint32_t meshVertex2Hash(MeshVertex u){
    uint32_t path_hash_u = vertexHash(u.position);
    return path_hash_u;
}


// construct closed contour using segments created from identify step
Paths3D contourConstruct(Paths3D hole_edges){
    Paths3D contourList;

    QHash<uint32_t, Path3D> pathHash;
    if (hole_edges.size() == 0)
        return contourList;

    int debug_count=0;

    for (int i=0; i<hole_edges.size(); i++){
        Path3D p = hole_edges[i];
        //insertPathHash(pathHash, p[0], p[1]); // inserts opposite too

        uint32_t path_hash_u = meshVertex2Hash(p[0]);
        uint32_t path_hash_v = meshVertex2Hash(p[1]);

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
        Path3D contour;
        MeshVertex start, pj_prev, pj, pj_next, last;

        pj_prev = pathHash.begin().value()[0];
        start = pj_prev;
        contour.push_back(pj_prev);
        vector<MeshVertex>* dest = &(pathHash.begin().value());

        if (dest->size() == 0|| dest->size() == 1){
            pathHash.remove(meshVertex2Hash(pj_prev));
            continue;
        } else if (dest->size() ==2){
            pj = (*dest)[1];
            last = (*dest)[0]; // pj_prev itself
            pathHash.remove(meshVertex2Hash(pj_prev));
        } else {
            pj = (*dest)[1];
            last = (*dest)[2];

            dest->erase(dest->begin()+1);
            dest->erase(dest->begin()+2);
            if (dest->size() == 1)
                pathHash.remove(meshVertex2Hash(pj_prev));
        }
        while(pj_next != last){
            contour.push_back(pj);
            dest = &(pathHash[meshVertex2Hash(pj)]);

            if (dest->size() == 0|| dest->size() == 1){
                pathHash.remove(meshVertex2Hash(pj));
                break;
            } else if (dest->size() == 2){
                start = (*dest)[0]; // itself
                pathHash.remove(meshVertex2Hash(pj));
                pathHash.remove(meshVertex2Hash((*dest)[1])); // maybe needless
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
                pathHash.remove(meshVertex2Hash(pj));


            pj_prev = pj;
            pj = pj_next;
        }

        contour.push_back(last);
        contour.push_back(start);

        uint32_t last_hash = meshVertex2Hash(last);
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
