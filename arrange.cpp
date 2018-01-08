#include "arrange.h"
#include <QDebug>

Arrange::Arrange()
{

}
int cnt = 0;
Paths SpreadingCheck(Mesh* mesh, bool* check, bool* outer_check, int chking){
    cnt++;
    //if(chking==10691){qDebug() << "1*-*-*-*-*";}
    /****/qDebug() << "Check face" << chking <<cnt;
    Paths paths;
    MeshFace mf = mesh->faces[chking];
    check[chking] =true;
    for (int i=0; i<3; i++){
        int neighbor = mf.connected_face_idx[i];
        if (IsEdgeBound(mesh, mf, i)){//bound edge: connected to face with opposit fn.z or not connected any face
            if(!outer_check[chking]){
                qDebug() << chking << "-" << i;//setting to trak outline
                int path_head = mf.mesh_vertex[i];//set end point of the bound edges as path_head, working with following if statement
                if(i==0 && IsEdgeBound(mesh, mf, 2)) {
                    if(IsEdgeBound(mesh, mf, 1)){//all side of chking face is bound, face is alone
                        Path path;
                        for(int j = 0; j<3; j++){
                            int v_idx = mf.mesh_vertex[j];
                            QVector3D vertex = mesh->vertices[v_idx].position;
                            mesh->Mesh::addPoint(vertex.x(), vertex.y(), &path);
                        }
                        paths.push_back(path);//occurring error: face which has neighbor comes in to this part
                        break;
                    }
                    else path_head = mf.mesh_vertex[2];//reset path_head
                }
                paths.push_back(GetOutline(mesh, check, outer_check, chking, path_head));//GetOutline and push result
            }
        }else if(!check[neighbor] && !outer_check[neighbor]){
            qDebug() << chking << "-" << i << "-" << neighbor;
            if(neighbor==10691){qDebug() << "0*-*-*-*-*";}
            Paths paths1 = SpreadingCheck(mesh, check, outer_check, neighbor);
            //if(neighbor==10691){qDebug() << "2*-*-*-*-*";}
            for(int j=0; j<paths1.size(); j++){
                paths.push_back(paths1[j]);
            }
        }
    }
    return paths;
}

Path GetOutline(Mesh* mesh, bool* check, bool* outer_check, int chking, int path_head){
    /****/qDebug() << "GetOutline from" << chking;
    std::vector<int> path_by_idx;
    bool outline_closed = false;
    int outer_chking = chking;
    int from = -1;
    int path_tail = path_head;
    while(!outline_closed){
        MeshFace mf = mesh->faces[outer_chking];
        int outline_edge_cnt = 0;
        int path_tail_idx;
        int orientation = 0;
        for (int i=0; i<3; i++){
            if(mf.mesh_vertex[i]==path_tail) path_tail_idx = i;
            if(IsEdgeBound(mesh, mf, i)) outline_edge_cnt++;
        }
        if(IsEdgeBound(mesh, mf, path_tail_idx)){
            orientation = 1;
        }else if(IsEdgeBound(mesh, mf, (path_tail_idx+2)%3)){
            orientation = -1;
        }
        /****/qDebug() << "tail:" << path_tail
                       << "outer_chking" << outer_chking << mf.fn.z() << from << path_tail_idx
                       << mf.connected_face_idx[0] << IsEdgeBound(mesh, mf, 0)
                       << mf.connected_face_idx[1] << IsEdgeBound(mesh, mf, 1)
                       << mf.connected_face_idx[2] << IsEdgeBound(mesh, mf, 2)
                       << orientation << outline_edge_cnt;
        if(outer_check[outer_chking]) qDebug() << "-reduplication-";//occurring error: face sequence shouldn't go back except few cases but sometime infinite loop occurs
        if(orientation!=0){
            path_by_idx.push_back(path_tail);
            outer_check[outer_chking] = true;
            check[outer_chking] = true;
            if(outline_edge_cnt==1){
                path_tail = mf.mesh_vertex[(path_tail_idx+orientation+3)%3];
                from = outer_chking;
                outer_chking = mf.connected_face_idx[(path_tail_idx+1)%3];
            }else{
                path_by_idx.push_back(mf.mesh_vertex[(path_tail_idx+orientation)%3]);
                path_tail = mf.mesh_vertex[(path_tail_idx+2*orientation+3)%3];
                from = outer_chking;
                outer_chking = mf.connected_face_idx[(path_tail_idx+orientation+1)%3];
            }
        }else{//if orientation is 0, the face doesn't share any bound edge with current outline
            check[outer_chking] = true; //the face may share any bound edge with other outline(do not midify outer_check)
            if(mf.connected_face_idx[path_tail_idx]==from){
                from = outer_chking;
                outer_chking = mf.connected_face_idx[(path_tail_idx+2)%3];
            }else{
                from = outer_chking;
                outer_chking = mf.connected_face_idx[path_tail_idx];
            }
        }
        if(path_tail == path_head) outline_closed = true;
    }
    Path path;
    for(int i : path_by_idx){
        QVector3D vertex = mesh->vertices[i].position;
        //*mesh->Mesh::addPoint(1*, 1*vertex.y(), &path);//resolution related
        IntPoint ip;
        ip.X = 1000000000*vertex.x();
        ip.Y = 1000000000*vertex.y();
        path.push_back(ip);
        qDebug() << "path build" << i << ip.X << ip.Y;
    }
    return path;
}

