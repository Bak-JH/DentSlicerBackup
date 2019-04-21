#include "slicer.h"
#include <QHash>
#include <QElapsedTimer>
#include <QTextStream>
#include "qmlmanager.h"

using namespace ClipperLib;

Slices Slicer::slice(Mesh* mesh){
    Slices slices;
    slices.mesh = mesh;

    if (mesh == nullptr || mesh->getFaces().size() ==0){
        return slices;
    }

    // mesh slicing step
    std::vector<Paths> meshslices = meshSlice(mesh);


    printf("meshslice done\n");
    fflush(stdout);

    // contour construction step per layer
    for (int i=0; i< meshslices.size(); i++){
        //qDebug() << "constructing contour" << i+1 << "/" << meshslices.size() << "offset" << -(scfg->wall_thickness+scfg->nozzle_width)/2;
        Slice meshslice;
        meshslice.outershell = contourConstruct(meshslices[i]);
        int prev_size = meshslice.outershell.size();
        meshslice.z = scfg->layer_height*i;

        // flaw exists if contour overlaps
        //meshslice.outerShellOffset(-(scfg->wall_thickness+scfg->nozzle_width)/2, jtRound);
        slices.push_back(meshslice);
        qDebug() << i << "th meshslice.outershell.size() = " << meshslice.outershell.size();

        for (int j=0; j<meshslice.outershell.size(); j++){
            qDebug() << meshslice.outershell[j].size();
        }
    }
    //printf("meshslice done\n");

    //QTextStream(stdout) << "meshslice done" <<endl;
    //cout << "meshslice done" << endl;
    //qCritical() << "meshslice done";

    // overhang detection step
    //overhangDetect(slices);
    //cout << "overhangdetect done" <<endl;


    //containmentTreeConstruct();

    // below steps need to be done in parallel way
    // infill generation step
    //Infill infill(scfg->infill_type);
    //infill.generate(slices);
    //cout << "infill done" <<endl;

    // support generation step
    /*Support support(scfg->support_type);
    support.generate(slices);
    printf("support done\n");
    fflush(stdout);*/
    //cout << "support done" <<endl;

    // raft generation step
    /*Raft raft(scfg->raft_type);
    raft.generate(slices);
    printf("raft done\n");
    fflush(stdout);*/
    //cout << "raft done" <<endl;

    slices.containmentTreeConstruct();
    return slices;
}

/****************** Mesh Slicing Step *******************/

