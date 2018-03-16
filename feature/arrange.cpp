#include "arrange.h"

Arrange::Arrange()
{

}
bool shit = false;

Paths spreadingCheck(Mesh* mesh, bool* check, int chking_start, bool is_chking_pos){
    /**/qDebug() << "SpreadingCheck started from" << chking_start;
    Paths paths;
    int chking = -1;
    vector<MeshFace*> to_check;
    to_check.push_back(& mesh->faces[chking_start]);
    while(to_check.size()>0){
        //**qDebug() << "New spreadingCheck generation (" << to_check.size() << "faces)";
        vector<MeshFace*> next_to_check;
        for(int i=0; i<to_check.size(); i++){
            chking = to_check[i]->idx;
            /*Debug
            qDebug() << "to_check" << i << "(Face" << chking << ")";
            for(int side=0; side<3; side++){
                MeshFace* neighbor = to_check[i]->neighboring_faces[side][0];
                if(isEdgeBound(to_check[i], side)) qDebug() << " - unchecked bound";
                else qDebug() << " -" << to_check[i]->neighboring_faces[side].size()
                               << "neighbors (first:" << to_check[i]->neighboring_faces[side][0]->idx << ")";
            }//*/
            if(check[chking]) continue;
            check[chking] = true;
            MeshFace* mf = to_check[i];
            int side;
            int outline_checked = false;
            for(side=0; side<3; side++){
                if(isEdgeBound(mf, side, is_chking_pos)){
                    if(!outline_checked){
                        int path_head = getPathHead(mf, side, is_chking_pos);
                        Path path = buildOutline(mesh, check, chking, path_head, is_chking_pos);
                        paths.push_back(path);
                        outline_checked = true;
                    }
                }else{//법선 방향 조건이 만족되는 이웃만 to_check에 추가하는 것이 맞을지 검토
                    vector<MeshFace*> neighbors = mf->neighboring_faces[side];
                    next_to_check.insert(next_to_check.end(), neighbors.begin(), neighbors.end());
                }
            }
        }
        to_check.clear();
        to_check.insert(to_check.end(), next_to_check.begin(), next_to_check.end());
    }
    return paths;
}

int getPathHead(MeshFace* mf, int side, bool is_chking_pos){
    if(side==0 && isEdgeBound(mf, 2, is_chking_pos)) {
        if(isEdgeBound(mf, 1, is_chking_pos)) return -1;//all side of chking face is bound, face is alone
        else return mf->mesh_vertex[2];
    }
    return mf->mesh_vertex[side];
}

Path buildOutline(Mesh* mesh, bool* check, int chking, int path_head, bool is_chking_pos){
    //**qDebug() << "buildOutline from" << chking;
    vector<int> path_by_idx;
    if(path_head==-1){//혼자있는 면의 경우 오리엔테이션 확인 방법이 마련되어있지 않음
        check[chking] = true;
        path_by_idx = arrToVect(mesh->faces[chking].mesh_vertex);
        //**qDebug() << "buildOutline done";
        return idxsToPath(mesh, path_by_idx);
    }
    bool outline_closed = false;
    int from = -1;
    int nxt_chk = -1;
    int path_tail = path_head;
    while(!outline_closed){
        //**qDebug() << "chking" << chking;
        MeshFace* mf = & mesh->faces[chking];
        int outline_edge_cnt = 0;
        int tail_idx;//The index that path_tail has in the mf->mesh_vertex
        for(int i=0; i<3; i++){
            if(mf->mesh_vertex[i]==path_tail) tail_idx = i;
            if(isEdgeBound(mf, i, is_chking_pos)) outline_edge_cnt++;
        }
        if(isEdgeBound(mf, tail_idx, is_chking_pos)){
            path_by_idx.push_back(path_tail);
            check[chking] = true;
            if(outline_edge_cnt==1){
                path_tail = getNbrVtx(mf, tail_idx, 1);
                nxt_chk = mf->neighboring_faces[(tail_idx+1)%3][0]->idx;
            }else{//outline_edge_cnt==2
                path_by_idx.push_back(getNbrVtx(mf, tail_idx, 1));
                path_tail = getNbrVtx(mf, tail_idx, 2);
                nxt_chk = mf->neighboring_faces[(tail_idx+2)%3][0]->idx;
            }
            if(path_tail == path_head) outline_closed = true;
        }else{//if not isEdgeBound(mf, tail_idx), the face doesn't share any bound edge with current outline
            //the face may share some bound edge with other outline, so we do not mark it checked
            if(mf->neighboring_faces[tail_idx][0]->idx==from) nxt_chk = mf->neighboring_faces[(tail_idx+2)%3][0]->idx;
            else nxt_chk = mf->neighboring_faces[tail_idx][0]->idx;
        }
        from = chking;
        chking = nxt_chk;
    }
    //**qDebug() << "buildOutline done";
    return idxsToPath(mesh, path_by_idx);
}

