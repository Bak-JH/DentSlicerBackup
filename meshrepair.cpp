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

    vector<MeshFace>::iterator mf_it = mesh->faces.begin();

    while (mf_it != mesh->faces.end()){
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

    vector<MeshFace>::iterator mf_it = mesh->faces.begin();
    while (mf_it != mesh->faces.end()){
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
    Paths3D hole_edges;

    for (MeshFace &mf : mesh->faces){
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
    qDebug() << "hole edges count :" << hole_edges.size();
    mesh->holes = hole_edges;

    // construct hole using hole_edges;
    holeConstruct(mesh);

    // get closed contour from hole_edges
    //mesh->holes = holeConstruct(hole_edges);F
    //mesh->holes = contourConstruct(hole_edges);
    qDebug() << "mesh hole count :" << mesh->holes.size();
    return;
}


// detects hole and remove them
void fillHoles(Mesh* mesh){
    for (Path3D hole : mesh->holes){

        // refine hole boundary edges
        if (hole.size() <= 2) { // if edge is less than 3 (no hole)
            continue;
        }

        Path3D newhole;
        Path3D::iterator vert_it = hole.begin() +1;
        while (vert_it != hole.end()-1){
            MeshVertex prev_vert = (*(vert_it-1));
            MeshVertex cur_vert = (*vert_it);
            MeshVertex next_vert = (*(vert_it+1));


            QVector3D cur_vert_position = cur_vert.position;
            QVector3D next_vector = next_vert.position - cur_vert.position;
            QVector3D prev_vector = cur_vert.position - prev_vert.position;

            int div_cnt = suggestDivisionCnt(prev_vector, next_vector);

            qDebug() << div_cnt << "division occured";

            // remove cur_vert
            //vert_it = hole.erase(vert_it);

            if (div_cnt == 1){ // if less than 70 deg
                // add face with prev, cur, next vertices
                mesh->addFace(prev_vert.position, cur_vert_position, next_vert.position);

            } else { // if larger than 70 deg
                for (int cur_div=1; cur_div<=div_cnt; cur_div++){
                    QVector3D diff_vector = (cur_div*prev_vector + (div_cnt-cur_div)*next_vector)/div_cnt;
                    MeshVertex new_vertex = MeshVertex(cur_vert_position + diff_vector);
                    hole.insert(vert_it, new_vertex);

                    //hole.insert(new_vertex);
                }
            }
            qDebug() << "before erase : hole size :" << hole.size();
            vert_it = hole.erase(vert_it);
            //vert_it ++;
        }
    }
}

// floating sphere detects gaps and connects gaps
void fillGaps(){

}


int suggestDivisionCnt(QVector3D e1,QVector3D e2){

    float norm_prod = e1.length() * e2.length();
    float dot_prod = QVector3D::dotProduct(e1, e2);

    float cos = dot_prod/norm_prod;


    if (cos >= cos75){
        return 1;
    } else if (cos135<=cos<cos75){
        return 2;
    /*} else if (cos150<=cos<cos135){
        return 2;*/
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

// construct closed hole using segments
Paths3D holeConstruct(Mesh* mesh){
    Paths3D holeList;

    QMultiHash<int, MeshVertex> connectionHash;
    // insert connectionHashes
    for (Path3D edges: mesh->holes){
        int hash_u = edges[0].idx;
        int hash_v = edges[1].idx;

        if (!connectionHash.contains(hash_u)){
            connectionHash.insert(hash_u, edges[0]);
        }
        if (!connectionHash.contains(hash_v)){
            connectionHash.insert(hash_v, edges[1]);
        }

        connectionHash.insert(hash_u, edges[1]);
        connectionHash.insert(hash_u, edges[0]);
    }

    MeshVertex init_vert = mesh->idx2MV(connectionHash.begin().key());
    qDebug() << init_vert.position;

    MeshVertex prev_vert = init_vert;

    Path3D cur_path;

    while(connectionHash.size() != 0){

        cur_path.push_back(prev_vert);
        QList<MeshVertex> connected_vertices = connectionHash.values(prev_vert.idx);
        qDebug() << connected_vertices.size();
        int prev_vert_idx = prev_vert.idx;

        bool foundEnd = false;
        int found_idx = 0;

        if (connected_vertices.size() == 0){ // not closed contour, so end up with init_vert

            qDebug() << "connected vertices size 0";

            cur_path.push_back(init_vert);
            holeList.push_back(cur_path);
            cur_path.clear();

            // choose next pivot
            if (connectionHash.size() !=0){
                prev_vert = mesh->idx2MV(connectionHash.begin().key());
            }

            continue;
        }


        // search init_vert
        for(int i=0; i<connected_vertices.size(); i++){
            if (connected_vertices[i].idx == init_vert.idx){
                foundEnd = true;
                found_idx = i;
            }
        }

        prev_vert = connected_vertices[found_idx];
        if (foundEnd){
            qDebug() << "found end" << cur_path.size();
            // end cur_path and push it to holeList
            cur_path.push_back(init_vert);
            holeList.push_back(cur_path);
            cur_path.clear();

            // remove history
            connectionHash.remove(prev_vert_idx, connected_vertices[found_idx]);

            // select new pivot
            prev_vert = mesh->idx2MV(connectionHash.begin().key());

        } else {
            // select next vertex
            prev_vert = connected_vertices[0];
            qDebug() << "finding next vertex from" << prev_vert_idx << "" << connected_vertices.size();

            // remove history
            connectionHash.remove(prev_vert_idx, connected_vertices[0]);
        }


    }

    return holeList;
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
