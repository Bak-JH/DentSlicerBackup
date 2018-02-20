#include "arrange.h"
#include <QDebug>

Arrange::Arrange()
{

}
bool shit = false;

Paths spreadingCheck(Mesh* mesh, bool* check, int chking_start){
    /**/qDebug() << "SpreadingCheck started from" << chking_start;
    Paths paths;
    int chking = -1;
    vector<MeshFace*> to_check;
    to_check.push_back(& mesh->faces[chking_start]);
    while(to_check.size()>0){
        /**/qDebug() << "New spreadingCheck generation (" << to_check.size() << "faces)";
        vector<MeshFace*> next_to_check;
        for(uint8_t i=0; i<to_check.size(); i++){
            chking = to_check[i]->idx;
            //*Debug
            qDebug() << "to_check" << i << "(Face" << chking << ")";
            for(int side=0; side<3; side++){
                if (isEdgeBound(to_check[i], side)) qDebug() << " - unchecked bound";
                else qDebug() << " -" << to_check[i]->neighboring_faces[side].size()
                               << "neighbors (first:" << to_check[i]->neighboring_faces[side][0]->idx << ")";
            }//*/
            if(check[chking]) continue;
            check[chking] = true;
            MeshFace* mf = to_check[i];
            int side;
            int outline_checked = false;
            for(side=0; side<3; side++){
                if (isEdgeBound(mf, side)){
                    if(!outline_checked){
                        int path_head = getPathHead(mf, side);
                        Path path = buildOutline(mesh, check, chking, path_head);
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

int getPathHead(MeshFace* mf, int side){
    if(side==0 && isEdgeBound(mf, 2)) {
        if(isEdgeBound(mf, 1)) return -1;//all side of chking face is bound, face is alone
        else return mf->mesh_vertex[2];
    }
    return mf->mesh_vertex[side];
}

Path buildOutline(Mesh* mesh, bool* check, int chking, int path_head){
    //*Debug
    qDebug() << "buildOutline from" << chking;
    //*/
    vector<int> path_by_idx;
    if(path_head==-1){//혼자있는 면의 경우 오리엔테이션 확인 방법이 마련되어있지 않음
        check[chking] = true;
        path_by_idx = arrToVect(mesh->faces[chking].mesh_vertex);
        qDebug() << "buildOutline done";
        return idxsToPath(mesh, path_by_idx);
    }
    bool outline_closed = false;
    int from = -1;
    int nxt_chk = -1;
    int path_tail = path_head;
    while(!outline_closed){
        qDebug() << chking;
        MeshFace* mf = & mesh->faces[chking];
        int outline_edge_cnt = 0;
        int tail_idx;//The index that path_tail has in the mf->mesh_vertex
        int orientation = 0;//path_head를 가지는 face와의 상대 orientation임(fn과 orientation 일치성 확보되면 불필요)
        for (int i=0; i<3; i++){
            if(mf->mesh_vertex[i]==path_tail) tail_idx = i;
            if(isEdgeBound(mf, i)) outline_edge_cnt++;
        }
        if(isEdgeBound(mf, tail_idx)) orientation = 1;
        else if(isEdgeBound(mf, (tail_idx+2)%3)) orientation = -1;
        if(orientation!=0){
            path_by_idx.push_back(path_tail);
            check[chking] = true;

            if(outline_edge_cnt==1){
                path_tail = getVetex(mf, tail_idx, 1, orientation);
                nxt_chk = mf->neighboring_faces[(tail_idx+1)%3][0]->idx;
            }else{//outline_edge_cnt==2
                path_by_idx.push_back(getVetex(mf, tail_idx, 1, orientation));
                path_tail = getVetex(mf, tail_idx, 2, orientation);
                nxt_chk = mf->neighboring_faces[(tail_idx+orientation+1)%3][0]->idx;
            }

            if(path_tail == path_head) outline_closed = true;
        }else{//if orientation is 0, the face doesn't share any bound edge with current outline
            //the face may share some bound edge with other outline, so we do not mark it checked
            if(mf->neighboring_faces[tail_idx][0]->idx==from) nxt_chk = mf->neighboring_faces[(tail_idx+2)%3][0]->idx;
            else nxt_chk = mf->neighboring_faces[tail_idx][0]->idx;
        }
        from = chking;
        chking = nxt_chk;
    }
    qDebug() << "buildOutline done";
    return idxsToPath(mesh, path_by_idx);
}

bool isEdgeBound(MeshFace* mf, int side){//bound edge: connected to face with opposit fn.z or not connected any face
    bool is_edge_bound = true;
    for(uint8_t i=0; i<mf->neighboring_faces[side].size(); i++){
        MeshFace* neighbor = mf->neighboring_faces[side][i];
        if(neighbor->fn.z()>=0){
            is_edge_bound = false;
            break;
        }
    }
    return is_edge_bound;
}

vector<int> arrToVect(int arr[]){
    vector<int> vec (arr, arr + sizeof arr / sizeof arr[0]);
    return vec;
}

int getVetex(MeshFace* mf, int base, int xth, int orientation){
    if(xth>0) return mf->mesh_vertex[(base+xth*(orientation+3))%3];
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

void debugPath(Paths paths){
    qDebug() << "===============";
    for (uint8_t i=0; i<paths.size(); i++){
        qDebug() << ""; //*/qDebug() << "path"; qDebug() << i;
        for (uint8_t j=0; j<paths[i].size(); j++){
            qDebug()<< paths[i][j].X << paths[i][j].Y;
        }
    }
    qDebug() << "===============";
}