bool isEdgeBound(MeshFace* mf, int side, bool is_chking_pos){//bound edge: connected to face with opposit fn.z/not connected any face/multiple neighbor/opposit orientation
    if(mf->neighboring_faces[side].size() != 1) return true;
    MeshFace* neighbor = mf->neighboring_faces[side][0];
    if(!checkFNZ(neighbor, is_chking_pos)) return true;
    if(!isNbrOrientSame(mf, side)) return true;
    return false;
}

bool isNbrOrientSame(MeshFace* mf, int side){
    MeshFace* nbr = mf->neighboring_faces[side][0];
    if(getNbrVtx(nbr, searchVtxInFace(nbr, mf->mesh_vertex[side]), 2) == getNbrVtx(mf, side, 1)) return true;
    return false;
}

int searchVtxInFace(MeshFace* mf, int vertexIdx){
    for(int i=0; i<3; i++){
        if(mf->mesh_vertex[i] == vertexIdx) return i;
    }
    return -1;
}

vector<int> arrToVect(int arr[]){
    vector<int> vec (arr, arr + sizeof arr / sizeof arr[0]);
    return vec;
}

int getNbrVtx(MeshFace* mf, int base, int xth){//getNeighborVtx
    if(xth>0) return mf->mesh_vertex[(base+xth+3)%3];
    else return -1;
}

Path idxsToPath(Mesh* mesh, vector<int> path_by_idx){
    Path path;
    for(int id : path_by_idx){
        QVector3D vertex = mesh->vertices[id].position;
        mesh->Mesh::addPoint(vertex.x(), vertex.y(), &path);
        //qDebug() << "path build" << id << ip.X << ip.Y;
    }
    return path;
}

Paths project(Mesh* mesh){
    int faces_size = mesh->faces.size();
    vector<Paths> outline_sets;
    bool is_chking_pos = false;
    for(int i=0; i<2; i++){
        is_chking_pos = !is_chking_pos;
        bool check_done = false;
        bool face_checked[faces_size] = {false}; //한 번 확인한 것은 체크리스트에서 제거되는 자료구조 도입 필요(법선 확인이 반복시행됨)
        /****/qDebug() << "Get outline(is_chking_pos:" << is_chking_pos << ")";
        while(!check_done){
            for(int face_idx=0; face_idx<faces_size; face_idx++){
                if(checkFNZ(& mesh->faces[face_idx], is_chking_pos) && !face_checked[face_idx]){
                    outline_sets.push_back(spreadingCheck(mesh, face_checked, face_idx, is_chking_pos));//위/아래를 보는 면 사이 orientation 괜찮은가
                    break;
                }else if(face_idx==faces_size-1) check_done = true;
            }
        }
    }
    /****/qDebug() << "Total outline_set : " << outline_sets.size() << "->" << clipOutlines(outline_sets).size();
    return clipOutlines(outline_sets);
}

Paths clipOutlines(vector<Paths> outline_sets){
    Paths projection;
    //Paths tmp_clip_result;
    Clipper c;
    for(uint i=0; i<(outline_sets.size()); i++){
        //*qDebug() << "Clipping" << i;
        c.Clear();
        c.AddPaths(projection, ptSubject, true);
        c.AddPaths(outline_sets[i], ptClip, true);
        //simplyfy 사용할지 검토
        c.Execute(ctUnion, projection, pftNonZero, pftNonZero);
        /*projection.clear();
        projection.insert(projection.end(), tmp_clip_result.begin(), tmp_clip_result.end());
        tmp_clip_result.clear();*/
    }
    return projection;
}

bool checkFNZ(MeshFace* face, bool is_chking_pos){
    if(is_chking_pos){
        if(face->fn.z()>=0) return true;
        else return false;
    }else{
        if(face->fn.z()<=0) return true;
        else return false;
    }
}

void debugPaths(Paths paths){
    qDebug() << "===============";
    for(int i=0; i<paths.size(); i++){
        qDebug() << "";
        //qDebug() << "path" << i;
        for(int j=0; j<paths[i].size(); j++){
            qDebug()<< paths[i][j].X << paths[i][j].Y;
        }
    }
    qDebug() << "===============";
}

void debugPath(Path path){
    qDebug() << "===============";
    for(int i=0; i<path.size(); i++){
        qDebug()<< path[i].X << path[i].Y;
    }
    qDebug() << "===============";
}

void debugFaces(Mesh* mesh, vector<int> face_list){
    for(int i=0; i<face_list.size(); i++){
        debugFace(mesh, face_list[i]);
    }
}

