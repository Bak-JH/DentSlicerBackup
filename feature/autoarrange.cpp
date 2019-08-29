#include "autoarrange.h"
#include <cstdlib>
#include <ctime>
#define ANGLE_UNIT 270
#define STAGE_WIDTH 95000
#define STAGE_HEIGHT 75000
#define OFFSET 2000
#include <qmlmanager.h>
#include <exception>
#include <unordered_map>
#include "DentEngine/src/mesh.h"
#include "DentEngine/src/configuration.h"
#include "DentEngine/src/slicer.h"
using namespace ClipperLib;
using namespace Hix::Engine3D;
autoarrange::autoarrange()
{
}

Paths autoarrange::getMeshRecArea(const Mesh& mesh){//getting rectangle area of mesh in XY Plane, OFFSET included
    Paths outline;
    Path vertices;//all vertices in mesh
    Path vertices45rot;//45 degree check
    for(const auto& vertex : mesh.getVertices()){
        QVector3D v_pos = vertex.position;
		ClipperLib::addPoint(v_pos.x(), v_pos.y(), &vertices);
		ClipperLib::addPoint(round(v_pos.x()*cosf(M_PI/4) - v_pos.y()*sinf(M_PI/4)), round(v_pos.x()*sinf(M_PI/4) + v_pos.y()*cosf(M_PI/4)), &vertices45rot);//45 degree check
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
#ifdef _DEBUG_AUTO_ARRANGE
    qDebug() << "got MeshRecArea";
    debugPaths(outline);
#endif
    return outline;
}

Paths autoarrange::getMeshConvexHull(const Mesh& mesh){//getting convex hull area of mesh in XY Plane
    Paths outline;
    Path vertices;//all vertices in mesh
    for(const auto& vertex : mesh.getVertices()){
        QVector3D v_pos = vertex.position;
		ClipperLib::addPoint(v_pos.x(), v_pos.y(), &vertices);
    }
    outline.push_back(getConvexHull(&vertices));
    /**/qDebug() << "got MeshConvexHull";
    return outline;
}

Paths autoarrange::spreadingCheck(const Mesh* mesh, std::map<const MeshFace *, bool>& check, const MeshFace * chking_start, bool is_chking_pos){
    /**/qDebug() << "SpreadingCheck started from" << chking_start;
    Paths paths;
	const MeshFace * chking;
    std::vector<const MeshFace *> toCheck;
	toCheck.push_back(chking_start);
    while(toCheck.size()>0){
        std::vector<const MeshFace *> nextIndexToCheck;
        for(int i=0; i< toCheck.size(); i++){
			chking = toCheck[i];
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
            const MeshFace * mf = toCheck[i];
            bool outline_checked = false;
			auto edgeCirc = mf->edgeCirculator();
			//wtf
			auto first = mf->edge;
			auto second = first->next;
			auto last = first->next->next;
            for(size_t j =0; j<3; ++j)
			{
                if(isEdgeBound(mesh, mf, edgeCirc.toPtr(), is_chking_pos)){
                    if(!outline_checked){
						const MeshVertex* path_head = nullptr;
						if (j == 0 && isEdgeBound(mesh, mf, last.operator->(), is_chking_pos)) {
							if (!isEdgeBound(mesh, mf, second.operator->(), is_chking_pos))
                                path_head =mf->meshVertices()[2].operator->();
						}
						else
						{
                            path_head = mf->meshVertices()[j].operator->();
						}
                        Path path = buildOutline(mesh, check, chking, path_head, is_chking_pos);
                        if(path.size()==0) return {};
                        paths.push_back(path);
                        outline_checked = true;
                    }
                }else{//법선 방향 조건이 만족되는 이웃만 to_check에 추가하는 것이 맞을지 검토
                    auto neighbors = edgeCirc->nonOwningFaces();
					for (auto nItr : neighbors)
					{
						nextIndexToCheck.push_back(nItr.operator->());
					}
                }
				++edgeCirc;
            }
        }
		toCheck.clear();
		toCheck.insert(toCheck.end(), nextIndexToCheck.begin(), nextIndexToCheck.end());
    }
    return paths;
}
Path autoarrange::buildOutline(const Mesh* mesh, std::map<const MeshFace *, bool>& check, const MeshFace * chking,
	const MeshVertex* path_head, bool is_chking_pos){
    //**qDebug() << "buildOutline from" << chking;
    std::vector<const MeshVertex* > path_by_idx;
	auto meshVertices = chking->meshVertices();

    if(path_head== nullptr){//혼자있는 면의 경우 오리엔테이션 확인 방법이 마련되어있지 않음
        check[chking] = true;
		for (auto mvItr : meshVertices)
		{
			path_by_idx.push_back(mvItr.operator->());
		}
        //**qDebug() << "buildOutline done";
        return idxsToPath(mesh, path_by_idx);
    }
    bool outline_closed = false;
	const MeshFace * from;
	const MeshFace * nxt_chk;
    auto path_tail = path_head;
    while(!outline_closed){
        if(check[chking]){
            qDebug() << "mesh error";
            return {};
        }
        //**qDebug() << "chking" << chking;
        int outline_edge_cnt = 0;
		auto edgeCirc = chking->edgeCirculator();
		HalfEdgeConstItr tailEdge;
		for (auto each : path_tail->arrivingEdges)
		{
            if (each->owningFace.operator->() == chking)
			{
				tailEdge = each;
			}
		}
        for(int i=0; i<3; i++){
            auto current = meshVertices[i].operator->();
			auto currEdge = edgeCirc.toItr();

            if(isEdgeBound(mesh, chking, currEdge.operator->(), is_chking_pos)) outline_edge_cnt++;
			++edgeCirc;
        }
        auto tailOwningFaces = tailEdge->next->nonOwningFaces();
        if(isEdgeBound(mesh, chking, tailEdge.operator->(), is_chking_pos)){
            path_by_idx.push_back(path_tail);
            check[chking] = true;
            if(outline_edge_cnt==1){
                path_tail = getNbrVtx(chking, tailEdge.operator->(), 1);
                const MeshFace *  face = tailOwningFaces[0].operator->();
                nxt_chk = face;
            }else{//outline_edge_cnt==2
                path_by_idx.push_back(getNbrVtx(chking, tailEdge.operator->(), 1));
                path_tail = getNbrVtx(chking, tailEdge.operator->(), 2);
                const MeshFace * face = tailOwningFaces[0].operator->();
                nxt_chk = face;
            }
            if(path_tail == path_head) outline_closed = true;
        }else{//if not isEdgeBound(chking, tail_idx), the face doesn't share any bound edge with current outline
            //the face may share some bound edge with other outline, so we do not mark it checked
            const MeshFace * faceA = tailOwningFaces[0].operator->();
            auto prevTailedgeNFaces = tailEdge->prev->nonOwningFaces();
            const MeshFace * faceB = prevTailedgeNFaces[0].operator->();
            if(faceA == from) nxt_chk = faceB;
            else nxt_chk = faceA;
        }
        from = chking;
        chking = nxt_chk;
    }
    //**qDebug() << "buildOutline done";
    return idxsToPath(mesh, path_by_idx);
}

bool autoarrange::isEdgeBound(const Mesh* mesh, const MeshFace * mf, const HalfEdge* edge, bool is_chking_pos){
    //condition of bound edge:
    //1. connected to face with opposit fn.z
    //2. not connected any face
    //3. multiple neighbor
    //4. opposit orientation
	auto sideNBRFaces = edge->nonOwningFaces();
    if(sideNBRFaces.size() != 1) return true;
    const MeshFace * neighbor = sideNBRFaces[0].operator->();
    if(!checkFNZ(*neighbor, is_chking_pos)) return true;
    if(!isNbrOrientSame(mesh, mf, edge)) return true;
    return false;
}

bool autoarrange::isNbrOrientSame(const Mesh* mesh, const MeshFace * mf, const HalfEdge* edge){

	auto leavingVtx = edge->from;
	for (auto leavingEdge : leavingVtx->leavingEdges)
	{
		//if an edge traveling in same direction, vtxs, and different faces
		if (leavingEdge.operator->() != edge && leavingEdge->to == edge->to)
			return true;
	}
    return false;
}




const MeshVertex* autoarrange::getNbrVtx(const MeshFace * mf, const HalfEdge* base, size_t offset){//getNeighborVtx

	offset = offset % 3;
	for (size_t i = 0; i < offset; ++i)
	{
		base = base->next.operator->();
	}
    return base->from.operator->();
	
}

Path autoarrange::idxsToPath(const Mesh* mesh, std::vector<const MeshVertex* > path_by_idx){
    Path path;
    for(const MeshVertex* vtx : path_by_idx){
        QVector3D vertex = vtx->position;
		ClipperLib::addPoint(vertex.x(), vertex.y(), &path);
    }
    return path;
}

Paths autoarrange::project(const Mesh* mesh){
    std::vector<Paths> outline_sets;
    bool is_chking_pos = false;
    bool mesh_error = false;
	std::map<const MeshFace *, bool> face_checked;

	size_t faces_size = mesh->getFaces().size();
	for (int i = 0; i < 2; i++)
	{
		is_chking_pos = !is_chking_pos;
		bool check_done = false;
		face_checked.clear();//한 번 확인한 것은 체크리스트에서 제거되는 자료구조 도입 필요(법선 확인이 반복시행됨)
		/****/qDebug() << "Get outline(is_chking_pos:" << is_chking_pos << ")";
		while (!check_done && !mesh_error) {
			for (const auto& face : mesh->getFaces())
			{
				if (checkFNZ(face, is_chking_pos) && !face_checked[&face])
				{
					outline_sets.push_back(spreadingCheck(mesh, face_checked, &face, is_chking_pos));
					if (outline_sets[outline_sets.size() - 1].size() == 0) 
						mesh_error = true;
					break;
				}
			}
			check_done = true;

		}
		if (mesh_error) break;
	}
    if(mesh_error){
        return getMeshConvexHull(*mesh);
    }
    else{
        /****/qDebug() << "Total outline_set : " << outline_sets.size() << "->" << clipOutlines(outline_sets).size();
        return clipOutlines(outline_sets);
    }
}

Paths autoarrange::clipOutlines(std::vector<Paths> outline_sets){
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

bool autoarrange::checkFNZ(const MeshFace& face, bool is_chking_pos){
    if(is_chking_pos){
        if(face.fn.z()>=0) return true;
        else return false;
    }else{
        if(face.fn.z()<=0) return true;
        else return false;
    }
}


const MeshVertex* autoarrange::findVertexWithIntpoint(IntPoint p, const Mesh* mesh){
    return findVertexWithIntXY(p.X, p.Y, mesh);
}

const MeshVertex* autoarrange::findVertexWithIntXY(size_t x, size_t y, const Mesh* mesh){
	for (auto& each : mesh->getVertices())
	{
        int x_int = round(each.position.x()*ClipperLib::INT_PT_RESOLUTION);
        int y_int = round(each.position.y()*ClipperLib::INT_PT_RESOLUTION);
        if(x_int==x && y_int==y) return &each;
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

//typedef std::pair<IntPoint, float> XYArrangement;

std::vector<XYArrangement> autoarrange::simpArngMeshes(std::vector<const Mesh*>& meshes){
    std::vector<Paths> outlines;
    /**/qDebug() << "Arrange start";
    for(int idx=0; idx<meshes.size(); idx++){
        outlines.push_back(getMeshConvexHull(*meshes[idx]));
        RDPSimpPaths(&outlines[idx]);
        offsetPath(&outlines[idx]);
    }
    /**/qDebug() << "Projection done @arngMeshes";
    return arng2D(outlines);
}

typedef struct LLIPoint {
    long long int x, y;
} LLIPoint;

#define max2(x,y) ((x)>(y)?(x):(y))
#define min2(x,y) ((x)<(y)?(x):(y))

bool is_valid_range(int y, int x, int k){
    return 0<=y && y<=k && 0<=x && x<=k;
}

bool make_outline(int y, int x, int startY, int startX, int depth, int direction,
	std::unordered_map<size_t, std::unordered_map<size_t, bool>>*block,
	std::unordered_map<size_t, std::unordered_map<size_t, bool>>*outline, std::vector<std::pair<int, int> > *vertice, int k) {
    int dy[8] = {0, -1, -1, -1, 0, 1, 1, 1}, dx[8] = {1, 1, 0, -1, -1, -1, 0, 1};
    (*outline)[y][x] = true;
    if (depth != 0 && y == startY && x == startX) {
        return true;
    }
    for (int kk=direction+2; kk>=direction-2; kk--){
        if (is_valid_range(y+dy[(kk+8)%8], x+dx[(kk+8)%8], k) && (*block)[y+dy[(kk+8)%8]][x+dx[(kk+8)%8]]) {
            if (direction != kk)
                vertice->push_back(std::make_pair(y, x));
            if (make_outline(y+dy[(kk+8)%8], x+dx[(kk+8)%8], startY, startX, depth+1, (kk+8)%8, block, outline, vertice, k))
                return true;
            if (direction != kk)
                vertice->pop_back();
        }
    }
    return false;
}
std::vector<LLIPoint> getPaddedProj(const Mesh *mesh, int floatPointPadding) {
    long long int INF=10000000000000;
    int k=100, padding;
    int bedSize = 120;
    std::vector<std::pair<LLIPoint, LLIPoint> > edges;
    std::vector<std::pair<int, int> > vertice;
    std::vector<LLIPoint> result;

    qDebug() << "make edges";
    for (auto& mf: mesh->getFaces()){
		auto meshVertices = mf.meshVertices();
        std::vector<LLIPoint> vs;
        for (int i = 0; i < 3; i++) {
            LLIPoint temp;
            QVector3D vertex = meshVertices[i]->position;
            temp.x = int(vertex.x() * floatPointPadding); temp.y = int(vertex.y() * floatPointPadding);
            vs.push_back(temp);
        }
        for (int i = 0, j = 2; i < 3; j=i++) {
            if (vs[i].x < vs[j].x)
                edges.push_back(std::make_pair(vs[i], vs[j]));
            else if (vs[i].x == vs[j].x && vs[i].y < vs[j].y)
                edges.push_back(std::make_pair(vs[i], vs[j]));
        }
    }



    qDebug() << "fill block";
    int edgeCnt = edges.size();
    long long int minx = INF, maxx = -INF, miny = INF, maxy = -INF, blockX, blockY, ttt;
    for (int i = 0; i < edgeCnt; i++){
        minx = min2(minx, min2(edges[i].first.x, edges[i].second.x));
        miny = min2(miny, min2(edges[i].first.y, edges[i].second.y));
        maxx = max2(maxx, max2(edges[i].first.x, edges[i].second.x));
        maxy = max2(maxy, max2(edges[i].first.y, edges[i].second.y));
    }
    ttt = (maxx-minx)/10; minx-=ttt; maxx+=ttt;
    ttt = (maxy-miny)/10; miny-=ttt; maxy+=ttt;

    int cmp = ((maxx-minx)>(maxy-miny)?(maxx-minx):(maxy-miny))/bedSize;
    padding = 12 - cmp/5;
    padding = padding>10?10:padding;
    padding = padding<1?1:padding;

    blockX = (maxx-minx)/k;
    blockY = (maxy-miny)/k;
	//when using operator=, value is initialized to false anyway
	std::unordered_map<size_t, std::unordered_map<size_t,bool>> block, padding_block, outline;


    qDebug() << minx << " " << maxx << " " << miny << " " << maxy;
    qDebug() << blockX << " " << blockY;
    for (int i = 0; i < edgeCnt; i++) {
        int startXBlock = int((edges[i].first.x - minx) / blockX);
        int endXBlock = int((edges[i].second.x - minx) / blockX);
        int startYBlock = int((edges[i].first.y - miny) / blockY);
        int endYBlock;
		//block.clear();
		//padding_block.clear();
		//outline.clear();

		//block.resize(std::max(startYBlock, endYBlock));
		//padding_block.resize(std::max(startYBlock, endYBlock));
		//outline.resize(std::max(startYBlock, endYBlock));
//        qDebug() << "fill block error here1";

        while(true){
            if (startXBlock == endXBlock)
                break;

//            qDebug() << "fill block error here2";
            long long int endXcor = minx + (startXBlock+1) * blockX;
            long long int endYcor = (edges[i].first.y + (edges[i].second.y - edges[i].first.y) * (endXcor - edges[i].first.x) / (edges[i].second.x - edges[i].first.x));
            endYBlock = int((endYcor - miny) / blockY);
//            qDebug() << "fill block error here3";
            for (int j = min2(startYBlock, endYBlock); j <= max2(startYBlock, endYBlock); j++){
                block[j][startXBlock] = true;
            }

            startXBlock ++;
            startYBlock = endYBlock;
        }
//        qDebug() << "fill block error here4";
        endYBlock = int((edges[i].second.y - miny) / blockY);
        for (int j = min2(startYBlock, endYBlock); j <= max2(startYBlock, endYBlock); j++){
            block[j][startXBlock] = true;
        }
    }

    qDebug() << "make padding";
    for (int i = 0; i <= k; i++){
        for (int j = 0; j <= k; j++) {
            if (block[i][j]) {
                int iMax = min2(k, i+padding), iMin = max2(0, i-padding);
                int jMax = min2(k, j+padding), jMin = max2(0, j-padding);
                for (int h = jMin; h <= jMax; h++){
                    padding_block[iMin][h] = true;
                    padding_block[iMax][h] = true;
                }
                for (int h = iMin; h <= iMax; h++){
                    padding_block[h][jMin] = true;
                    padding_block[h][jMax] = true;
                }
            }
        }
    }
    block = padding_block;

    qDebug() << "find dfs start point";
    int startY=k, startX=k;
    for (int i = 0; i <= k; i++) {
        for (int j = 0; j <= k; j++) {
            if (block[i][j]) {
                if (i < startY) {
                    startY = i; startX = j;
                } else if (i == startY && j < startX){
                    startY = i; startX = j;
                }
            }
        }
    }

    qDebug() << "get outline";
    vertice.push_back(std::make_pair(startY, startX));
    if (block[startY][startX+1])
        make_outline(startY, startX, startY, startX, 0, 0, &block, &outline, &vertice, k);
    else if (block[startY+1][startX+1])
        make_outline(startY, startX, startY, startX, 0, 7, &block, &outline, &vertice, k);
    else if (block[startY+1][startX])
        make_outline(startY, startX, startY, startX, 0, 6, &block, &outline, &vertice, k);
    else
        startX /= 0;

//    char tttttt[400];
//    for (int i = k; i >= 0; i--){
//        for (int j = 0; j <=k ; j++){
//            if (outline[i][j]) {
//                tttttt[j*2] = 'O';
//                tttttt[j*2+1] = ' ';
//            } else {
//                tttttt[j*2] = ' ';
//                tttttt[j*2+1] = ' ';
//            }
//        }
//        tttttt[k*2+1] = '\0';
//        qDebug() << tttttt;
//    }

    qDebug() << "make summarized outline";

    int vn = vertice.size();
    for (int i = 0; i < vn; i++){
        int y1 = vertice[(i-1+vn)%vn].first-vertice[(i-2+vn)%vn].first;
        int x1 = vertice[(i-1+vn)%vn].second-vertice[(i-2+vn)%vn].second;
        int y2 = vertice[(i+1)%vn].first-vertice[i].first;
        int x2 = vertice[(i+1)%vn].second-vertice[i].second;
        int y3 = vertice[i].first-vertice[(i-1+vn)%vn].first;
        int x3 = vertice[i].second-vertice[(i-1+vn)%vn].second;
        int y4 = vertice[(i+2)%vn].first-vertice[(i+1)%vn].first;
        int x4 = vertice[(i+2)%vn].second-vertice[(i+1)%vn].second;
        if (y1 != y2 || x1 != x2 || y3 != y4 || x3 != x4 || (y1 * y1 + x1 * x1 != 1 && y3 * y3 + x3 * x3 != 1)) {
            LLIPoint tt;
            tt.y = miny + vertice[i].first * blockY;
            tt.x = minx + vertice[i].second * blockX;
            result.push_back(tt);
        }
    }

    return result;
}

int CCW(LLIPoint p1, LLIPoint p2, LLIPoint p3) {
    long long int ccw = p1.x*p2.y+p2.x*p3.y+p3.x*p1.y - p1.y*p2.x-p2.y*p3.x-p3.y*p1.x;
    if (ccw > 0)
        return 1;
    if (ccw < 0)
        return -1;
    return 0;
}

bool isIntersectedLines(LLIPoint A, LLIPoint B, LLIPoint C, LLIPoint D) {
    return (CCW(A,C,D) != CCW(B,C,D) && CCW(A,B,C) != CCW(A,B,D));
}

long long int getConvexHullSizeIfNotCollision(std::vector<std::vector<LLIPoint> > *outlines, std::vector<LLIPoint> *centers, std::vector<LLIPoint> *translational_motions, std::vector<float> *rotates) {
    int meshCnt = outlines->size();

    std::vector<std::vector<LLIPoint> > newOutLines;
    for (int i = 0; i < meshCnt; i++){
        std::vector<LLIPoint> newOutLine;
        float rad_angle = -(*rotates)[i] * M_PI / 180;
        float cosq = cos(rad_angle), sinq = sin(rad_angle);
        int vertexCnt = (*outlines)[i].size();
        for (int j = 0; j < vertexCnt; j++) {
            LLIPoint newPoint;
            newPoint.x = ((*outlines)[i][j].x - (*centers)[i].x) * cosq - ((*outlines)[i][j].y - (*centers)[i].y) * sinq + (*centers)[i].x + (*translational_motions)[i].x;
            newPoint.y = ((*outlines)[i][j].y - (*centers)[i].y) * cosq + ((*outlines)[i][j].x - (*centers)[i].x) * sinq + (*centers)[i].y + (*translational_motions)[i].y;
            newOutLine.push_back(newPoint);
        }
        newOutLines.push_back(newOutLine);
    }

    for (int i = 0; i < meshCnt - 1; i++){
        int j = meshCnt - 1;
        // because moved target mesh is last mesh
        int iCnt = newOutLines[i].size();
        int jCnt = newOutLines[j].size();
        for (int i1=0, i2=iCnt-1; i1 < iCnt; i2=i1++){
            for (int j1=0, j2=jCnt-1; j1 < jCnt; j2=j1++){
                if (isIntersectedLines(newOutLines[i][i1], newOutLines[i][i2], newOutLines[j][j1], newOutLines[j][j2]))
                    return 100000000000000000;
            }
        }
    }
    Path ppp;
    for (int i = 0; i < meshCnt; i++){
        int vertexCnt = newOutLines[i].size();
        for (int j = 0; j < vertexCnt; j++) {
            ppp.push_back(IntPoint(newOutLines[i][j].x, newOutLines[i][j].y));
        }
    }
    Path convex = getConvexHull(&ppp);
    int convexVertexCnt = convex.size();
    long long int contourLength=0, temp;

    for (int i =0, j = convexVertexCnt-1; i < convexVertexCnt; j = i++){
        contourLength += sqrt((convex[j].Y - convex[i].Y) * (convex[j].Y - convex[i].Y) + (convex[j].X - convex[i].X) * (convex[j].X - convex[i].X));
    }

    return contourLength;
}


std::vector<XYArrangement> autoarrange::arngMeshes(std::vector<const Mesh*>& meshes, std::vector<const Qt3DCore::QTransform*> m_transform_set){
    std::vector<std::vector<LLIPoint> > outlines;
    std::vector<LLIPoint> centers;
    std::vector<LLIPoint> translational_motions;
    std::vector<float> rotates;
    std::vector<XYArrangement> result;
    srand((unsigned int)time(NULL));
    int meshCnt = meshes.size();

    int floatPointPadding = 100;
    /**/qDebug() << "Arrange start";
    qmlManager->setProgress(0);
    qmlManager->setProgressText("Arranging models...");

    int temp_idx= 0;
    for (const Qt3DCore::QTransform* transform : m_transform_set){

        qDebug() << "arngMesh model transform :" << temp_idx  << transform->translation() << transform->rotation();
        temp_idx ++;
    }

    qDebug() << "get padded projected outlines";
    for(int idx=0; idx<meshCnt; idx++){
        LLIPoint temp;
        temp.x = (meshes[idx]->x_max() + meshes[idx]->x_min()) / 2 * floatPointPadding;
        temp.y = (meshes[idx]->y_max() + meshes[idx]->y_min()) / 2 * floatPointPadding;
        centers.push_back(temp);
        QVector3D t1 = m_transform_set[idx]->translation();
        temp.x = t1.x() * floatPointPadding; temp.y = t1.y() * floatPointPadding;
        translational_motions.push_back(temp);
        rotates.push_back(0.0);
        outlines.push_back(getPaddedProj(meshes[idx], floatPointPadding));
        qDebug() << "complete get padded projection" << " size: " << outlines[idx].size();
        qmlManager->setProgress(0.5*idx/meshCnt);
    }

    qDebug() << "start";
    int targetIdx = meshCnt-1;
    if (getConvexHullSizeIfNotCollision(&outlines, &centers, &translational_motions, &rotates) == 100000000000000000){
        translational_motions[targetIdx].x = 70*floatPointPadding;
        translational_motions[targetIdx].y = 70*floatPointPadding;
    }
    for (int trycnt = 0; trycnt < 30; trycnt++) {
        LLIPoint origin_translational_motion = translational_motions[targetIdx], best_translational_motion;
        long long int optimal_v = 100000000000000000;
        float origin_rotate = rotates[targetIdx], best_rotate;

        float dx[14] = {0, -0.01, 0.01, -0.03, 0.03, -0.1, 0.1, -0.3, 0.3, 1, 1, 3, -3, -10};
        float dy[14] = {0, -0.01, 0.01, -0.03, 0.03, -0.1, 0.1, -0.3, 0.3, 1, 1, 3, -3, -10};
        float dr[11] = {0, -1, 1, -3, 3, -10, 10, -30, 30, -90, 90};

        for (int xcase = 0; xcase < 14; xcase++) {
            for (int ycase = 0; ycase < 14; ycase++) {
                for (int rcase = 0; rcase < 11; rcase++) {
                    translational_motions[targetIdx].x = origin_translational_motion.x + dx[xcase] * floatPointPadding;
                    translational_motions[targetIdx].y = origin_translational_motion.y + dy[ycase] * floatPointPadding;
                    rotates[targetIdx] = origin_rotate + dr[rcase];
                    long long int value = getConvexHullSizeIfNotCollision(&outlines, &centers, &translational_motions, &rotates);
                    if (value < optimal_v) {
                        optimal_v = value;
                        best_translational_motion = translational_motions[targetIdx];
                        best_rotate = rotates[targetIdx];
                    }
                }
            }
        }
        if (optimal_v != 100000000000000000) {
            translational_motions[targetIdx] = best_translational_motion;
            rotates[targetIdx] = best_rotate;
        }
        qDebug() << "best value: " << optimal_v;
        qDebug() << best_translational_motion.x << best_translational_motion.y;
        qDebug() << best_rotate;
        if (best_translational_motion.x == origin_translational_motion.x && best_translational_motion.y == origin_translational_motion.y && origin_rotate == rotates[targetIdx])
            break;
        qmlManager->setProgress(0.4* (trycnt/30) + 0.5);
    }

    for(int idx=0; idx<meshCnt; idx++){
        result.push_back(std::make_pair(IntPoint(translational_motions[idx].x, translational_motions[idx].y), rotates[idx]));
    }
    /*result[1].first.X = 1000;
    result[1].first.Y = 0;
    result[1].second = 90.0;*/
    return result;
}

bool compareArea(std::pair<Paths*, int>& fig1, std::pair<Paths*, int>& fig2){//used for sort
    return Area((*fig1.first)[0])>Area((*fig2.first)[0]);
}

std::vector<XYArrangement> autoarrange::arng2D(std::vector<Paths>& figs){
    qmlManager->setProgressText("Simulating arrangement");
    std::vector<std::pair<Paths*, int> > figs_with_idx;//tried to use reference istead of pointer, but it caused error
    for(Paths& fig : figs) figs_with_idx.push_back({&fig, figs_with_idx.size()});
    sort(figs_with_idx.begin(), figs_with_idx.end(), compareArea);
    std::vector<XYArrangement> arng_result_set;
    arng_result_set.resize(figs.size(), {{0,0},-1});
    Paths cum_outline;
    initStage(cum_outline);
    int cumXOfNotArrangeable = 0;
    int progress = 0;
    for(std::pair<Paths*, int>& fig_with_idx : figs_with_idx){
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

#ifdef _DEBUG_AUTO_ARRANGE
    /**/debugPaths(cum_outline);
#endif
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
        return std::make_pair(IntPoint(0,0),-1);
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
        return std::make_pair(IntPoint(0,0),-1);
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
#ifdef _DEBUG_AUTO_ARRANGE
    /**/qDebug() << "- got object convexHull" << checkConvex(convex_obj);
    //*debugPath(convex_obj);
#endif
    std::vector<Vecs> sub_vecs;
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
        std::vector<Vecs> obj_vecs_in_regions = getObjVecsInRegions(sub_vecs[path_idx], obj_vecs);
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

std::vector<Vecs> autoarrange::getObjVecsInRegions(Vecs& sub_vecs, Vecs& obj_vecs){
    std::vector<Vecs> obj_vecs_in_regions;
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

//void autoarrange::arrangeQt3D(std::vector<Qt3DCore::QTransform*> m_transform_set, std::vector<XYArrangement> arng_result_set){
//    for(int idx=0; idx<arng_result_set.size(); idx++){
//        arrangeSingleQt3D(m_transform_set[idx], arng_result_set[idx]);
//    }
//}
//
//void autoarrange::arrangeSingleQt3D(Qt3DCore::QTransform* m_transform, XYArrangement arng_result){
//    QVector3D trans_vec = QVector3D(arng_result.first.X/ClipperLib::INT_PT_RESOLUTION, arng_result.first.Y/ClipperLib::INT_PT_RESOLUTION, m_transform->translation().z());
//    m_transform->setTranslation(trans_vec);
//    m_transform->setRotationZ(arng_result.second);
//}

/*void autoarrange::arrangeGlmodels(std::vector<GLModel*>* glmodels){
    std::vector<Mesh> meshes_to_arrange;
    std::vector<XYArrangement> arng_result_set;
    std::vector<Qt3DCore::QTransform*> m_transform_set;
    for(int idx=0; idx<glmodels->size(); idx++){
        meshes_to_arrange.push_back(* (*glmodels)[idx]->mesh);
        m_transform_set.push_back((*glmodels)[idx]->m_transform);
    }
    arng_result_set = arngMeshes(&meshes_to_arrange);
    arrangeQt3D(m_transform_set, arng_result_set);
}*/

#ifdef _DEBUG_AUTO_ARRANGE

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




void autoarrange::debugPaths(Paths paths) {
	qDebug() << "===============";
	for (size_t i = 0; i < paths.size(); i++) {
		qDebug() << "";
		//qDebug() << "path" << i;
		for (size_t j = 0; j < paths[i].size(); j++) {
			qDebug() << paths[i][j].X << paths[i][j].Y;
		}
	}
	qDebug() << "===============";
}

void autoarrange::debugPath(Path path) {
	qDebug() << "===============";
	for (size_t i = 0; i < path.size(); i++) {
		qDebug() << path[i].X << path[i].Y;
	}
	qDebug() << "===============";
}

void autoarrange::debugFaces(const Mesh* mesh, std::vector<const MeshFace *> face_list) {
	for (size_t i = 0; i < face_list.size(); i++) {
		//debugFace(mesh, face_list[i]);
	}
}
//
//void autoarrange::debugFace(const Mesh* mesh, const MeshFace * face) {
//    const MeshFace * mf = face;
//	for (HalfEdgeConstItr edge = 0; side < 3; side++) {
//		QVector3D vtx = mf->meshVertices()[side]->position;
//		float x_f = vtx.x();
//		float y_f = vtx.y();
//		float z_f = vtx.z();
//		int x_int = round(x_f * ClipperLib::INT_PT_RESOLUTION);
//		int y_int = round(y_f * ClipperLib::INT_PT_RESOLUTION);
//		int z_int = round(z_f * ClipperLib::INT_PT_RESOLUTION);
//		qDebug() << "(" << x_f << "," << y_f << "," << z_f << ")";
//	}
//	qDebug() << "face normal:" << "(" << mf->fn.x() << "," << mf->fn.y() << "," << mf->fn.z() << ")";
//	for (HalfEdgeConstItr edge = 0; side < 3; side++) {
//		if (mf->neighboring_faces[side].size() == 1) {
//			const MeshFace * neighbor = mf->neighboring_faces[side][0];
//			if (neighbor->fn.z() >= 0) {
//				if (isNbrOrientSame(mesh, mf, side)) qDebug() << "side" << side;
//				else qDebug() << "side" << side << ": bound(ornt diff)";
//			}
//			else {
//				if (isNbrOrientSame(mesh, mf, side)) qDebug() << "side" << side << ": bound(fn.z diff)";
//				else qDebug() << "side" << side << ": bound(fn.z diff, ornt diff)";
//			}
//		}
//		else if (mf->neighboring_faces[side].size() == 0) {
//			qDebug() << "side" << side << ": bound(no neighbor)";
//		}
//		else {
//			qDebug() << "side" << side << ": bound(multi-neighbor" << mf->neighboring_faces[side].size() << ")";
//		}
//	}
//}

#endif
