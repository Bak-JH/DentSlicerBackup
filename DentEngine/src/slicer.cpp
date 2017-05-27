#include "slicer.h"
#include <QHash>
#include <QElapsedTimer>

using namespace ClipperLib;

Slices Slicer::slice(Mesh* mesh){

    // mesh slicing step
    vector<Paths> meshslices = meshSlice(mesh);

    // contour construction step
    for (int i=0; i< meshslices.size(); i++){
        qDebug() << "constructing contour" << i+1 << "/" << meshslices.size();
        Slice meshslice;
        meshslice.outershell = contourConstruct(meshslices[i]);
        meshslice.z = cfg->layer_height*i;
        meshslice.outerShellOffset(-(cfg->wall_thickness+cfg->nozzle_width)/2, jtRound);
        slices.push_back(meshslice);
    }

    // overhang detection step
    overhangDetect();

    // below steps need to be done in parallel way
    // infill generation step
    Infill infill(cfg->infill_type);
    infill.generate(slices);

    // support generation step
    Support support(cfg->support_type);
    support.generate(slices);

    // raft generation step
    Raft raft(cfg->raft_type);
    raft.generate(slices);

    printf("done slicing\n");
    return slices;
}

/****************** Mesh Slicing Step *******************/

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

/****************** Contour Construction Step *******************/

