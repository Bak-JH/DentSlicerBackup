#include "arrange.h"
#include <QDebug>

Arrange::Arrange()
{

}
bool shit = false;

Paths spreadingCheck(Mesh* mesh, bool* check, int chking_start){
    //qDebug() << "SpreadingCheck started from" << chking_start;
    Paths paths;
    int chking = -1;
    bool outer_check[mesh->faces.size()] = {false};
    vector<MeshFace*> to_check;
    to_check.push_back(& mesh->faces[chking_start]);
    while(to_check.size()>0){
        //qDebug() << "New spreadingCheck generation (" << to_check.size() << "faces)";
        vector<MeshFace*> next_to_check;
        for(int i=0; i<to_check.size(); i++){
            chking = to_check[i]->idx;
            /*Debug
            qDebug() << "to_check" << i << "(Face" << chking << ")";
            for(int side=0; side<3; side++){
                if (isEdgeBound(to_check[i], side)){
                    if(!outer_check[chking]) qDebug() << " - unchecked bound";
                    else qDebug() << " - checked bound";
                }else qDebug() << " -" << to_check[i]->neighboring_faces[side].size()
                               << "neighbors (first:" << to_check[i]->neighboring_faces[side][0]->idx << ")";
            }//*/
            if(check[chking]) continue;
            check[chking] = true;
            MeshFace* mf = to_check[i];
            for(int side=0; side<3; side++){
                if (isEdgeBound(mf, side)){
                    if(!outer_check[chking]){
                        int path_head = getPathHead(mf, side);
                        Path path = buildOutline(mesh, check, outer_check, chking, path_head);
                        paths.push_back(path);
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

int getPathHead(MeshFace* mf, int side){
    int path_head = mf->mesh_vertex[side];//set end point of the bound edges as path_head, working with following if statement
    if(side==0 && isEdgeBound(mf, 2)) {
        if(isEdgeBound(mf, 1)){//all side of chking face is bound, face is alone
            path_head = -1;
        }
        else path_head = mf->mesh_vertex[2];//reset path_head
    }
    return path_head;
}

Path buildOutline(Mesh* mesh, bool* check, bool* outer_check, int chking, int path_head){
    /*Debug
    qDebug() << "buildOutline from" << chking;
    //*/
    if(path_head==-1){
        /*Debug
        qDebug() << "!!!" << chking
                 << cfg->resolution * mesh->vertices[mesh->faces[chking].mesh_vertex[0]].position.x()
                 << cfg->resolution * mesh->vertices[mesh->faces[chking].mesh_vertex[0]].position.y();
        //*/
        outer_check[chking] = true;
        check[chking] = true;
        MeshFace* mf = & mesh->faces[chking];
        Path path;
        for(int j = 0; j<3; j++){
            int v_idx = mf->mesh_vertex[j];
            QVector3D vertex = mesh->vertices[v_idx].position;
            mesh->Mesh::addPoint(vertex.x(), vertex.y(), &path);
        }
        return path;
    }
    vector<int> path_by_idx;
    bool outline_closed = false;
    int outer_chking = chking;
    int from = -1;
    int path_tail = path_head;
    while(!outline_closed){
        MeshFace* mf = & mesh->faces[outer_chking];
        int outline_edge_cnt = 0;
        int path_tail_idx;
        int orientation = 0;//path_head를 가지는 face와의 상대 orientation임(fn과 orientation 일치성 확보되면 불필요)
        for (int i=0; i<3; i++){
            if(mf->mesh_vertex[i]==path_tail) path_tail_idx = i;
            if(isEdgeBound(mf, i)) outline_edge_cnt++;
        }
        if(isEdgeBound(mf, path_tail_idx)){
            orientation = 1;
        }else if(isEdgeBound(mf, (path_tail_idx+2)%3)){
            orientation = -1;
        }
        if(orientation!=0){
            path_by_idx.push_back(path_tail);
            outer_check[outer_chking] = true;
            check[outer_chking] = true;
            from = outer_chking;
            if(outline_edge_cnt==1){
                path_tail = mf->mesh_vertex[(path_tail_idx+orientation+3)%3];
                outer_chking = mf->neighboring_faces[(path_tail_idx+1)%3][0]->idx;
            }else{
                path_by_idx.push_back(mf->mesh_vertex[(path_tail_idx+orientation)%3]);
                path_tail = mf->mesh_vertex[(path_tail_idx+2*orientation+3)%3];
                outer_chking = mf->neighboring_faces[(path_tail_idx+orientation+1)%3][0]->idx;
            }
        }else{//if orientation is 0, the face doesn't share any bound edge with current outline
            check[outer_chking] = true; //the face may share some bound edge with other outline, so we do not midify outer_check
            if(mf->neighboring_faces[path_tail_idx][0]->idx==from){
                from = outer_chking;
                outer_chking = mf->neighboring_faces[(path_tail_idx+2)%3][0]->idx;
            }else{
                from = outer_chking;
                outer_chking = mf->neighboring_faces[path_tail_idx][0]->idx;
            }
        }
        if(path_tail == path_head) outline_closed = true;
    }
    return idxsToPath(mesh, path_by_idx);
}

bool isEdgeBound(MeshFace* mf, int side){//bound edge: connected to face with opposit fn.z or not connected any face
    bool is_edge_bound = true;
    for(int i=0; i<mf->neighboring_faces[side].size(); i++){
        MeshFace* neighbor = mf->neighboring_faces[side][i];
        if(neighbor->fn.z()>=0){
            is_edge_bound = false;
            break;
        }
    }
    return is_edge_bound;
}

Path idxsToPath(Mesh* mesh, vector<int> path_by_idx){
    Path path;
    for(int id : path_by_idx){
        QVector3D vertex = mesh->vertices[id].position;
        //*mesh->Mesh::addPoint(1*, 1*vertex.y(), &path);//resolution related
        IntPoint ip;
        ip.X = cfg->resolution*vertex.x();
        ip.Y = cfg->resolution*vertex.y();
        path.push_back(ip);
        //qDebug() << "path build" << id << ip.X << ip.Y;
    }
    return path;
}

Paths project(Mesh* mesh){
    int face_number = mesh->faces.size();
    bool check_done = false;
    int chking_start;
    bool face_checked[face_number] = {false}; //한 번 확인한 것은 체크리스트에서 제거되는 자료구조 도입 필요(법선 확인이 반복시행됨)
    vector<Paths> outline_sets;
    /****/qDebug() << "Get outline";
    while(!check_done){
        for(int i=0; i<face_number; i++){
            if (0<=mesh->faces[i].fn.z() && !face_checked[i]){
                chking_start=i;//new checking target obtained
                if(outline_sets.size()==27) shit=true;
                if(outline_sets.size()==28) shit=false;
                outline_sets.push_back(spreadingCheck(mesh, face_checked, chking_start));
                break;
            }else if(i==face_number-1) check_done = true;
        }
    }
    /****/qDebug() << "Total outline_set : " << outline_sets.size() << "->" << clipOutlines(outline_sets).size();
    return clipOutlines(outline_sets);
}

Paths clipOutlines(vector<Paths> outline_sets){
    Paths projection;
    //Paths tmp_clip_result;
    Clipper c;
    for(int i=0; i<(outline_sets.size()); i++){
        //if(i!=28)continue;
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

Mesh pathsToMesh(Paths paths){
    paths = resize(paths);
    Mesh mesh;
    float thickness = 0.1;
    for (int i=0; i<paths.size(); i++){
        /****/qDebug() << "PathsToMesh Path" << i;
        for (int j=0; j<paths[i].size(); j++){
            /****/qDebug() << "PathsToMesh edge" << j;
            float x1 = paths[i][j].X/1000, y1 = paths[i][j].Y/1000;
            float x2 = paths[i][(j+1)%paths[i].size()].X/1000, y2 = paths[i][(j+1)%paths[i].size()].Y/1000;
            float vec_x = thickness*(x2-x1)/sqrt(pow(x2-x1,2)+pow(y2-y1,2));
            float vec_y = thickness*(y2-y1)/sqrt(pow(x2-x1,2)+pow(y2-y1,2));
            /****/qDebug() << x1 << y1 <<x2 << y2 << vec_x << vec_y;
            QVector3D v0 = QVector3D(x1, y1, 0);
            QVector3D v1 = QVector3D(x2, y2, 0);
            QVector3D v2 = QVector3D(x1+vec_y, y1-vec_x, 0);
            QVector3D v3 = QVector3D(x2+vec_y, y2-vec_x, 0);
            mesh.addFace(v2, v1, v0);
            mesh.addFace(v1, v2, v3);
        }
    }
    mesh.connectFaces();
    cfg->origin = QVector3D((mesh.x_min+mesh.x_max)/2, (mesh.y_min+mesh.y_max)/2, (mesh.z_min+mesh.z_max)/2);
    /****/qDebug() << "PathsToMesh done";
    return mesh;
}

Paths resize(Paths paths){
    float max_x = paths[0][0].X, min_x = paths[0][0].X, max_y = paths[0][0].Y, min_y = paths[0][0].Y;
    for (int i=0; i<paths.size(); i++){
        for (int j=0; j<paths[i].size(); j++){
            if(paths[i][j].X>max_x) max_x=paths[i][j].X;
            else if(paths[i][j].X<min_x) min_x=paths[i][j].X;
            if(paths[i][j].Y>max_y) max_y=paths[i][j].Y;
            else if(paths[i][j].Y<min_y) min_y=paths[i][j].Y;
        }
    }

    qDebug() << "max_x" << max_x << "min_x" << min_x;
    qDebug() << "max_y" << max_y << "min_y" << min_y;

    float center_x = 0, center_y = 0;
    float max_length = 60*1000;
    float move_x, move_y, multiplying_factor;
    move_x = center_x - (max_x + min_x)/2;
    move_y = center_y - (max_y + min_y)/2;
    if (max_x - min_x > max_y - min_y) multiplying_factor = max_length / (max_x - min_x);
    else multiplying_factor = max_length / (max_y - min_y);

    qDebug() << "move_x" << move_x << "move_y" << move_y << "multiplying_factor" << multiplying_factor;

    for (int i=0; i<paths.size(); i++){
        for (int j=0; j<paths[i].size(); j++){
            paths[i][j].X = (paths[i][j].X + move_x) * multiplying_factor;
            paths[i][j].Y = (paths[i][j].Y + move_y) * multiplying_factor;
        }
    }
    return paths;
}

void debugPath(Paths paths){
    qDebug() << "===============";
    for (int i=0; i<paths.size(); i++){
        qDebug() << ""; //*/qDebug() << "path"; qDebug() << i;
        for (int j=0; j<paths[i].size(); j++){
            qDebug()<< paths[i][j].X << paths[i][j].Y;
        }
    }
    qDebug() << "===============";
}