// slices mesh into segments
std::vector<Paths> Slicer::meshSlice(Mesh* mesh){
    float delta = scfg->layer_height;

    std::vector<float> planes;

    if (! strcmp(scfg->slicing_mode, "uniform")) {
        planes = buildUniformPlanes(mesh->z_min(), mesh->z_max(), delta);
    } else if (scfg->slicing_mode == "adaptive") {
        // adaptive slice
        planes = buildAdaptivePlanes(mesh->z_min(), mesh->z_max());
    }

    // build triangle list per layer height
    std::vector<std::vector<int>> triangleLists = buildTriangleLists(mesh, planes, delta);
    std::vector<Paths> pathLists;

    std::vector<int> A;
    A.reserve(mesh->getFaces().size());

    for (int i=0; i<planes.size(); i++){
        A.insert(A.end(), triangleLists[i].begin(), triangleLists[i].end()); // union
        Paths paths;

        for (int t_idx=0; t_idx<A.size(); t_idx++){
            MeshFace cur_mf = mesh->idx2MF(A[t_idx]);
            if (mesh->getFaceZmax(cur_mf) < planes[i]){
                A.erase(A.begin()+t_idx);
                t_idx --;
            }
            else{
                // compute intersection including on same line 2 points or 1 point
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

/****************** Contour Construction Step *******************/




/****************** Helper Functions For Offsetting Step *******************/

void Slice::outerShellOffset(float delta, JoinType join_type){
    ClipperOffset co;

    /*
    for (int path_idx=0; path_idx < outershell.size(); path_idx ++){
        Path p = outershell[path_idx];
        Path temp_p;
        co.AddPath(p, join_type, etClosedPolygon);
        co.Execute(temp_p, delta);
    }*/

    /*Paths temp_outershell;

    co.AddPaths(outershell, join_type, etClosedPolygon);
    co.Execute(temp_outershell, delta);

    outershell = temp_outershell;*/

    co.AddPaths(outershell, join_type, etClosedPolygon);
    co.Execute(outershell, delta);
    return;
}


/****************** Helper Functions For Mesh Slicing Step *******************/

// builds std::vector of std::vector of triangle idxs sorted by z_min
std::vector<std::vector<int>> Slicer::buildTriangleLists(Mesh* mesh, std::vector<float> planes, float delta){

    // Create List of list
    std::vector<std::vector<int>> list_list_triangle;
    for (int l_idx=0; l_idx < planes.size()+1; l_idx ++){
        std::vector<int>* triangles_per_layer = new std::vector<int>;
        list_list_triangle.push_back(* triangles_per_layer);
    }

    // Uniform Slicing O(n)
    if (delta>0){
        for (int f_idx=0; f_idx < mesh->getFaces().size(); f_idx++){
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


std::vector<float> Slicer::buildUniformPlanes(float z_min, float z_max, float delta){
    std::vector<float> planes;
    int idx_max = ceil((z_max-z_min)/delta);
    for (int idx=0; idx<=idx_max; idx++){
        float plane_z = round(z_min+delta*idx,2);
        qDebug() << "build Uniform Planes at height z "<< plane_z;
        //float plane_z = (idx == idx_max) ? z_min+delta*(idx-1)+delta/2:z_min + delta*idx;
        planes.push_back(plane_z);
    }
    return planes;
}

std::vector<float> Slicer::buildAdaptivePlanes(float z_min, float z_max){
    std::vector<float> planes;
    return planes;
}


/****************** Helper Functions For Contour Construction Step *******************/

void Slicer::insertPathHash(QHash<uint32_t, Path>& pathHash, IntPoint u, IntPoint v){
    QVector3D u_qv3 = QVector3D(u.X, u.Y, 0);
    QVector3D v_qv3 = QVector3D(v.X, v.Y, 0);

    uint32_t path_hash_u = vertexHash(u_qv3);
    uint32_t path_hash_v = vertexHash(v_qv3);

    if (! pathHash.contains(path_hash_u)){
        //pathHash[path_hash_u].push_back(u);
        Path* temp_path = new Path;
        temp_path->push_back(u); // first element denotes key itself
        pathHash.insert(path_hash_u, *temp_path);
    }
    if (! pathHash.contains(path_hash_v)){
        //pathHash[path_hash_u].push_back(v);
        Path* temp_path = new Path;
        temp_path->push_back(v); // first element denotes key itself
        pathHash.insert(path_hash_u, *temp_path);
    }
    pathHash[path_hash_u].push_back(u);
    pathHash[path_hash_v].push_back(v);

    return;
}


/****************** ZFill method on intersection point *******************/

void zfillone(IntPoint& e1bot, IntPoint& e1top, IntPoint& e2bot, IntPoint& e2top, IntPoint& pt){
    //qDebug() << "zfillone" << pt.X << pt.Y;
//    e1bot.Z = 1;
//    e2bot.Z = 1;
//    e1top.Z = 1;
//    e2top.Z = 1;
    pt.Z = 1;
}


/****************** Deprecated functions *******************/

void Slices::containmentTreeConstruct(){
    Clipper clpr;

    for (int idx=0; idx<this->size(); idx++){ // divide into parallel threads
        Slice* slice = &((*this)[idx]);
        clpr.Clear();
        clpr.AddPaths(slice->outershell, ptSubject, true);
        clpr.Execute(ctUnion, slice->polytree);

        for (PolyNode* pn : slice->polytree.Childs){
            qDebug() << pn->IsHole() << pn->Parent << pn->Parent->Parent;
        }
    }

    // std::vector<std::vector<IntPoint>> to std::vector<std::vector<Point>>

    /*for (int idx=0; idx<slices.size(); idx++){
        Slice* slice = &(slices[idx]);

        std::vector<std::vector<c2t::Point>> pointPaths;
        pointPaths.reserve(slice->outershell.size());

        for (Path p : slice->outershell){
            std::vector<c2t::Point> pointPath;
            pointPath.reserve(p.size());
            for (IntPoint ip : p){
                pointPath.push_back(c2t::Point((float)ip.X/scfg->resolution,(float)ip.Y/scfg->resolution));
            }
            pointPaths.push_back(pointPath);
        }

        clip2tri ct;

        ct.mergePolysToPolyTree(pointPaths, slice->polytree);
        qDebug() << "in slice " << idx << ", constructing polytree with " << slice->outershell.size() << " to " << pointPaths.size() << " point pathes" << " total tree : " << slice->polytree.Total();
    }*/
}