// construct closed contour using segments created from meshSlice step
Paths Slicer::contourConstruct(Paths pathList){
    Paths contourList;

    QHash<int64_t, Path> pathHash;
    if (pathList.size() == 0)
        return contourList;

    int debug_count=0;

    for (int i=0; i<pathList.size(); i++){
        Path p = pathList[i];
        //insertPathHash(pathHash, p[0], p[1]); // inserts opposite too

        int64_t path_hash_u = intPoint2Hash(p[0]);
        int64_t path_hash_v = intPoint2Hash(p[1]);

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
            dest->erase(dest->begin()+2);
            if (dest->size() == 1)
                pathHash.remove(intPoint2Hash(pj_prev));
        }
        while(pj_next != last){
            contour.push_back(pj);
            dest = &(pathHash[intPoint2Hash(pj)]);

            if (dest->size() == 0|| dest->size() == 1){
                pathHash.remove(intPoint2Hash(pj));
                break;
            } else if (dest->size() == 2){
                start = (*dest)[0]; // itself
                pathHash.remove(intPoint2Hash(pj));
                pathHash.remove(intPoint2Hash((*dest)[1])); // maybe needless
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

        int64_t last_hash = intPoint2Hash(last);
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

        if (Orientation(contour)){
            ReversePath(contour);
        }

        contourList.push_back(contour);
    }


    return contourList;
}

/****************** Overhang Detection Step *******************/

// detects overhang regions in all layers
void Slicer::overhangDetect(){
    Clipper clpr;
    clpr.ZFillFunction(&zfillone);

    Paths prev_sum;
    // calculate overhang regions
    for (int idx=slices.size()-1; idx>=0; idx--){
        qDebug() << "overhang region detect" << idx << "/" << slices.size();
        Slice& slice = slices[idx];

        // region subtraction
        clpr.Clear();
        clpr.AddPaths(prev_sum, ptSubject, true);
        clpr.AddPaths(slice.outershell, ptClip, true);
        clpr.Execute(ctDifference, slice.overhang_region, pftNonZero, pftNonZero);

        // update prev_sum
        clpr.Clear();
        clpr.AddPaths(slice.outershell, ptSubject, true);
        clpr.AddPaths(slice.overhang_region, ptClip, true);
        clpr.Execute(ctUnion, prev_sum, pftNonZero, pftNonZero);

        // calculate critical overhang region
        getCriticalOverhangRegion(slice);

        // no critical overhang region so continue
        if (slice.critical_overhang_region.size() == 0){
            continue;
        }

        // poll 1/n position
        for (int cop_idx=0; cop_idx<slice.critical_overhang_region.size(); cop_idx ++){
            Path cop = slice.critical_overhang_region[cop_idx];
            float avg_x=0, avg_y=0;
            for (int int_idx=0; int_idx<cop.size(); int_idx ++){
                avg_x += cop[int_idx].X;
                avg_y += cop[int_idx].Y;
            }
            avg_x /= cop.size()*Configuration::resolution;
            avg_y /= cop.size()*Configuration::resolution;

            slices.overhang_positions.push_back(QVector3D(avg_x, avg_y, slice.z));
        }
    }

    printf("overhang region detection done\n");
}

/****************** Helper Functions For Offsetting Step *******************/

void Slice::outerShellOffset(float delta, JoinType join_type){
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
    int idx_max = ceil((z_max-z_min)/delta);
    for (int idx=0; idx<=idx_max; idx++){
        float plane_z = z_min+delta*idx;
        //float plane_z = (idx == idx_max) ? z_min+delta*(idx-1)+delta/2:z_min + delta*idx;
        planes.push_back(plane_z);
    }
    return planes;
}

vector<float> Slicer::buildAdaptivePlanes(float z_min, float z_max){
    vector<float> planes;
    return planes;
}


/****************** Helper Functions For Contour Construction Step *******************/

void Slicer::insertPathHash(QHash<int64_t, Path>& pathHash, IntPoint u, IntPoint v){
    QVector3D u_qv3 = QVector3D(u.X, u.Y, 0);
    QVector3D v_qv3 = QVector3D(v.X, v.Y, 0);

    int64_t path_hash_u = vertexHash(u_qv3);
    int64_t path_hash_v = vertexHash(v_qv3);

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

int64_t Slicer::intPoint2Hash(IntPoint u){
    QVector3D u_qv3 = QVector3D(u.X, u.Y, 0);
    int64_t path_hash_u = vertexHash(u_qv3);
    return path_hash_u;
}

/****************** Helper Functions For Contour Management *******************/

// finds critical overhang region which exceeds threshold diagonal length
void Slicer::getCriticalOverhangRegion(Slice& slice){


    for (int contour_idx=0; contour_idx< slice.overhang_region.size(); contour_idx++){
        Path contour = slice.overhang_region[contour_idx];

        Path right_contour;
        Path left_contour;

        int z0count = 0;
        int max_z0count =0;

        int head0count = 0;
        int tail0count = 0;

        bool reverse = true;

        int ip_idx = 0;
        int start_ip_idx = 0;
        int end_ip_idx = contour.size()-1;
        int temp_start_ip_idx = 0;

        while(ip_idx < contour.size()){
            IntPoint ip = contour[ip_idx];
            if (ip.Z != 0)
                break;
            head0count ++;
            ip_idx ++;
        }

        ip_idx = contour.size();
        while(ip_idx >0 ){
            ip_idx --;
            IntPoint ip = contour[ip_idx];
            if (ip.Z != 0)
                break;
            tail0count ++;
        }

        start_ip_idx = head0count;
        end_ip_idx = contour.size()-tail0count-1;

        if (end_ip_idx<0)
            return;

        max_z0count = head0count + tail0count;

        temp_start_ip_idx = head0count;
        for (ip_idx=head0count; ip_idx<contour.size()-tail0count; ip_idx++){
            IntPoint ip = contour[ip_idx];

            if (ip.Z == 1){
                if (z0count > max_z0count){
                    max_z0count = z0count;
                    end_ip_idx = ip_idx;
                    start_ip_idx = temp_start_ip_idx;
                    reverse = false;
                }
                z0count =0;
                temp_start_ip_idx = ip_idx;
            } else {
                z0count ++;
            }
        }

        if (reverse){
            right_contour.insert(right_contour.end(), contour.begin()+end_ip_idx, contour.end());
            left_contour.insert(left_contour.end(), contour.begin()+start_ip_idx, contour.begin()+end_ip_idx);
            right_contour.insert(right_contour.end(), contour.begin(), contour.begin()+start_ip_idx);
        } else {
            left_contour.insert(left_contour.end(), contour.begin()+end_ip_idx, contour.end());
            right_contour.insert(right_contour.end(), contour.begin()+start_ip_idx, contour.begin()+end_ip_idx);
            left_contour.insert(left_contour.end(), contour.begin(), contour.begin()+start_ip_idx);
        }

        if (right_contour.size() <= 1 || left_contour.size() <= 1) // needs to be fixed for triangle 110
            return;

        // left right contour split done

        IntPoint prev_ip = right_contour[0];
        Path critical_right_path;
        Path critical_left_path;


//        qDebug() << "inner contour : " << right_contour.size(); // critical overhang region detection is too long

        QElapsedTimer timer;
        timer.start();
        int support_dense = int(15/cfg->support_density);

        if (right_contour.size()/support_dense ==0){ // for small critical regions (including rectangles)
            IntPoint ip = right_contour[1];

            Path perpendicular;
            perpendicular.push_back(ip);
            perpendicular.push_back(ip+IntPoint(ip.Y-prev_ip.Y, prev_ip.X-ip.X));

            IntPoint left_hit;
            if (checkPerpendicularLength(perpendicular, left_contour, left_hit))
            {
                // need to be sorted
                critical_right_path.push_back(ip);
                critical_left_path.push_back(left_hit);
            } else {
                if (critical_right_path.size() != 0){
                    // merge critical_path
                    critical_right_path.insert(critical_right_path.end(), critical_left_path.begin(), critical_left_path.end());

                    slice.critical_overhang_region.push_back(critical_right_path);
                    critical_right_path.clear();
                    critical_left_path.clear();
                }
            }
        }

        // clean buffer paths for next usage
        critical_right_path.clear();
        critical_left_path.clear();

        //int upper_ip_idx = (right_contour.size()/support_dense==0) ? 1:right_contour.size()/support_dense;
        for (int ip_idx=0; ip_idx<=right_contour.size()/support_dense; ip_idx++){
            IntPoint ip = right_contour[ip_idx*support_dense];
            if (ip_idx == 0)
                continue;

            Path perpendicular;
            perpendicular.push_back(ip);
            perpendicular.push_back(ip+IntPoint(ip.Y-prev_ip.Y, prev_ip.X-ip.X));
//            qDebug() << "timer 4-1" << timer.elapsed() << "milliseconds";
            IntPoint left_hit;
            if (checkPerpendicularLength(perpendicular, left_contour, left_hit))
            {
                // need to be sorted
                critical_right_path.push_back(ip);
                critical_left_path.push_back(left_hit);
            } else {
                if (critical_right_path.size() != 0){
                    // merge critical_path
                    critical_right_path.insert(critical_right_path.end(), critical_left_path.begin(), critical_left_path.end());

                    slice.critical_overhang_region.push_back(critical_right_path);
                    critical_right_path.clear();
                    critical_left_path.clear();
                }
            }

            prev_ip = ip;

        }
    }
}

IntPoint lineIntersection(Path& A, Path& B) {
    // Store the values for fast access and easy
    // equations-to-code conversion
    float x1 = A[0].X, x2 = A[1].X, x3 = B[0].X, x4 = B[1].X;
    float y1 = A[0].Y, y2 = A[1].Y, y3 = B[0].Y, y4 = B[1].Y;

    float d = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
    // If d is zero, there is no intersection
    if (d == 0) return NULL;

    // Get the x and y
    float pre = (x1*y2 - y1*x2), post = (x3*y4 - y3*x4);
    float x = ( pre * (x3 - x4) - (x1 - x2) * post ) / d;
    float y = ( pre * (y3 - y4) - (y1 - y2) * post ) / d;

    // Check if the x and y coordinates are within both lines
    if ( x < min(x1, x2) || x > max(x1, x2) ||
    x < min(x3, x4) || x > max(x3, x4) ) return NULL;
    if ( y < min(y1, y2) || y > max(y1, y2) ||
    y < min(y3, y4) || y > max(y3, y4) ) return NULL;

    // Return the point of intersection
    IntPoint ret;
    ret.X = x;
    ret.Y = y;
    return ret;
}

float pointDistance(IntPoint A, IntPoint B){
    return sqrt(pow(A.X-B.X, 2)+pow(A.Y-B.Y,2));
}

bool Slicer::checkPerpendicularLength(Path A, Path B, IntPoint& left_hit){
    IntPoint prev_ip = B[0];

    for (int idx=0; idx<B.size(); idx++){
        IntPoint ip = B[idx];
        if (idx == 0)
            continue;

        Path B_part;
        B_part.push_back(prev_ip);
        B_part.push_back(ip);
        IntPoint li = lineIntersection(A,B_part);
        if (li != NULL){
            if (pointDistance(ip, li)/Configuration::resolution > cfg->overhang_threshold){
                left_hit = li;
                return true;
            }
        }
        prev_ip = ip;
    }
    return false;
}

/****************** ZFill method on intersection point *******************/

void zfillone(IntPoint& e1bot, IntPoint& e1top, IntPoint& e2bot, IntPoint& e2top, IntPoint& pt){
    pt.Z =1;
}



/****************** Deprecated functions *******************/

void Slicer::containmentTreeConstruct(){
    Clipper clpr;

    for (int idx=0; idx<slices.size(); idx++){ // divide into parallel threads
        Slice* slice = &(slices[idx]);
        clpr.Clear();
        clpr.AddPaths(slice->outershell, ptSubject, true);
        clpr.Execute(ctUnion, slice->polytree);
    }

    printf ("containment tree construction done\n");
}


void Slicer::overhangPositionPoll(){
    // collect critical overhang region and select critical overhang position
    for (int idx=slices.size()-1; idx>=0; idx--){
        qDebug() << "overhang position poll " << idx+1 << "/" << slices.size();
        Slice slice = slices[idx];
        getCriticalOverhangRegion(slice);

        // no critical overhang region so continue
        if (slice.critical_overhang_region.size() == 0){
            continue;
        }

        // poll 1/n position
        for (int cop_idx=0; cop_idx<slice.critical_overhang_region.size(); cop_idx ++){
            Path cop = slice.critical_overhang_region[cop_idx];
            float avg_x=0, avg_y=0;
            for (int int_idx=0; int_idx<cop.size(); int_idx ++){
                avg_x += cop[int_idx].X;
                avg_y += cop[int_idx].Y;
            }
            avg_x /= cop.size()*Configuration::resolution;
            avg_y /= cop.size()*Configuration::resolution;

            slices.overhang_positions.push_back(QVector3D(avg_x, avg_y, slice.z));
        }
    }
}

