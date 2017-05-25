#include "slicer.h"
#include <QHash>

using namespace ClipperLib;

Slices Slicer::slice(Mesh* mesh){

    // mesh slicing step
    vector<Paths> meshslices = meshSlice(mesh);
    Slices slices;

    // contour construction step
    for (int i=0; i< meshslices.size(); i++){
        Slice meshslice;
        meshslice.outershell = contourConstruct(meshslices[i]);
        meshslice.z = cfg->layer_height*i;
        meshslice.outerShellOffset(-(cfg->wall_thickness+cfg->nozzle_width)/2, jtRound);
        slices.push_back(meshslice);
    }

    // containment relationship tree construction step, needs to be done in parallel way
    containmentTreeConstruct(&slices);

    // overhang detection step
    overhangDetect(&slices);

    // below steps need to be done in parallel way
    // infill generation step
    Infill infill(cfg->infill_type);
    infill.generate(&slices);

    // support generation step
    Support support(cfg->support_type);
    support.generate(&slices);

    // raft generation step
    Raft raft(cfg->raft_type);
    raft.generate(&slices);

    printf("done slicing\n");
    return slices;
}

// slices mesh into segments
vector<Paths> Slicer::meshSlice(Mesh* mesh){
    float delta = cfg->layer_height;
    vector<float> planes;

    if (! strcmp(cfg->slicing_mode, "uniform")){
        planes = buildUniformPlanes(mesh->z_min, mesh->z_max, delta);
    } else if (cfg->slicing_mode == "adaptive") {
        // adaptive slice
        planes = buildAdaptivePlanes(mesh->z_min, mesh->z_max);
    }

    vector<vector<int>> triangleLists = buildTriangleLists(mesh, planes, delta);
    vector<Paths> pathLists;

    vector<int> A;
    for (int i=0; i<planes.size(); i++){
        qDebug() << "slicing layer " << i << "/" << planes.size();
        A.insert(A.end(), triangleLists[i].begin(), triangleLists[i].end()); // union
        Paths paths;
        for (int t_idx=0; t_idx<A.size(); t_idx++){
            MeshFace cur_mf = mesh->idx2MF(A[t_idx]);
            if (mesh->getFaceZmax(cur_mf) < planes[i]){
                A.erase(A.begin()+t_idx);
                t_idx --;
            }
            else{
                // compute intersection
                Path intersection = mesh->intersectionPath(cur_mf, planes[i]);
                if (intersection.size()>0){
                    paths.push_back(intersection);
                }
                else
                    continue;
            }
        }
        pathLists.push_back(paths);
    }

    return pathLists;
}