void debugFace(Mesh* mesh, int face_idx){
        MeshFace* mf = & mesh->faces[face_idx];
        qDebug() << "face #" << face_idx;
        for(int side=0; side<3; side++){
            QVector3D vtx = mesh->vertices[mf->mesh_vertex[side]].position;
            float x_f = vtx.x();
            float y_f = vtx.y();
            float z_f = vtx.z();
            int x_int = round(x_f*cfg->resolution);
            int y_int = round(y_f*cfg->resolution);
            int z_int = round(z_f*cfg->resolution);
            qDebug() << "(" << x_f << "," << y_f << "," << z_f << ")";
        }
        qDebug() << "face normal:" << "(" << mf->fn.x() << "," << mf->fn.y() << "," << mf->fn.z() << ")";
        for(int side=0; side<3; side++){
            if(mf->neighboring_faces[side].size()==1){
                MeshFace* neighbor = mf->neighboring_faces[side][0];
                if(neighbor->fn.z()>=0){
                    if(isNbrOrientSame(mf, side)) qDebug() << "side" << side << ": nbr" << mf->neighboring_faces[side][0]->idx;
                    else qDebug() << "side" << side << ": bound(ornt diff)";
                }else{
                    if(isNbrOrientSame(mf, side)) qDebug() << "side" << side << ": bound(fn.z diff)";
                    else qDebug() << "side" << side << ": bound(fn.z diff, ornt diff)";
                }
            }else if(mf->neighboring_faces[side].size()==0){
                qDebug() << "side" << side << ": bound(no neighbor)";
            }else{
                qDebug() << "side" << side << ": bound(multi-neighbor" << mf->neighboring_faces[side].size() << ")";
            }
        }
}

int findVertexWithIntpoint(IntPoint p, Mesh* mesh){
    return findVertexWithIntXY(p.X, p.Y, mesh);
}

int findVertexWithIntXY(int x, int y, Mesh* mesh){
    for(int vtx_idx=0; vtx_idx<mesh->vertices.size(); vtx_idx++){
        QVector3D vtx_pos = mesh->vertices[vtx_idx].position;
        int x_int = round(vtx_pos.x()*cfg->resolution);
        int y_int = round(vtx_pos.y()*cfg->resolution);
        if(x_int==x && y_int==y) return vtx_idx;
    }
}

//=========================================
//Ramer–Douglas–Peucker line simplification
//=========================================
void RDPSimpPaths(Paths* paths){
    for(int path_idx=0; path_idx<paths->size(); path_idx++){
        RDPSimp(&(*paths)[path_idx]);
        if((*paths)[path_idx].size()==2) {
            paths->erase(paths->begin()+path_idx);
            path_idx--;
        }
    }
}

void RDPSimp(Path* path){
    int path_size = path->size();
    bool keepingPointMrkArr[path_size] = {false};
    keepingPointMrkArr[0] = true;
    keepingPointMrkArr[path_size-1] = true;
    RDPSimpIter(path, 0, path_size-1, keepingPointMrkArr);
    Path simplified_path;
    for(int point_idx=0; point_idx<path_size; point_idx++){
        if(keepingPointMrkArr[point_idx]) simplified_path.push_back((*path)[point_idx]);
    }
    /**/qDebug() << path->size() << "->" << simplified_path.size();
    path->swap(simplified_path);
    simplified_path.clear();
}

void RDPSimpIter(Path* path, int start, int end, bool* keepingPointMrkArr){
    float epsilon = 100;
    float max_distance = 0;
    int max_dist_point = -1;
    for(int point_idx=start+1; point_idx<end; point_idx++){
        float distance = distL2P(&(*path)[start], &(*path)[end], &(*path)[point_idx]);
        if(max_distance<distance){
            max_distance = distance;
            max_dist_point = point_idx;
        }
    }
    if(epsilon<max_distance){
        keepingPointMrkArr[max_dist_point] = true;
        if(start+1<max_dist_point)RDPSimpIter(path, start, max_dist_point, keepingPointMrkArr);
        if(end-1>max_dist_point)RDPSimpIter(path, max_dist_point, end, keepingPointMrkArr);
    }
}

float distL2P(IntPoint* line_p1, IntPoint* line_p2, IntPoint* p){
    float triangle_area_double = abs(line_p1->X*(line_p2->Y-p->Y) + line_p2->X*(p->Y-line_p1->Y) +p->X*(line_p1->Y-line_p2->Y));
    float bottom_side_length = sqrt(pow((line_p1->X-line_p2->X),2) + pow((line_p1->Y-line_p2->Y),2));
    return triangle_area_double/bottom_side_length;
}


