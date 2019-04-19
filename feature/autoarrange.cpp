#include "autoarrange.h"
#define ANGLE_UNIT 45
#define STAGE_WIDTH 95000
#define STAGE_HEIGHT 75000
#define OFFSET 2000
#include <qmlmanager.h>

autoarrange::autoarrange()
{

}

Paths autoarrange::getMeshRecArea(const Mesh& mesh){//getting rectangle area of mesh in XY Plane, OFFSET included
    Paths outline;
    Path vertices;//all vertices in mesh
    Path vertices45rot;//45 degree check
    for(const auto& vertex : mesh.getVertices()){
        QVector3D v_pos = vertex.position;
        mesh.addPoint(v_pos.x(), v_pos.y(), &vertices);
        mesh.addPoint(round(v_pos.x()*cosf(M_PI/4) - v_pos.y()*sinf(M_PI/4)), round(v_pos.x()*sinf(M_PI/4) + v_pos.y()*cosf(M_PI/4)), &vertices45rot);//45 degree check
    }
    int max_x = getMaxX(vertices) + OFFSET;
    int max_y = getMaxY(vertices) + OFFSET;
    int min_x = getMinX(vertices) - OFFSET;
    int min_y = getMinY(vertices) - OFFSET;
    Path recArea = {IntPoint(max_x, max_y),
                  IntPoint(min_x, max_y),
                  IntPoint(min_x, min_y),
                  IntPoint(max_x, min_y)};
    int tmp = (max_x-min_x)*(max_y-min_y);

    max_x = getMaxX(vertices45rot) + OFFSET;
    max_y = getMaxY(vertices45rot) + OFFSET;
    min_x = getMinX(vertices45rot) - OFFSET;
    min_y = getMinY(vertices45rot) - OFFSET;
    Path recArea45rot = {IntPoint(max_x, max_y),
                        IntPoint(min_x, max_y),
                        IntPoint(min_x, min_y),
                        IntPoint(max_x, min_y)};
    int tmp45rot = (max_x-min_x)*(max_y-min_y);

    if(tmp<=tmp45rot){
        outline.push_back(recArea);

    }else{
        outline.push_back(recArea45rot);
        outline = rotatePaths(outline,-45);
    }

    //outline.push_back(recArea);
    /**/qDebug() << "got MeshRecArea";
    /**/debugPaths(outline);
    return outline;
}

Paths autoarrange::getMeshConvexHull(const Mesh& mesh){//getting convex hull area of mesh in XY Plane
    Paths outline;
    Path vertices;//all vertices in mesh
    for(const auto& vertex : mesh.getVertices()){
        QVector3D v_pos = vertex.position;
        mesh.addPoint(v_pos.x(), v_pos.y(), &vertices);
    }
    outline.push_back(getConvexHull(&vertices));
    /**/qDebug() << "got MeshConvexHull";
    return outline;
}