// construct closed contour using segments created from meshSlice step
Paths Slicer::contourConstruct(Paths pathList){
    Paths contourList;

    QHash<int64_t, Path> pathHash;
    if (pathList.size() == 0)
        return contourList;

    for (int i=0; i<pathList.size(); i++){
        Path p = pathList[i];
        //insertPathHash(&pathHash, p[0], p[1]); // inserts opposite too

        QVector3D u_qv3 = QVector3D(p[0].X, p[0].Y, 0);
        QVector3D v_qv3 = QVector3D(p[1].X, p[1].Y, 0);

        int64_t path_hash_u = vertexHash(u_qv3);
        int64_t path_hash_v = vertexHash(v_qv3);

        if (! pathHash.contains(path_hash_u)){
            pathHash[path_hash_u].push_back(p[0]);
        }
        if (! pathHash.contains(path_hash_v)){
            pathHash[path_hash_v].push_back(p[1]);
        }

        pathHash[path_hash_u].push_back(p[1]);
        pathHash[path_hash_v].push_back(p[0]);

    }

/* // for debug
    for (int i=0; i<pathList.size(); i++){
        Path p = pathList[i];
        //insertPathHash(&pathHash, p[0], p[1]); // inserts opposite too

        QVector3D u_qv3 = QVector3D(p[0].X, p[0].Y, 0);
        QVector3D v_qv3 = QVector3D(p[1].X, p[1].Y, 0);

        int64_t path_hash_u = vertexHash(u_qv3);
        int64_t path_hash_v = vertexHash(v_qv3);

        qDebug() << pathHash[path_hash_u].size() << pathHash[path_hash_v].size();
    }
    qDebug() << pathHash.size();
*/

    // Build Polygons
    while(pathHash.size() >0){
        Path contour;
        IntPoint start, pj_prev, pj, pj_next, last;

        pj_prev = pathHash.begin().value()[0];
        start = pj_prev;
        contour.push_back(pj_prev);
        vector<IntPoint>* dest = &(pathHash.begin().value());
        if (dest->size()<3){
            /*qDebug() << "Not intact contour";
            return contourList;*/
            pathHash.remove(intPoint2Hash(pj_prev));
            continue;
        }

        pj = (*dest)[1];
        last = (*dest)[2];
        //qDebug() << "Starting position" << pj_prev.X << pj_prev.Y << pj.X << pj.Y << last.X << last.Y;

        dest->erase(dest->begin()+1);
        dest->erase(dest->begin()+2);
        if (dest->size() == 1)
            pathHash.remove(intPoint2Hash(pj_prev));

        while(pj_next != last){
            dest = &(pathHash[intPoint2Hash(pj)]);

            if (dest->size()<3){
                /*qDebug() << "Not intact contour" << dest->size() << pj_next.X << pj_next.Y << last.X << last.Y;
                if (pathHash.size() > 0)
                    qDebug() << "not finished";
                return contourList;*/
                pathHash.remove(intPoint2Hash(pj));
                break;
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

            contour.push_back(pj);

            pj_prev = pj;
            pj = pj_next;
        }

        contour.push_back(last);

        int64_t last_hash = intPoint2Hash(last);
        if (pathHash.contains(last_hash)){
            dest = &(pathHash[last_hash]);
            //qDebug() << "last edge count :" << dest->size();

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

        contourList.push_back(contour);
        //qDebug() << "contour added";
    }


    // remove 2~ 3 vertices contours
    Slice::iterator it;
    for (it=contourList.begin(); it!=contourList.end();){
        if (it->size() <=3){
            contourList.erase(it);
        } else
            ++it;
    }
    return contourList;
}

void Slicer::containmentTreeConstruct(Slices* contoursList){
    Clipper clpr;

    for (Slice slice : *contoursList){ // divide into parallel threads
        clpr.Clear();
        clpr.AddPaths(slice.outershell, ptSubject, true);
        clpr.Execute(ctUnion, slice.polytree);
    }

    printf ("containment tree construction done\n");
}

// detects overhang regions in all layers
void Slicer::overhangDetect(Slices* contoursList){
    Clipper clpr;

    Slice prev_slice;

    // calculate overhang regions
    for (Slice slice : *contoursList){
        // do something with polytree of slice
        // region subtraction
        prev_slice = slice;
    }

    printf ("overhang detection done\n");
}

void Slice::offsetShellOffset(float delta, JoinType join_type){
    ClipperOffset co;

    co.AddPaths(outershell, join_type, etClosedPolygon);
    co.Execute(outershell, delta);
    return;
}


/****************** Helper Functions For Mesh Slicing Step *******************/

// builds vector of vector of triangle idxs sorted by z_min
vector<vector<int>> Slicer::buildTriangleLists(Mesh* mesh, vector<float> planes, float delta){

    // Create List of list
    vector<vector<int>> list_list_triangle;
    for (int l_idx=0; l_idx < planes.size()+1; l_idx ++){
        vector<int>* triangles_per_layer = new vector<int>;
        list_list_triangle.push_back(* triangles_per_layer);
    }

    // Uniform Slicing O(n)
    if (delta>0){
        for (int f_idx=0; f_idx < mesh->faces.size(); f_idx++){
            int llt_idx;
            MeshFace mf = mesh->idx2MF(f_idx);
            float z_min = mesh->getFaceZmin(mf);
            if (z_min < planes[0])
                llt_idx = 0;
            else if (z_min > planes[planes.size()-1])
                llt_idx = planes.size()-1;
            else
                llt_idx = (int)((z_min - planes[0])/delta) +1;

            list_list_triangle[llt_idx].push_back(f_idx);
        }
    }

    // General Case
    else {

    }

    return list_list_triangle;
}

vector<float> Slicer::buildUniformPlanes(float z_min, float z_max, float delta){
    vector<float> planes;
    for (int idx=0; idx<(z_max-z_min)/delta; idx++){
        planes.push_back(z_min + delta*idx);
    }
    return planes;
}

vector<float> Slicer::buildAdaptivePlanes(float z_min, float z_max){
    vector<float> planes;
    return planes;
}


/****************** Helper Functions For Contour Construction Step *******************/

void Slicer::insertPathHash(QHash<int64_t, Path>* pathHash, IntPoint u, IntPoint v){
    QVector3D u_qv3 = QVector3D(u.X, u.Y, 0);
    QVector3D v_qv3 = QVector3D(v.X, v.Y, 0);

    int64_t path_hash_u = vertexHash(u_qv3);
    int64_t path_hash_v = vertexHash(v_qv3);

    if (! pathHash->contains(path_hash_u)){
        Path temp_path;
        temp_path.push_back(u); // first element denotes key itself
        //pathHash->value(path_hash_u) = temp_path;
        pathHash->insert(path_hash_u, temp_path);
    }
    if (! pathHash->contains(path_hash_v)){
        Path temp_path;
        temp_path.push_back(v); // first element denotes key itself
        //pathHash->value(path_hash_v) = temp_path;
        pathHash->insert(path_hash_u, temp_path);
    }
    Path pathu = pathHash->value(path_hash_u);
    Path pathv = pathHash->value(path_hash_v);
    pathu.push_back(v);
    pathv.push_back(u);
    qDebug() << pathu.size() << pathHash->value(path_hash_u).size();
}

int64_t Slicer::intPoint2Hash(IntPoint u){
    QVector3D u_qv3 = QVector3D(u.X, u.Y, 0);
    int64_t path_hash_u = vertexHash(u_qv3);
    return path_hash_u;
}