//=========================================
//Arrangement Algorithm
//=========================================

//typedef pair<IntPoint, float> XYArrangement;

vector<XYArrangement> arngMeshes(vector<Mesh>* meshes){
    vector<Paths> outlines;
    /**/qDebug() << "Arrange start";
    for(int idx=0; idx<meshes->size(); idx++){
        outlines.push_back(project(&(*meshes)[idx]));
        RDPSimpPaths(&outlines[idx]);
    }
    /**/qDebug() << "Projection done @arngMeshes";
    return arng2D(&outlines);
}

vector<XYArrangement> arng2D(vector<Paths>* figs){
    vector<XYArrangement> arng_result;
    Paths cum_outline;
    initStage(&cum_outline);
    for(int idx=0; idx<figs->size(); idx++){
        XYArrangement new_result = arngFig(&cum_outline, &(*figs)[idx]);
        if(new_result.second==-1) break;//-1 means arng imposible
        arng_result.push_back(new_result);
        /**/qDebug() << idx << "arngd @arng2D";
    }
    /**/qDebug() << "Arrange all done";
    /**/debugPaths(cum_outline);
}

void initStage(Paths* cum_outline){

    int stage_x = 72000;
    int stage_y = 72000;

    Path stage;
    IntPoint p1, p2, p3, p4;
    p1.X=stage_x/2;p1.Y=stage_y/2;
    p2.X=-stage_x/2;p2.Y=stage_y/2;
    p3.X=-stage_x/2;p3.Y=-stage_y/2;
    p4.X=stage_x/2;p4.Y=-stage_y/2;
    stage.push_back(p4);//cw
    stage.push_back(p3);
    stage.push_back(p2);
    stage.push_back(p1);

    Path universal_plane;//to make placeable space have zero filling value
    IntPoint u_p1, u_p2, u_p3, u_p4;
    u_p1.X=stage_x/2+1;u_p1.Y=stage_y/2+1;
    u_p2.X=-stage_x/2-1;u_p2.Y=stage_y/2+1;
    u_p3.X=-stage_x/2-1;u_p3.Y=-stage_y/2-1;
    u_p4.X=stage_x/2+1;u_p4.Y=-stage_y/2-1;
    universal_plane.push_back(u_p1);//ccw
    universal_plane.push_back(u_p2);
    universal_plane.push_back(u_p3);
    universal_plane.push_back(u_p4);

    cum_outline->push_back(universal_plane);
    cum_outline->push_back(stage);
}

XYArrangement arngFig(Paths* cum_outline, Paths* fig){
    /**/qDebug() << "Arrange new fig @arngFig";
    float angle_unit = 360;

    Paths optimal_rot_fig;
    XYArrangement global_optimal_arrangement;
    int global_min_width = abs((*cum_outline)[0][0].X);//universal plane 상 최대 X
    int global_min_height = abs((*cum_outline)[0][0].Y);//universal plane 상 최대 Y
    /**/qDebug() << "- global_min_width init" << global_min_width << "@arngFig";
    for(float rot_angle=0; rot_angle<360; rot_angle += angle_unit){
        /**/qDebug() << "- rot_angle" << rot_angle << "@arngFig";
        Paths rot_fig = rotatePaths(fig, rot_angle);
        /**/qDebug() << "- fig rotated" << "@arngFig";
        Paths nfp_set = getNFP(cum_outline, &rot_fig);
        /**/qDebug() << "- got nfp_set" << "(size:" << nfp_set.size() << ")" << "@arngFig";
        IntPoint optimal_point = getOptimalPoint(&nfp_set);
        /**/qDebug() << "- got optimal_point" << "@arngFig";
        int local_min_width = getMaxX(optimal_point, &rot_fig);
        /**/qDebug() << "- got local_min_width" << local_min_width << "@arngFig";
        int local_min_height = getMaxY(optimal_point, &rot_fig);
        if(local_min_width<global_min_width){
            /**/qDebug() << "- global_min_width" << "@arngFig";
            optimal_rot_fig = rot_fig;
            global_optimal_arrangement = {optimal_point, rot_angle};
            global_min_width = local_min_width;
            global_min_height = local_min_height;
        }else if(local_min_width==global_min_width && local_min_height<global_min_height){
            /**/qDebug() << "- global_min_height" << "@arngFig";
            optimal_rot_fig = rot_fig;
            global_optimal_arrangement = {optimal_point, rot_angle};
            global_min_height = local_min_height;
        }
    }
    if(global_min_width == abs((*cum_outline)[0][0].X)){//arng imposible
        /**/qDebug() << "- arng imposible" << "@arngFig";
        IntPoint p = {0, 0};
        return make_pair(p,-1);//-1 means arng imposible
    }
    tanslatePaths(&optimal_rot_fig, global_optimal_arrangement.first);
    /**/qDebug() << "- path translated" << "@arngFig";
    cumulativeClip(cum_outline, &optimal_rot_fig);
    /**/qDebug() << "- cumulativeClip done" << "@arngFig";
    return global_optimal_arrangement;
}