Paths autoarrange::spreadingCheck(const Mesh* mesh, std::vector<bool>& check, int chking_start, bool is_chking_pos){
    /**/qDebug() << "SpreadingCheck started from" << chking_start;
    Paths paths;
    int chking = -1;
    vector<const MeshFace*> to_check;
    to_check.push_back(& mesh->getFaces()[chking_start]);
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
            const MeshFace* mf = to_check[i];
            int side;
            int outline_checked = false;
            for(side=0; side<3; side++){
                if(isEdgeBound(mf, side, is_chking_pos)){
                    if(!outline_checked){
                        int path_head = getPathHead(mf, side, is_chking_pos);
                        Path path = buildOutline(mesh, check, chking, path_head, is_chking_pos);
                        if(path.size()==0) return {};
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

int autoarrange::getPathHead(const MeshFace* mf, int side, bool is_chking_pos){
    if(side==0 && isEdgeBound(mf, 2, is_chking_pos)) {
        if(isEdgeBound(mf, 1, is_chking_pos)) return -1;//all side of chking face is bound, face is alone
        else return mf->mesh_vertex[2];
    }
    return mf->mesh_vertex[side];
}

Path autoarrange::buildOutline(const Mesh* mesh, std::vector<bool>& check, int chking, int path_head, bool is_chking_pos){
    //**qDebug() << "buildOutline from" << chking;
    vector<int> path_by_idx;
    if(path_head==-1){//혼자있는 면의 경우 오리엔테이션 확인 방법이 마련되어있지 않음
        check[chking] = true;
        path_by_idx = arrToVect(mesh->getFaces()[chking].mesh_vertex);
        //**qDebug() << "buildOutline done";
        return idxsToPath(mesh, path_by_idx);
    }
    bool outline_closed = false;
    int from = -1;
    int nxt_chk = -1;
    int path_tail = path_head;
    while(!outline_closed){
        if(check[chking]){
            qDebug() << "mesh error";
            return {};
        }
        //**qDebug() << "chking" << chking;
        const MeshFace* mf = & mesh->getFaces()[chking];
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

bool autoarrange::isEdgeBound(const MeshFace* mf, int side, bool is_chking_pos){
    //condition of bound edge:
    //1. connected to face with opposit fn.z
    //2. not connected any face
    //3. multiple neighbor
    //4. opposit orientation
    if(mf->neighboring_faces[side].size() != 1) return true;
    MeshFace* neighbor = mf->neighboring_faces[side][0];
    if(!checkFNZ(neighbor, is_chking_pos)) return true;
    if(!isNbrOrientSame(mf, side)) return true;
    return false;
}

bool autoarrange::isNbrOrientSame(const MeshFace* mf, int side){
    MeshFace* nbr = mf->neighboring_faces[side][0];
    if(getNbrVtx(nbr, searchVtxInFace(nbr, mf->mesh_vertex[side]), 2) == getNbrVtx(mf, side, 1)) return true;
    return false;
}

int autoarrange::searchVtxInFace(const MeshFace* mf, int vertexIdx){
    for(int i=0; i<3; i++){
        if(mf->mesh_vertex[i] == vertexIdx) return i;
    }
    return -1;
}

vector<int> autoarrange::arrToVect(const int arr[]){
    vector<int> vec (arr, arr + sizeof arr / sizeof arr[0]);
    return vec;
}

int autoarrange::getNbrVtx(const MeshFace* mf, int base, int xth){//getNeighborVtx
    if(xth>0) return mf->mesh_vertex[(base+xth+3)%3];
    else return -1;
}

Path autoarrange::idxsToPath(const Mesh* mesh, vector<int> path_by_idx){
    Path path;
    for(int idx : path_by_idx){
        QVector3D vertex = mesh->getVertices()[idx].position;
        mesh->Mesh::addPoint(vertex.x(), vertex.y(), &path);
    }
    return path;
}

Paths autoarrange::project(const Mesh* mesh){
    int faces_size = mesh->getFaces().size();
    vector<Paths> outline_sets;
    bool is_chking_pos = false;
    bool mesh_error = false;
    for(int i=0; i<2; i++){
        is_chking_pos = !is_chking_pos;
        bool check_done = false;
        std::vector<bool> face_checked(faces_size,false); //한 번 확인한 것은 체크리스트에서 제거되는 자료구조 도입 필요(법선 확인이 반복시행됨)
        /****/qDebug() << "Get outline(is_chking_pos:" << is_chking_pos << ")";
        while(!check_done && !mesh_error){
            for(int face_idx=0; face_idx<faces_size; face_idx++){
                if(checkFNZ(& mesh->getFaces()[face_idx], is_chking_pos) && !face_checked[face_idx]){
                    outline_sets.push_back(spreadingCheck(mesh, face_checked, face_idx, is_chking_pos));
                    if(outline_sets[outline_sets.size()-1].size()==0) mesh_error = true;
                    break;
                }else if(face_idx==faces_size-1) check_done = true;
            }
        }
        if(mesh_error) break;
    }
    if(mesh_error){
        return getMeshConvexHull(*mesh);
    }
    else{
        /****/qDebug() << "Total outline_set : " << outline_sets.size() << "->" << clipOutlines(outline_sets).size();
        return clipOutlines(outline_sets);
    }
}

Paths autoarrange::clipOutlines(vector<Paths> outline_sets){
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

bool autoarrange::checkFNZ(const MeshFace* face, bool is_chking_pos){
    if(is_chking_pos){
        if(face->fn.z()>=0) return true;
        else return false;
    }else{
        if(face->fn.z()<=0) return true;
        else return false;
    }
}

void autoarrange::debugPaths(Paths paths){
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

void autoarrange::debugPath(Path path){
    qDebug() << "===============";
    for(int i=0; i<path.size(); i++){
        qDebug()<< path[i].X << path[i].Y;
    }
    qDebug() << "===============";
}

void autoarrange::debugFaces(const Mesh* mesh, vector<int> face_list){
    for(int i=0; i<face_list.size(); i++){
        debugFace(mesh, face_list[i]);
    }
}

void autoarrange::debugFace(const Mesh* mesh, int face_idx){
        const MeshFace* mf = & mesh->getFaces()[face_idx];
        qDebug() << "face #" << face_idx;
        for(int side=0; side<3; side++){
            QVector3D vtx = mesh->getVertices()[mf->mesh_vertex[side]].position;
            float x_f = vtx.x();
            float y_f = vtx.y();
            float z_f = vtx.z();
            int x_int = round(x_f*scfg->resolution);
            int y_int = round(y_f*scfg->resolution);
            int z_int = round(z_f*scfg->resolution);
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

int autoarrange::findVertexWithIntpoint(IntPoint p, const Mesh* mesh){
    return findVertexWithIntXY(p.X, p.Y, mesh);
}

int autoarrange::findVertexWithIntXY(int x, int y, const Mesh* mesh){
    for(int vtx_idx=0; vtx_idx<mesh->getVertices().size(); vtx_idx++){
        QVector3D vtx_pos = mesh->getVertices()[vtx_idx].position;
        int x_int = round(vtx_pos.x()*scfg->resolution);
        int y_int = round(vtx_pos.y()*scfg->resolution);
        if(x_int==x && y_int==y) return vtx_idx;
    }
}

//=========================================
//Ramer–Douglas–Peucker line simplification
//=========================================
void autoarrange::RDPSimpPaths(Paths* paths){
    for(int path_idx=0; path_idx<paths->size(); path_idx++){
        RDPSimp(&(*paths)[path_idx]);
        if((*paths)[path_idx].size()==2) {
            paths->erase(paths->begin()+path_idx);
            path_idx--;
        }
    }
}

void autoarrange::RDPSimp(Path* path){
    int path_size = path->size();
    std::vector<bool> keepingPointMrkArr(path_size, false);
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

void autoarrange::RDPSimpIter(Path* path, int start, int end, std::vector<bool>& keepingPointMrkArr){
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

float autoarrange::distL2P(IntPoint* line_p1, IntPoint* line_p2, IntPoint* p){
    float triangle_area_double = abs(line_p1->X*(line_p2->Y-p->Y) + line_p2->X*(p->Y-line_p1->Y) +p->X*(line_p1->Y-line_p2->Y));
    float bottom_side_length = sqrt(pow((line_p1->X-line_p2->X),2) + pow((line_p1->Y-line_p2->Y),2));
    return triangle_area_double/bottom_side_length;
}

//=========================================
//Offset Function
//=========================================

void autoarrange::offsetPath(Paths* paths){
    Paths out;
    ClipperOffset co;
    co.AddPaths(*paths, jtSquare, etClosedPolygon);
    //if(Orientation(*paths)) co.Execute(out, offset);
    //else co.Execute(out, -offset);
    co.Execute(out, OFFSET);
    //*debugPaths(out);
    *paths = out;
}

void autoarrange::offsetPaths_rec(Paths& paths){
    paths[0][0] = {paths[0][0].X + 3, paths[0][0].Y + 3};
    paths[0][1] = {paths[0][1].X - 3, paths[0][1].Y + 3};
    paths[0][2] = {paths[0][2].X - 3, paths[0][2].Y - 3};
    paths[0][3] = {paths[0][3].X + 3, paths[0][3].Y - 3};
}

//=========================================
//Arrangement Algorithm
//=========================================

//typedef pair<IntPoint, float> XYArrangement;

vector<XYArrangement> autoarrange::simpArngMeshes(vector<Mesh>& meshes){
    vector<Paths> outlines;
    /**/qDebug() << "Arrange start";
    for(int idx=0; idx<meshes.size(); idx++){
        outlines.push_back(getMeshConvexHull(meshes[idx]));
        RDPSimpPaths(&outlines[idx]);
        offsetPath(&outlines[idx]);
    }
    /**/qDebug() << "Projection done @arngMeshes";
    return arng2D(outlines);
}

vector<XYArrangement> autoarrange::arngMeshes(vector<Mesh>& meshes){
    vector<Paths> outlines;
    /**/qDebug() << "Arrange start";
    qmlManager->setProgress(0);
    qmlManager->setProgressText("Getting projection of meshes on work plane...");
    for(int idx=0; idx<meshes.size(); idx++){
        //outlines.push_back(project(& meshes[idx]));
        //RDPSimpPaths(&outlines[idx]);
        //offsetPath(&outlines[idx]);
        outlines.push_back(getMeshRecArea(meshes[idx]));
        qmlManager->setProgress(0.5*idx/meshes.size());
    }
    return arng2D(outlines);
}

bool compareArea(pair<Paths*, int>& fig1, pair<Paths*, int>& fig2){//used for sort
    return Area((*fig1.first)[0])>Area((*fig2.first)[0]);
}

vector<XYArrangement> autoarrange::arng2D(vector<Paths>& figs){
    qmlManager->setProgressText("Simulating arrangement");
    vector<pair<Paths*, int> > figs_with_idx;//tried to use reference istead of pointer, but it caused error
    for(Paths& fig : figs) figs_with_idx.push_back({&fig, figs_with_idx.size()});
    sort(figs_with_idx.begin(), figs_with_idx.end(), compareArea);
    vector<XYArrangement> arng_result_set;
    arng_result_set.resize(figs.size(), {{0,0},-1});
    Paths cum_outline;
    initStage(cum_outline);
    int cumXOfNotArrangeable = 0;
    int progress = 0;
    for(pair<Paths*, int>& fig_with_idx : figs_with_idx){
        Paths& fig = *fig_with_idx.first;
        XYArrangement new_result = arngFig(cum_outline, fig);
        if(new_result.second==-1){//unable to arrange
            new_result.first = {-abs(cum_outline[0][0].X) + cumXOfNotArrangeable - getMinX(fig[0]),
                                abs(cum_outline[0][0].Y) - getMinY(fig[0])};
            new_result.second = 0;
            cumXOfNotArrangeable = cumXOfNotArrangeable + getMaxX(fig[0]) - getMinX(fig[0]);
        }
        arng_result_set[fig_with_idx.second] = new_result;
        /**/qDebug() << "- fig" << fig_with_idx.second << "arngd";
        qmlManager->setProgress(0.5 + 0.5*(++progress)/figs_with_idx.size());
    }
    /**/qDebug() << "Arrange all done";
    /**/debugPaths(cum_outline);
    //test position
    return arng_result_set;
}

void autoarrange::initStage(Paths& cum_outline){
    //universal_plane의 중앙이 (0,0)이라는 특성을 외부 함수에서 사용
    int half_width = STAGE_WIDTH/2;
    int half_height = STAGE_HEIGHT/2;
    //stage as hole, cw(hole)
    Path stage = {IntPoint(half_width, half_height),
                  IntPoint(half_width, -half_height),
                  IntPoint(-half_width, -half_height),
                  IntPoint(-half_width, half_height)};
    //set universal_plane to make placeable space have zero filling value, ccw(fill)
    Path universal_plane = {IntPoint(half_width+1, half_height+1),
                            IntPoint(-half_width-1, half_height+1),
                            IntPoint(-half_width-1, -half_height-1),
                            IntPoint(half_width+1, -half_height-1)};
    cum_outline.push_back(universal_plane);
    cum_outline.push_back(stage);
}

XYArrangement autoarrange::newArngFig(Paths& cum_outline, Paths& fig){
    /**/qDebug() << "Arrange new fig";
    Paths optimal_rot_fig;//_ptr;
    XYArrangement optimal_arrangement;
    int min_width = abs(cum_outline[0][0].X);//universal plane 상 최대 X
    int min_height = abs(cum_outline[0][0].Y);//universal plane 상 최대 Y
    for(float rot_angle=0; rot_angle<360; rot_angle += ANGLE_UNIT){
        IntPoint optimal_point;
        Paths rot_fig = rotatePaths(fig, rot_angle);
        Paths fig_extra_offset = fig;
        offsetPaths_rec(fig_extra_offset);
        Paths rot_fig_extra_offset = rotatePaths(fig_extra_offset, rot_angle);
        Paths nfp_set = getNFP(cum_outline, rot_fig_extra_offset);
        /*left for trouble shooting, there is known error case using this version
        if(nfp_set.size()>0) optimal_point = getOptimalPoint(nfp_set);
        else optimal_point = {abs(cum_outline[0][0].X), abs(cum_outline[0][0].Y)};//*/
        if(nfp_set.size()==0) continue;
        optimal_point = getOptimalPoint(nfp_set);
        //max relative x(and y) of rot_fig + optimal_point.X(and Y), paths[0]이 가장 외곽일거라 가정
        int tmp_min_width = getMaxX(rot_fig[0]) - rot_fig[0][0].X  + optimal_point.X;
        int tmp_min_height = getMaxY(rot_fig[0]) - rot_fig[0][0].Y  + optimal_point.Y;\
        if(tmp_min_width<min_width || (tmp_min_width==min_width && tmp_min_height<min_height)){
            optimal_rot_fig = rot_fig;
            optimal_arrangement = {optimal_point, rot_angle};
            min_width = tmp_min_width;
            min_height = tmp_min_height;
        }
    }
    if(min_width == abs(cum_outline[0][0].X)){
        /**/qDebug() << "- arng imposible";
        return make_pair(IntPoint(0,0),-1);
    }
    //Paths& optimal_rot_fig = *optimal_rot_fig_ptr;
    tanslatePaths(optimal_rot_fig, optimal_arrangement.first);
    cumulativeClip(cum_outline, optimal_rot_fig);
    /**/qDebug() << "- cumulativeClip done";
    return optimal_arrangement;
}

XYArrangement autoarrange::arngFig(Paths& cum_outline, Paths& fig){
    /**/qDebug() << "Arrange new fig";
    Paths optimal_rot_fig;//_ptr;
    XYArrangement optimal_arrangement;
    int min_width = abs(cum_outline[0][0].X);//universal plane 상 최대 X
    int min_height = abs(cum_outline[0][0].Y);//universal plane 상 최대 Y
    for(float rot_angle=0; rot_angle<360; rot_angle += ANGLE_UNIT){
        IntPoint optimal_point;
        Paths rot_fig = rotatePaths(fig, rot_angle);
        Paths nfp_set = getNFP(cum_outline, rot_fig);
        /*left for trouble shooting, there is known error case using this version
        if(nfp_set.size()>0) optimal_point = getOptimalPoint(nfp_set);
        else optimal_point = {abs(cum_outline[0][0].X), abs(cum_outline[0][0].Y)};//*/
        if(nfp_set.size()==0) continue;
        optimal_point = getOptimalPoint(nfp_set);
        //max relative x(and y) of rot_fig + optimal_point.X(and Y), paths[0]이 가장 외곽일거라 가정
        int tmp_min_width = getMaxX(rot_fig[0]) - rot_fig[0][0].X  + optimal_point.X;
        int tmp_min_height = getMaxY(rot_fig[0]) - rot_fig[0][0].Y  + optimal_point.Y;\
        if(tmp_min_width<min_width || (tmp_min_width==min_width && tmp_min_height<min_height)){
            optimal_rot_fig = rot_fig;
            optimal_arrangement = {optimal_point, rot_angle};
            min_width = tmp_min_width;
            min_height = tmp_min_height;
        }
    }
    if(min_width == abs(cum_outline[0][0].X)){
        /**/qDebug() << "- arng imposible";
        return make_pair(IntPoint(0,0),-1);
    }
    //Paths& optimal_rot_fig = *optimal_rot_fig_ptr;
    tanslatePaths(optimal_rot_fig, optimal_arrangement.first);
    cumulativeClip(cum_outline, optimal_rot_fig);
    /**/qDebug() << "- cumulativeClip done";
    return optimal_arrangement;
}

Paths autoarrange::rotatePaths(Paths& paths, float rot_angle){//Rotation about origin(because QTransform does so)
    Paths rot_paths;
    rot_paths.resize(paths.size());
    for(int path_idx=0; path_idx<paths.size();path_idx++){
        for(IntPoint& point : paths[path_idx]){
            rot_paths[path_idx].push_back(rotatePoint(point, rot_angle));
        }
    }
    return rot_paths;
}

IntPoint autoarrange::rotatePoint(IntPoint& point, float rot_angle){//Rotation about origin(because QTransform does so)
    float rad_angle = rot_angle*M_PI/180;
    /*left for trouble shooting, there is known error case using this version
    IntPoint rot_point;
    rot_point.X = point.X*cosf(rad_angle) - point.Y*sinf(rad_angle);
    rot_point.Y = point.X*sinf(rad_angle) + point.Y*cosf(rad_angle);
    return rot_point;//*/
    return {(int)round(point.X*cosf(rad_angle) - point.Y*sinf(rad_angle)), (int)round(point.X*sinf(rad_angle) + point.Y*cosf(rad_angle))};
}

void autoarrange::tanslatePaths(Paths& paths, IntPoint& tanslate_vec){
    Paths trans_paths;
    trans_paths.resize(paths.size());
    for(int path_idx=0; path_idx<paths.size();path_idx++){
        for(IntPoint& point : paths[path_idx]){
            trans_paths[path_idx].push_back(tanslatePoint(point, tanslate_vec));
        }
    }
    paths.swap(trans_paths);
    trans_paths.clear();
}

IntPoint autoarrange::tanslatePoint(IntPoint& point, IntPoint& tanslate_vec){
    IntPoint tanslated_point = point + tanslate_vec;
    return tanslated_point;
}

void autoarrange::cumulativeClip(Paths& cum_outline, Paths& new_fig){
    Clipper c;
    c.AddPaths(cum_outline, ptSubject, true);
    c.AddPaths(new_fig, ptClip, true);
    c.Execute(ctUnion, cum_outline, pftPositive, pftPositive);
}

IntPoint autoarrange::getOptimalPoint(Paths& nfp_set){
    IntPoint opt_point = {nfp_set[0][0].X, nfp_set[0][0].Y};
    for(Path& path : nfp_set){
        for(IntPoint& point : path){
            if(point.X < opt_point.X || (point.X == opt_point.X && point.Y < opt_point.Y)){
                opt_point = point;
            }
        }
    }
    return opt_point;
}

int autoarrange::getMaxX(Path& path){
    int max_x = path[0].X;
    for(IntPoint& point : path) if(max_x<point.X) max_x=point.X;
    return max_x;
}

int autoarrange::getMinX(Path& path){
    int min_x = path[0].X;
    for(IntPoint& point : path) if(min_x>point.X) min_x=point.X;
    return min_x;
}

int autoarrange::getMaxY(Path& path){
    int max_y = path[0].Y;
    for(IntPoint& point : path) if(max_y<point.Y) max_y=point.Y;
    return max_y;
}

int autoarrange::getMinY(Path& path){
    int min_y = path[0].Y;
    for(IntPoint& point : path) if(min_y>point.Y) min_y=point.Y;
    return min_y;
}

Paths autoarrange::getNFP(Paths& subject, Paths& object){
    Path convex_obj = getConvexHull(&object[0]);
    /**/qDebug() << "- got object convexHull" << checkConvex(convex_obj);
    //*debugPath(convex_obj);

    vector<Vecs> sub_vecs;
    sub_vecs.resize(subject.size());
    for(int path_idx=0; path_idx<subject.size(); path_idx++){//path_idx0 is univesal_plane
        for(int edge_idx=0; edge_idx<subject[path_idx].size(); edge_idx++){
            sub_vecs[path_idx].push_back(getEdgeVec(subject[path_idx], edge_idx, true));
        }
    }

    Vecs obj_vecs;
    for(int edge_idx=0; edge_idx<convex_obj.size(); edge_idx++){
        obj_vecs.push_back(getEdgeVec(convex_obj, edge_idx, false));
    }

    Paths raw_nfp_set;
    raw_nfp_set.resize(subject.size());
    for(int path_idx=1; path_idx<subject.size(); path_idx++){//path_idx0 is univesal_plane
        vector<Vecs> obj_vecs_in_regions = getObjVecsInRegions(sub_vecs[path_idx], obj_vecs);
        IntPoint tail = getFirstNFPPoint(sub_vecs[path_idx][0], subject[path_idx][0], convex_obj, obj_vecs);
        for(int edge_idx=0; edge_idx<subject[path_idx].size(); edge_idx++){
            raw_nfp_set[path_idx].push_back(tail);
            tail = tanslatePoint(tail, sub_vecs[path_idx][edge_idx]);
            for(int obj_vec_idx=0; obj_vec_idx<obj_vecs_in_regions[edge_idx].size(); obj_vec_idx++){
                raw_nfp_set[path_idx].push_back(tail);
                tail = tanslatePoint(tail, obj_vecs_in_regions[edge_idx][obj_vec_idx]);
            }
        }
        /**/if(tail!=raw_nfp_set[path_idx][0]) qDebug() << "raw_nfp" << path_idx << "not closed";
    }
    /**/qDebug() << "- got raw_nfp_set" << "(size:" << raw_nfp_set.size() << ")";
    //*debugPaths(raw_nfp_set);
    Paths separate_nfp_set = simplyfyRawNFP(raw_nfp_set, subject);
    return mergeNFP(separate_nfp_set);
}

Paths autoarrange::simplyfyRawNFP(Paths& raw_nfp_set, Paths& subject){
    Paths simplyfied_nfp_set;
    simplyfied_nfp_set.resize(1);//path_idx0 is univesal_plane
    for(int idx=1; idx<raw_nfp_set.size(); idx++){//path_idx0 is univesal_plane
        Paths result_paths;
        if(subject[idx].size() != 0){
            if(Orientation(subject[idx])){
                SimplifyPolygon(raw_nfp_set[idx], result_paths, pftPositive);
                if(result_paths.size()>0 && result_paths[0].size()>0){
                    if(Orientation(result_paths[0])) ReversePath(result_paths[0]);
                }
            }else{
                SimplifyPolygon(raw_nfp_set[idx], result_paths, pftNegative);
                if(result_paths.size()>0 && result_paths[0].size()>0){
                    if(!Orientation(result_paths[0])) ReversePath(result_paths[0]);
                }
            }
        }
        //*qDebug() << "- raw_nfp[" << idx << "] simplyfied" << result_paths.size();
        //*debugPaths(result_paths);
        if(result_paths.size()>0) simplyfied_nfp_set.push_back(result_paths[0]);
        else simplyfied_nfp_set.push_back({});
    }
    return simplyfied_nfp_set;
}

Paths autoarrange::mergeNFP(Paths& separate_nfp_set){
    Paths merged_nfp;
    for(Path& nfp : separate_nfp_set){
        if(nfp.size()>0){
            Clipper c;
            c.AddPaths(merged_nfp, ptSubject, true);
            c.AddPath(nfp, ptClip, true);
            if(Orientation(nfp)) c.Execute(ctUnion, merged_nfp, pftPositive, pftPositive);
            else c.Execute(ctDifference, merged_nfp, pftPositive, pftNegative);
        }
    }
    return merged_nfp;
}

vector<Vecs> autoarrange::getObjVecsInRegions(Vecs& sub_vecs, Vecs& obj_vecs){
    vector<Vecs> obj_vecs_in_regions;
    int obj_edge_tail;
    for(int obj_edge_idx=0; obj_edge_idx<obj_vecs.size(); obj_edge_idx++){
        Vecs obj_vecs_in_single_region_front_part;
        Vecs obj_vecs_in_single_region;
        if(isVecCCW(sub_vecs[0], obj_vecs[obj_edge_idx], sub_vecs[1])){
            if(obj_edge_idx==0){
                int obj_edge_idx2 = obj_vecs.size() - 1;
                while(isVecCCW(sub_vecs[0], obj_vecs[obj_edge_idx2], sub_vecs[1])&&obj_edge_idx2>=0){
                    obj_vecs_in_single_region_front_part.push_back(obj_vecs[obj_edge_idx2]);
                    obj_edge_idx2--;
                }
            }
            if(obj_vecs_in_single_region_front_part.size() != obj_vecs.size()){
                int start_from = obj_edge_idx;
                obj_edge_tail = obj_edge_idx;
                obj_vecs_in_single_region.push_back(obj_vecs[obj_edge_idx]);
                obj_edge_idx = (obj_edge_idx+1)%obj_vecs.size();
                while(isVecCCW(sub_vecs[0], obj_vecs[obj_edge_idx], sub_vecs[1]) && obj_edge_idx != start_from){
                    obj_edge_tail = obj_edge_idx;
                    obj_vecs_in_single_region.push_back(obj_vecs[obj_edge_idx]);
                    obj_edge_idx = (obj_edge_idx+1)%obj_vecs.size();
                }
            }
            obj_vecs_in_single_region.insert(obj_vecs_in_single_region.begin(),obj_vecs_in_single_region_front_part.rbegin(),obj_vecs_in_single_region_front_part.rend());
            //*qDebug() << "in region 0" << obj_vecs_in_single_region.size();
            obj_vecs_in_regions.push_back(obj_vecs_in_single_region);
            break;
        }
    }
    if(obj_vecs_in_regions.size() == 0){
        Vecs obj_vecs_in_single_region;
        //*qDebug() << "in region 0 0";
        obj_vecs_in_regions.push_back(obj_vecs_in_single_region);
    }
    for(int sub_edge_idx=1; sub_edge_idx<sub_vecs.size(); sub_edge_idx++){
        Vecs obj_vecs_in_single_region;
        IntPoint from_slope = sub_vecs[sub_edge_idx];
        IntPoint to_slope = sub_vecs[(sub_edge_idx+1)%sub_vecs.size()];
        int obj_edge_idx = (obj_edge_tail+1)%obj_vecs.size();
        int start_from = -1;
        while(isVecCCW(from_slope, obj_vecs[obj_edge_idx], to_slope) && obj_edge_idx != start_from){
            if(start_from == -1) start_from=obj_edge_idx;
            obj_edge_tail = obj_edge_idx;
            obj_vecs_in_single_region.push_back(obj_vecs[obj_edge_idx]);
            obj_edge_idx = (obj_edge_idx+1)%obj_vecs.size();
        }
        //*qDebug() << "in region" << sub_edge_idx << obj_vecs_in_single_region.size();
        obj_vecs_in_regions.push_back(obj_vecs_in_single_region);
    }
    return obj_vecs_in_regions;
}

bool autoarrange::isVecAlign(const IntPoint& a, const IntPoint& b){
    return a.X*b.Y==a.Y*b.X && a.X*b.X>=0 && a.Y*b.Y>=0;
}

bool autoarrange::isCCW(const IntPoint& a, const IntPoint& b, const IntPoint& c){
    return ccw(a, b, c)==-1;
}

bool autoarrange::isVecCCW(const IntPoint& a, const IntPoint& b, const IntPoint& c){
    if(isVecAlign(b, c)) return false;
    if(isVecAlign(a, b)) return true;
    IntPoint o = {0,0};
    if(isCCW(a, o, c)){
        if(isCCW(a, o, b) && isCCW(b, o, c)) return false;
        return true;
    }else{
        if(isCCW(c, o, b) && isCCW(b, o, a)) return true;
        return false;
    }
}

IntPoint autoarrange::getEdgeVec(const Path& path, int edge_idx, bool isForward){
    if(isForward){
        return path[(edge_idx+1)%(path.size())] - path[edge_idx];
    }else{
        return path[edge_idx] - path[(edge_idx+1)%(path.size())];
    }
}

IntPoint autoarrange::getFirstNFPPoint(const IntPoint& first_sub_vec, const IntPoint& first_sub_vtx, const Path& obj, const Vecs& obj_vecs){
    int first_obj_vec = 0;
    for(int idx=1; idx<obj.size(); idx++){
        if(isVecCCW(first_sub_vec, obj_vecs[idx], obj_vecs[first_obj_vec])) first_obj_vec = idx;
    }
    IntPoint first_obj_vtx = obj[first_obj_vec];
    IntPoint first_nfp_vtx = first_sub_vtx - first_obj_vtx;
    return first_nfp_vtx;
}

//=========================================
//arrange Qt3D
//=========================================

void autoarrange::arrangeQt3D(vector<Qt3DCore::QTransform*> m_transform_set, vector<XYArrangement> arng_result_set){
    for(int idx=0; idx<arng_result_set.size(); idx++){
        arrangeSingleQt3D(m_transform_set[idx], arng_result_set[idx]);
    }
}

void autoarrange::arrangeSingleQt3D(Qt3DCore::QTransform* m_transform, XYArrangement arng_result){
    QVector3D trans_vec = QVector3D(arng_result.first.X/scfg->resolution, arng_result.first.Y/scfg->resolution, m_transform->translation().z());
    m_transform->setTranslation(trans_vec);
    m_transform->setRotationZ(arng_result.second);
}

/*void autoarrange::arrangeGlmodels(vector<GLModel*>* glmodels){
    vector<Mesh> meshes_to_arrange;
    vector<XYArrangement> arng_result_set;
    vector<Qt3DCore::QTransform*> m_transform_set;
    for(int idx=0; idx<glmodels->size(); idx++){
        meshes_to_arrange.push_back(* (*glmodels)[idx]->mesh);
        m_transform_set.push_back((*glmodels)[idx]->m_transform);
    }
    arng_result_set = arngMeshes(&meshes_to_arrange);
    arrangeQt3D(m_transform_set, arng_result_set);
}*/

//=========================================
//check
//=========================================
bool autoarrange::checkConvex(Path& path){
    int not_allowed = -1;
    if(Orientation(path)) not_allowed = 1;
    IntPoint p1 = path[path.size()-2];
    IntPoint p2 = path[path.size()-1];
    for(IntPoint p3 : path){
        if(ccw(p1, p2, p3)==not_allowed) return false;
        p1 = p2;
        p2 = p3;
    }
    return true;
}

//=========================================
//test
//=========================================

void autoarrange::testSimplifyPolygon(){
    Path path;
    int path_data[][2] =
    {
        {0, 0},
        {1, 0},
        {2, 0},
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
    qDebug() << "testSimplifyPolygon";
    debugPaths(result_paths);
}

void autoarrange::testClip(){
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

    cumulativeClip(cumulative_paths, new_paths);
    debugPaths(cumulative_paths);
}

void autoarrange::testOffset(){

    Path path1;
    int path1_data[][2] =
    {
        {400, 400},
        {400, 0},
        {0, 0},
        {0, 400}
    };
    for(int idx=0; idx<sizeof(path1_data)/sizeof(path1_data[0]);idx++){
        IntPoint point;
        point.X=path1_data[idx][0];
        point.Y=path1_data[idx][1];
        path1.push_back(point);
    }
    Paths paths1;
    paths1.push_back(path1);

    //offsetPath(&path1);
    debugPath(path1);
}

/*
 * 연결된 face들로 구성된 집합의 외곽 경로를 구하는 함수 입니다.
 * mesh와 탐색을 시작할 면을 인풋으로 받습니다.
 *
 *   1. 면f가 구하고자 하는 면들의 집햅에 포함되고
 *   2. 면f의 n번째 변에 대해 isEdgeBound(f,n)이 거짓일 때
 *   ->  면f과 n번째 변을 공유하는 면(이웃)은 집합에 포함됩니다.
 *
 *   1. 면f가 n번째 변을 공유하는 이웃이 유일하게 존재하고
 *   2. 면f과 n번째 변을 공유하는 이웃이 같은 orientation을 가지며
 *   3. meetNbrCondExt(이웃 면)이 참일 떄
 *   -> isEdgeBound(f,n)은 거짓입니다.
 *   (1번 및 2번 조건을 제거/수정할 경우 나머지 코드들이 유효하지 않을 수 있습니다.)
 *
 * meetNbrCondExt()함수 및 매개변수를 수정하여 사용하시면 됩니다.
 * 외곽 경로의 orientation은 탐색을 시작하는 면의 orientaiton과 같습니다.
 * 리턴값은 mesh가 가지는 vertices에 속한 MeshVertex의 복제본으로 구성된 Paths3D입니다.
 * 꼼꼼히 검토했으나 용도에 따른 개조로 인해 불안정한 부분이 있을 수 있습니다...ㅜㅜ
 */

Paths3D spreadingCheckExt(const Mesh& mesh, int chking_start){
    std::vector<bool> check(mesh.getFaces().size(), false);
    std::vector<bool> outer_check(mesh.getFaces().size(), false);
    Paths3D paths;
    int chking = -1;
    vector<const MeshFace*> to_check;
    to_check.push_back(& mesh.getFaces()[chking_start]);
    while(to_check.size()>0){
        vector<MeshFace*> next_to_check;
        for(int i=0; i<to_check.size(); i++){
            chking = to_check[i]->idx;
            if(check[chking]) continue;
            check[chking] = true;
            const MeshFace& mf = *to_check[i];
            int side;
            int outline_checked = false;
            for(side=0; side<3; side++){
                if(isEdgeBoundExt(mf, side)){
                    if(!outline_checked && !outer_check[chking]){
                        int path_head = getExtPathHead(mf, side);
                        Path3D path = buildOutlineExt(mesh, outer_check, chking, path_head);
                        if(path.size()==0) return {};//empty_paths;
                        paths.push_back(path);
                        outline_checked = true;
                    }
                }else{
                    vector<MeshFace*> neighbors = mf.neighboring_faces[side];
                    next_to_check.insert(next_to_check.end(), neighbors.begin(), neighbors.end());
                }
            }
        }
        to_check.clear();
        to_check.insert(to_check.end(), next_to_check.begin(), next_to_check.end());
    }
    return paths;
}

int getExtPathHead(const MeshFace& mf, int side){
    if(side==0 && isEdgeBoundExt(mf, 2)) {
        if(isEdgeBoundExt(mf, 1)) return -1;//all side of chking face is bound, face is alone
        else return mf.mesh_vertex[2];
    }
    return mf.mesh_vertex[side];
}

Path3D buildOutlineExt(const Mesh& mesh, std::vector<bool>& outer_check, int chking, int path_head){
    vector<int> path_by_idx;
    if(path_head==-1){
        path_by_idx = arrToVectExt(mesh.getFaces()[chking].mesh_vertex);
        return idxsToPathExt(mesh, path_by_idx);
    }
    bool outline_closed = false;
    int from = -1;
    int nxt_chk = -1;
    int path_tail = path_head;
    while(!outline_closed){
        const MeshFace& mf = mesh.getFaces()[chking];
        int outline_edge_cnt = 0;
        int tail_idx;//The index that path_tail has in the mf.mesh_vertex
        for(int i=0; i<3; i++){
            if(mf.mesh_vertex[i]==path_tail) tail_idx = i;
            if(isEdgeBoundExt(mf, i)) outline_edge_cnt++;
        }
        if(isEdgeBoundExt(mf, tail_idx)){
            if(outer_check[chking]) return {};//empty_path; //mesh error
            path_by_idx.push_back(path_tail);
            outer_check[chking] = true;
            if(outline_edge_cnt==1){
                path_tail = getNbrVtxExt(mf, tail_idx, 1);
                nxt_chk = mf.neighboring_faces[(tail_idx+1)%3][0]->idx;
            }else{//outline_edge_cnt==2
                path_by_idx.push_back(getNbrVtxExt(mf, tail_idx, 1));
                path_tail = getNbrVtxExt(mf, tail_idx, 2);
                nxt_chk = mf.neighboring_faces[(tail_idx+2)%3][0]->idx;
            }
            if(path_tail == path_head) outline_closed = true;
        }else{//if not isEdgeBoundExt(mf, tail_idx), the face doesn't share any bound edge with current outline
            //the face may share some bound edge with other outline, so we do not mark it checked
            if(mf.neighboring_faces[tail_idx][0]->idx==from) nxt_chk = mf.neighboring_faces[(tail_idx+2)%3][0]->idx;
            else nxt_chk = mf.neighboring_faces[tail_idx][0]->idx;
        }
        from = chking;
        chking = nxt_chk;
    }
    return idxsToPathExt(mesh, path_by_idx);
}

int getNbrVtxExt(const MeshFace& mf, int base, int xth){//getNeighborVtx
    if(xth>0) return mf.mesh_vertex[(base+xth+3)%3];
    else return -1;
}

bool isEdgeBoundExt(const MeshFace& mf, int side){
    if(mf.neighboring_faces[side].size() != 1) return true;
    //MeshFace& neighbor = *mf.neighboring_faces[side][0];
    //if(!meetNbrCondExt(neighbor)) return true;
    if(!isNbrOrientSameExt(mf, side)) return true;
    return false;
}

bool isNbrOrientSameExt(const MeshFace& mf, int side){
    MeshFace& nbr = *mf.neighboring_faces[side][0];
    if(getNbrVtxExt(nbr, searchVtxInFaceExt(nbr, mf.mesh_vertex[side]), 2) == getNbrVtxExt(mf, side, 1)) return true;
    return false;
}

bool meetNbrCondExt(const MeshFace& mf){//mf가 이웃의 조건을 만족하는가
    //코드와 매개변수를 추가하여 사용해주십시오.
    if(1) return true;
    return false;
}

Path3D idxsToPathExt(const Mesh& mesh, vector<int> path_by_idx){
    Path3D path;
    for(int idx : path_by_idx) path.push_back(mesh.getVertices()[idx]);
    return path;
}

int searchVtxInFaceExt(const MeshFace& mf, int vertexIdx){
    for(int i=0; i<3; i++){
        if(mf.mesh_vertex[i] == vertexIdx) return i;
    }
    return -1;
}

vector<int> arrToVectExt(const int arr[]){
    vector<int> vec (arr, arr + sizeof arr / sizeof arr[0]);
    return vec;
}