bool IsEdgeBound(Mesh* mesh, MeshFace mf, int neighbor_idx){
    int neighbor = mf.connected_face_idx[neighbor_idx];
    return (neighbor == -1 || 0>mesh->faces[neighbor].fn.z());
}

Paths Projection(Mesh* mesh){
    int face_number = mesh->faces.size();
    bool check_done = false;
    int checking_face;
    bool face_checked[face_number] = {false};
    bool outer_face_checked[face_number] = {false};
    std::vector<std::pair<int, int>> outline_edges;
    std::vector<Paths> outline_sets;
    while(!check_done){
        /****/qDebug() << "Get outline" << outline_sets.size();
        for(int i=0; i<face_number; i++){
            if (0<=mesh->faces[i].fn.z() && !face_checked[i]){
                checking_face=i;
                break;
            }else if(i==face_number-1){
                check_done = true;
                break;
            }
        }
        if(!check_done){
            Paths outline_set;
            outline_set = SpreadingCheck(mesh, face_checked, outer_face_checked, checking_face);
            outline_sets.push_back(outline_set);
        }
    }
    /****/qDebug() << "Total outline_set : " << outline_sets.size();
    /****************************/
    Paths projection;
    Clipper c;
    for(int i=0; i<outline_sets.size(); i++){
        //*qDebug() << "Clipping" << i;
        c.Clear();
        c.AddPaths(projection, ptSubject, true);
        c.AddPaths(outline_sets[i], ptClip, true);
        //review if simplyfy needed
        c.Execute(ctUnion, projection, pftNonZero, pftNonZero);
    }
    //*qDebug() << "Paths in projection  : " << projection.size();
    return projection;
    //*****************************/

    /***************************test code*
    int set = 2;
    qDebug() << "Projection result" << set;
    //qDebug() << "number of point" << outline_sets[set].size();
    //for (int i=0; i<outline_sets[set].size(); i++)qDebug() << "(" << outline_sets[set][i].X << "," << outline_sets[set][i].Y << ")";
    //return outline_sets[set];
    Paths s;
    //SimplifyPolygons(outline_sets[set], s, pftNonZero);
    //return s;
    return outline_sets[set];
    /*****************************/
}

Mesh PathsToMesh(Paths paths){
    paths = Resize(paths);
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

Paths Resize(Paths paths){
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

void DebugPath(Paths paths){
    qDebug() << "===============";
    for (int i=0; i<paths.size(); i++){
        qDebug() << ""; //*/qDebug() << "path"; qDebug() << i;
        for (int j=0; j<paths[i].size(); j++){
            qDebug()<< paths[i][j].X << paths[i][j].Y;
        }
    }
    qDebug() << "===============";
}