Paths rotatePaths(Paths* paths, float rot_angle){//Rotation about origin,because QTransform does so.
    Paths rot_paths;
    rot_paths.resize(paths->size());
    for(int path_idx=0; path_idx<paths->size();path_idx++){
        for(int point_idx=0; point_idx<(*paths)[path_idx].size(); point_idx++){
            rot_paths[path_idx].push_back(rotateIntPoint((*paths)[path_idx][point_idx], rot_angle));
        }
    }
    return rot_paths;
}

IntPoint rotateIntPoint(IntPoint point, float rot_angle){//Rotation about origin,because QTransform does so.
    IntPoint rot_point;
    float rad_angle = rot_angle*M_PI/180;
    rot_point.X = point.X*cosf(rad_angle) - point.Y*sinf(rad_angle);
    rot_point.Y = point.X*sinf(rad_angle) + point.Y*cosf(rad_angle);
    return rot_point;
}

void tanslatePaths(Paths* paths, IntPoint tanslate_vec){
    Paths translated_paths;
    translated_paths.resize(paths->size());
    for(int path_idx=0; path_idx<paths->size();path_idx++){
        for(int point_idx=0; point_idx<(*paths)[path_idx].size(); point_idx++){
            translated_paths[path_idx].push_back(tanslateIntPoint((*paths)[path_idx][point_idx], tanslate_vec));
        }
    }
    paths->swap(translated_paths);
    translated_paths.clear();
}

IntPoint tanslateIntPoint(IntPoint point, IntPoint tanslate_vec){
    IntPoint tanslated_point;
    tanslated_point.X = point.X + tanslate_vec.X;
    tanslated_point.Y = point.Y + tanslate_vec.Y;
    return tanslated_point;
}

void cumulativeClip(Paths* cum_outline, Paths* new_fig){
    Clipper c;
    c.AddPaths(*cum_outline, ptSubject, true);
    c.AddPaths(*new_fig, ptClip, true);
    c.Execute(ctUnion, *cum_outline, pftPositive, pftPositive);
}

IntPoint getOptimalPoint(Paths* nfp_set){
    /**/qDebug() << "-0" << "@arngFig";
    int min_x = (*nfp_set)[1][0].X;
    int min_y = (*nfp_set)[1][0].Y;
    /**/qDebug() << "-1" << "@arngFig";
    int paths_inculding_optimal_point = 1;
    /**/qDebug() << "-2" << "@arngFig";
    int optimal_point_idx = 0;
    /**/qDebug() << "- optimalPoint checked" << "0" << "@getOptimalPoint";
    for(int path_idx=1; path_idx<nfp_set->size();path_idx++){
        for(int point_idx=0; point_idx<(*nfp_set)[path_idx].size(); point_idx++){
            if((*nfp_set)[path_idx][point_idx].X<min_x){
                min_x = (*nfp_set)[path_idx][point_idx].X;
                min_y = (*nfp_set)[path_idx][point_idx].Y;
                paths_inculding_optimal_point = path_idx;
                optimal_point_idx = point_idx;
                /**/qDebug() << "- upadte optimalPoint to" << point_idx << "@getOptimalPoint";
            }else if((*nfp_set)[path_idx][point_idx].X==min_x && (*nfp_set)[path_idx][point_idx].Y<min_y){
                min_y = (*nfp_set)[path_idx][point_idx].Y;
                paths_inculding_optimal_point = path_idx;
                optimal_point_idx = point_idx;
                /**/qDebug() << "- upadte optimalPoint to" << point_idx << "@getOptimalPoint";
            }
            /**/qDebug() << "- optimalPoint checked" << point_idx << "@getOptimalPoint";
        }
    }
    return (*nfp_set)[paths_inculding_optimal_point][optimal_point_idx];
}

int getMaxX(IntPoint translate_vec, Paths* fig){
    Path path = (*fig)[0];//paths[0]이 가장 외곽일거라 가정
    int max_x = path[0].X;
    for(int idx=1; idx<path.size(); idx++){
        if(max_x<path[idx].X) max_x=path[idx].X;
    }
    return max_x + translate_vec.X - path[0].X;
}

