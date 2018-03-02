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
                if(isEdgeBound(mf, side)){
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
    //**qDebug() << "buildOutline from" << chking;
    //*/
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
            if(isEdgeBound(mf, i)) outline_edge_cnt++;
        }
        if(isEdgeBound(mf, tail_idx)){
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

bool isEdgeBound(MeshFace* mf, int side){//bound edge: connected to face with opposit fn.z/not connected any face/multiple neighbor/opposit orientation
    if(mf->neighboring_faces[side].size() != 1) return true;
    MeshFace* neighbor = mf->neighboring_faces[side][0];
    if(neighbor->fn.z()<0) return true;
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
    int face_cnt = mesh->faces.size();
    bool check_done = false;
    int chking_start;
    bool face_checked[face_cnt] = {false}; //한 번 확인한 것은 체크리스트에서 제거되는 자료구조 도입 필요(법선 확인이 반복시행됨)
    vector<Paths> outline_sets;
    /****/qDebug() << "Get outline";
    while(!check_done){
        for(int i=0; i<face_cnt; i++){
            if(0<=mesh->faces[i].fn.z() && !face_checked[i]){
                chking_start=i;//new checking target obtained
                outline_sets.push_back(spreadingCheck(mesh, face_checked, chking_start));
                break;
            }else if(i==face_cnt-1) check_done = true;
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
    for(int i=0; i<paths.size(); i++){
        qDebug() << ""; //*/qDebug() << "path"; qDebug() << i;
        for(int j=0; j<paths[i].size(); j++){
            qDebug()<< paths[i][j].X << paths[i][j].Y;
        }
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