int getMaxY(IntPoint translate_vec, Paths* fig){
    Path path = (*fig)[0];//paths[0]이 가장 외곽일거라 가정
    int max_y = path[0].Y;
    for(int idx=1; idx<path.size(); idx++){
        if(max_y<path[idx].Y) max_y=path[idx].Y;
    }
    return max_y + translate_vec.Y - path[0].Y;
}

Paths getNFP(Paths* subject, Paths* object){
    Path convex_obj = getConvexHull(&(*object)[0]);
    /**/qDebug() << "- getConvexHull result" << "@getNFP";
    /**/debugPath(convex_obj);//qDebug

    vector<vector<float>> sub_slope_set;
    /**/qDebug() << "- sub_slope_set" << "@getNFP";
    sub_slope_set.resize(subject->size());
    for(int path_idx=1; path_idx<subject->size(); path_idx++){//path_idx0 is univesal_plane
        for(int edge_idx=0; edge_idx<(*subject)[path_idx].size(); edge_idx++){
            sub_slope_set[path_idx].push_back(getNthEdgeSlope(&(*subject)[path_idx], edge_idx, true));
        }
    }

    vector<vector<IntPoint>> sub_vec_set;
    /**/qDebug() << "- sub_vec_set" << "@getNFP";
    sub_vec_set.resize(subject->size());
    for(int path_idx=0; path_idx<subject->size(); path_idx++){//path_idx0 is univesal_plane
        for(int edge_idx=0; edge_idx<(*subject)[path_idx].size(); edge_idx++){
            sub_vec_set[path_idx].push_back(getNthEdgeVec(&(*subject)[path_idx], edge_idx, true));
        }
    }

    vector<float> obj_slope_set;
    /**/qDebug() << "- obj_slope_set" << "@getNFP";
    for(int edge_idx=0; edge_idx<convex_obj.size(); edge_idx++){
        /**/qDebug()<<convex_obj[edge_idx].X<<convex_obj[edge_idx].Y << getNthEdgeSlope(&convex_obj, edge_idx, false);
        obj_slope_set.push_back(getNthEdgeSlope(&convex_obj, edge_idx, false));
    }

    vector<IntPoint> obj_vec_set;
    /**/qDebug() << "- obj_vec_set" << "@getNFP";
    for(int edge_idx=0; edge_idx<convex_obj.size(); edge_idx++){
        obj_vec_set.push_back(getNthEdgeVec(&convex_obj, edge_idx, false));
    }

    /**/qDebug() << "- nfp setup done" << "@getNFP";

    Paths raw_nfp_set;
    raw_nfp_set.resize(subject->size());
    for(int path_idx=1; path_idx<subject->size(); path_idx++){//path_idx0 is univesal_plane
        vector<vector<IntPoint>> obj_vecs_in_regions = getObjVecsInRegions(&sub_slope_set[path_idx], &obj_slope_set, &obj_vec_set);
        /**/qDebug() << "- getObjVecsInRegions done" << "@getNFP";
        IntPoint tail = getFirstNFPPoint(sub_slope_set[path_idx][0], (*subject)[path_idx][0], &convex_obj, obj_slope_set);
        /**/qDebug() << "- got nfp first point" << "@getNFP";
        for(int edge_idx=0; edge_idx<(*subject)[path_idx].size(); edge_idx++){
            /**/qDebug() << "- subject edge" << edge_idx << obj_vecs_in_regions[edge_idx].size() << "@getNFP";
            raw_nfp_set[path_idx].push_back(tail);
            tail = tanslateIntPoint(tail, sub_vec_set[path_idx][edge_idx]);
            for(int obj_vec_idx=0; obj_vec_idx<obj_vecs_in_regions[edge_idx].size(); obj_vec_idx++){
                raw_nfp_set[path_idx].push_back(tail);
                /**/qDebug() << obj_vecs_in_regions[edge_idx][obj_vec_idx].X <<","<<obj_vecs_in_regions[edge_idx][obj_vec_idx].Y << "@getNFP";
                tail = tanslateIntPoint(tail, obj_vecs_in_regions[edge_idx][obj_vec_idx]);
            }
        }
    }
    /**/qDebug() << "- got raw_nfp_set" << "(size:" << raw_nfp_set.size() << ")" << "@getNFP";
    /**/debugPaths(raw_nfp_set);
    return simplyfyRawNFP(&raw_nfp_set, subject);
}

Paths simplyfyRawNFP(Paths* raw_nfp_set, Paths* subject){
    Paths simplyfied_nfp_set;
    simplyfied_nfp_set.resize(1);//path_idx0 is univesal_plane
    for(int idx=1; idx<raw_nfp_set->size(); idx++){//path_idx0 is univesal_plane
        /**/qDebug() << "- simplyfy raw_nfp[" << idx << "]" << "@simplyfyRawNFP";
        Paths result_paths;
        if(Orientation((*subject)[idx])){
            /**/qDebug() << "- ccw" << "@simplyfyRawNFP";
            SimplifyPolygon((*raw_nfp_set)[idx], result_paths, pftPositive);
        }else{
            /**/qDebug() << "- cw" << "@simplyfyRawNFP";
            SimplifyPolygon((*raw_nfp_set)[idx], result_paths, pftNegative);
        }
        /**/qDebug() << "- raw_nfp[" << idx << "] simplyfied" << "@simplyfyRawNFP";
        /**/qDebug() << "- result_paths size" << result_paths.size() << "@simplyfyRawNFP";
        /**/debugPaths(result_paths);
        simplyfied_nfp_set.push_back(result_paths[0]);
    }
    /**/qDebug() << "- simplyfyRawNFP done" << "@simplyfyRawNFP";
    return simplyfied_nfp_set;
}

vector<vector<IntPoint>> getObjVecsInRegions(vector<float>* sub_slope_set, vector<float>* obj_slope_set, vector<IntPoint>* obj_vec_set){
    vector<vector<IntPoint>> obj_vecs_in_regions;
    //obj_vecs_in_regions.resize(sub_slope_set->size());
    int obj_edge_tail;
    for(int obj_edge_idx=0; obj_edge_idx<obj_slope_set->size(); obj_edge_idx++){
        vector<IntPoint> obj_vecs_in_single_region_front_part;
        vector<IntPoint> obj_vecs_in_single_region;
        if(isOnCCWPath((*sub_slope_set)[0], (*sub_slope_set)[1], (*obj_slope_set)[obj_edge_idx])){
            if(obj_edge_idx==0){
                int obj_edge_idx2 = obj_slope_set->size() - 1;
                while(isOnCCWPath((*sub_slope_set)[0], (*sub_slope_set)[1], (*obj_slope_set)[obj_edge_idx2])&&obj_edge_idx2>=0){
                    obj_vecs_in_single_region_front_part.push_back((*obj_vec_set)[obj_edge_idx2]);
                    obj_edge_idx2--;
                }
            }
            if(obj_vecs_in_single_region_front_part.size() != obj_slope_set->size()){
                obj_edge_tail = obj_edge_idx;
                obj_vecs_in_single_region.push_back((*obj_vec_set)[obj_edge_idx]);
                obj_edge_idx++;
                while(isOnCCWPath((*sub_slope_set)[0], (*sub_slope_set)[1], (*obj_slope_set)[obj_edge_idx])){
                    obj_edge_tail = obj_edge_idx;
                    obj_vecs_in_single_region.push_back((*obj_vec_set)[obj_edge_idx]);
                    obj_edge_idx ++;
                }
            }
            obj_vecs_in_single_region.insert(obj_vecs_in_single_region.begin(),obj_vecs_in_single_region_front_part.rbegin(),obj_vecs_in_single_region_front_part.rend());
            /**/qDebug() << "in region 0" << obj_vecs_in_single_region.size();
            obj_vecs_in_regions.push_back(obj_vecs_in_single_region);
            break;
        }
    }

    for(int sub_edge_idx=1; sub_edge_idx<(*sub_slope_set).size(); sub_edge_idx++){
        vector<IntPoint> obj_vecs_in_single_region;
        int from_slope = (*sub_slope_set)[sub_edge_idx];
        int to_slope = (*sub_slope_set)[(sub_edge_idx+1)%(*sub_slope_set).size()];
        int obj_edge_idx = (obj_edge_tail+1)%(*obj_slope_set).size();
        //int tmp= obj_edge_idx; start from?
        while(isOnCCWPath(from_slope, to_slope, (*obj_slope_set)[obj_edge_idx])){
            obj_edge_tail = obj_edge_idx;
            obj_vecs_in_single_region.push_back((*obj_vec_set)[obj_edge_idx]);
            obj_edge_idx = (obj_edge_idx+1)%(*obj_slope_set).size();
            /**/qDebug() << isOnCCWPath(from_slope, to_slope, (*obj_slope_set)[obj_edge_idx]);
        }
        /**/qDebug() << "in region" << sub_edge_idx << obj_vecs_in_single_region.size();
        obj_vecs_in_regions.push_back(obj_vecs_in_single_region);
    }
    return obj_vecs_in_regions;
}

bool isOnCCWPath(float start, float end, float object){
    if(start<0) start=start+2*M_PI;
    if(end<0) end=end+2*M_PI;
    if(object<0) object=object+2*M_PI;
    if(start<end) return  start<=object && object<end;
    else if(start>end) return  start<=object || object<end;
    else return false;
}

float getNthEdgeSlope(Path* path, int edge_idx, bool isForward){
    if(isForward){
        return getEdgeSlope(&(*path)[edge_idx], &(*path)[(edge_idx+1)%(path->size())]);
    }else{
        return getEdgeSlope(&(*path)[(edge_idx+1)%(path->size())], &(*path)[edge_idx]);
    }
}

float getEdgeSlope(IntPoint* p1, IntPoint* p2){
    //*qDebug() << "- edge slope" << atan2f((p2->Y-p1->Y),(p2->X-p1->X));
    float raw_slope = atan2f((p2->Y-p1->Y),(p2->X-p1->X));
    if(raw_slope<0) return raw_slope + 2*M_PI;
    else return raw_slope;
}

IntPoint getNthEdgeVec(Path* path, int edge_idx, bool isForward){
    if(isForward){
        return getEdgeVec(&(*path)[edge_idx], &(*path)[(edge_idx+1)%(path->size())]);
    }else{
        return getEdgeVec(&(*path)[(edge_idx+1)%(path->size())], &(*path)[edge_idx]);
    }
}

IntPoint getEdgeVec(IntPoint* p1, IntPoint* p2){
    IntPoint vec;
    vec.X = p2->X - p1->X;
    vec.Y = p2->Y - p1->Y;
    /**/qDebug() << vec.X << "," << vec.Y;
    return vec;
}

IntPoint getFirstNFPPoint(float first_sub_slope, IntPoint first_point_sub, Path* obj, vector<float> obj_slope_set){
    /**/qDebug() << "- getFirstNFPPoint start";
    int first_obj_slope_idx = -1;
    float min_slope_diff = 2*M_PI;
    for(int idx=0; idx<obj->size(); idx++){
        float raw_slope_diff = obj_slope_set[idx] - first_sub_slope;
        float slope_diff;
        if(raw_slope_diff<0) slope_diff = raw_slope_diff + 2*M_PI;
        else slope_diff = raw_slope_diff;
        if(slope_diff<min_slope_diff){
            first_obj_slope_idx = idx;
            min_slope_diff = slope_diff;
        }
    }
    qDebug()<<"------";
    qDebug()<<first_sub_slope;
    qDebug()<<obj_slope_set[first_obj_slope_idx];
    IntPoint first_point_obj = (*obj)[first_obj_slope_idx];
    qDebug()<< first_point_obj.X << first_point_obj.Y;
    IntPoint fist_nfp_point;
    fist_nfp_point.X = first_point_sub.X - first_point_obj.X;
    fist_nfp_point.Y = first_point_sub.Y - first_point_obj.Y;
    return fist_nfp_point;
}

IntPoint rotateIntPointRad(IntPoint point, float rot_angle){
    IntPoint rot_point;
    //*qDebug() << "set rot_point";
    rot_point.X = point.X*cosf(rot_angle) - point.Y*sinf(rot_angle);
    rot_point.Y = point.X*sinf(rot_angle) + point.Y*cosf(rot_angle);
    //*qDebug() << "- point rotated";
    return rot_point;
}


//=========================================
//test
//=========================================

void testSimplifyPolygon(){
    Path path;
    int path_data[][2] =
    {
        {0, 0},
        {1, 0},
        {0, 1},
        {0, -1},
        {-1, 0}
    };
    for(int idx=0; idx<sizeof(path_data)/sizeof(path_data[0]);idx++){
        IntPoint point;
        point.X=path_data[idx][0];
        point.Y=path_data[idx][1];
        path.push_back(point);
    }
    Paths result_paths;
    SimplifyPolygon(path, result_paths, pftPositive);
    debugPaths(result_paths);
}

void testClip(){
    Path path1;
    int path1_data[][2] =
    {
        {0, 0},
        {40, 0},
        {40, 40},
        {0, 40}
    };
    for(int idx=0; idx<sizeof(path1_data)/sizeof(path1_data[0]);idx++){
        IntPoint point;
        point.X=path1_data[idx][0];
        point.Y=path1_data[idx][1];
        path1.push_back(point);
    }
    Paths cumulative_paths;
    cumulative_paths.push_back(path1);

    Path path2;
    int path2_data[][2] =
    {
        //{5, 0},
        //{7, 5},
        //{3, 5}

        {10, 10},
        {10, 30},
        {30, 30},
        {30, 10}
    };
    for(int idx=0; idx<sizeof(path2_data)/sizeof(path2_data[0]);idx++){
        IntPoint point;
        point.X=path2_data[idx][0];
        point.Y=path2_data[idx][1];
        path2.push_back(point);
    }
    Paths new_paths;
    new_paths.push_back(path2);

    cumulativeClip(&cumulative_paths, &new_paths);
    debugPaths(cumulative_paths);
}
