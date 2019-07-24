#include "slicer.h"
#include <QHash>
#include <QElapsedTimer>
#include <QTextStream>
#include "qmlmanager.h"
#include "../../utils/mathutils.h"
#include "polyclipping/QDebugPolyclipping.h"
#include "configuration.h"

using namespace ClipperLib;
using namespace Hix::Debug;
QDebug Hix::Debug::operator<< (QDebug d, const Slice& obj) {
	d << "z: " << obj.z;
	d << "polytree: " << obj.polytree;
	d << "overhang_region: " << obj.overhang_region;
	d << "critical_overhang_region: " << obj.critical_overhang_region;

	d << "outershell: " << obj.outershell;
	d << "infill: " << obj.infill;
	d << "support: " << obj.support;
	d << "Area: " << obj.Area;
	return d;
}
QDebug Hix::Debug::operator<< (QDebug d, const Slices& obj) {
	d << "mesh: " << obj.mesh;
	d << "raft_points: " << obj.raft_points;
	d << "overhang_regions: " << obj.overhang_regions;
	d << "overhang_points: " << obj.overhang_points;
	d << "slices: ";
	for (const auto& each : obj)
	{
		d << each;
	}
	return d;
}



namespace Slicer
{
	namespace Private
	{
		/****************** Mesh Slicing Step *******************/
		std::vector<Paths> meshSlice(Mesh* mesh); // totally k elements
		/****************** Helper Functions For Mesh Slicing Step *******************/
		std::vector<std::vector<const MeshFace*>> buildTriangleLists(Mesh* mesh, std::vector<float> planes, float delta);
		std::vector<float> buildUniformPlanes(float z_min, float z_max, float delta);
		std::vector<float> buildAdaptivePlanes(float z_min, float z_max);
		/****************** Helper Functions For Contour Construction Step *******************/
		void insertPathHash(QHash<uint32_t, Path>& pathHash, IntPoint u, IntPoint v);
	}
}
using namespace Slicer::Private;

bool Slicer::slice(Mesh* mesh, Slices* slices){
    slices->mesh = mesh;

    if (mesh == nullptr || mesh->getFaces().size() ==0){
        return true;
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
        slices->push_back(meshslice);
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

    slices->containmentTreeConstruct();
    return true;
}

/****************** Mesh Slicing Step *******************/

// slices mesh into segments
std::vector<Paths> Slicer::Private::meshSlice(Mesh* mesh){
    float delta = scfg->layer_height;

    std::vector<float> planes;

    if (scfg->slicing_mode == SlicingConfiguration::SlicingMode::Uniform) {
        planes = buildUniformPlanes(mesh->z_min(), mesh->z_max(), delta);
    } 
	//else if (scfg->slicing_mode == "adaptive") {
 //       // adaptive slice
 //       planes = buildAdaptivePlanes(mesh->z_min(), mesh->z_max());
 //   }

    // build triangle list per layer height
    std::vector<std::vector<const MeshFace*>> triangleLists = buildTriangleLists(mesh, planes, delta);
    std::vector<Paths> pathLists;

    std::vector<const MeshFace*> A;
    A.reserve(mesh->getFaces().size());

    for (int i=0; i<planes.size(); i++){
        A.insert(A.end(), triangleLists[i].begin(), triangleLists[i].end()); // union
        Paths paths;

        for (int t_idx=0; t_idx<A.size(); t_idx++){
            const MeshFace& cur_mf = *A[t_idx];
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
std::vector<std::vector<const MeshFace*>> Slicer::Private::buildTriangleLists(Mesh* mesh, std::vector<float> planes, float delta){

    // Create List of list
    std::vector<std::vector<const MeshFace*>> list_list_triangle;
    for (int l_idx=0; l_idx < planes.size()+1; l_idx ++){
        list_list_triangle.push_back(std::vector< const MeshFace*>());
    }

    // Uniform Slicing O(n)
    if (delta>0){
		for(const auto& mf : mesh->getFaces())
		{
			int llt_idx;
            float z_min = mesh->getFaceZmin(mf);
            if (z_min < planes[0])
                llt_idx = 0;
            else if (z_min > planes[planes.size()-1])
                llt_idx = planes.size()-1;
            else
                llt_idx = (int)((z_min - planes[0])/delta) +1;


            list_list_triangle[llt_idx].push_back(&mf);
        }
    }

    // General Case
    else {

    }

    return list_list_triangle;
}


std::vector<float> Slicer::Private::buildUniformPlanes(float z_min, float z_max, float delta){
	float halfDelta = delta / 2.0f;
    std::vector<float> planes;

    int idx_max = ceil((z_max-z_min)/delta);
	int idx = 0;
	float plane_z = Utils::Math::round(z_min + halfDelta, 3);
	while (plane_z < z_max)
	{
		qDebug() << "build Uniform Planes at height z " << plane_z;
		planes.push_back(plane_z);
		++idx;
		plane_z = Utils::Math::round(z_min + delta * idx + halfDelta, 3);
	} 
    return planes;
}

std::vector<float> Slicer::Private::buildAdaptivePlanes(float z_min, float z_max){
    std::vector<float> planes;
    return planes;
}


/****************** Helper Functions For Contour Construction Step *******************/

void Slicer::Private::insertPathHash(QHash<uint32_t, Path>& pathHash, IntPoint u, IntPoint v){
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
                pointPath.push_back(c2t::Point((float)ip.X/ClipperLib::INT_PT_RESOLUTION,(float)ip.Y/ClipperLib::INT_PT_RESOLUTION));
            }
            pointPaths.push_back(pointPath);
        }

        clip2tri ct;

        ct.mergePolysToPolyTree(pointPaths, slice->polytree);
        qDebug() << "in slice " << idx << ", constructing polytree with " << slice->outershell.size() << " to " << pointPaths.size() << " point pathes" << " total tree : " << slice->polytree.Total();
    }*/
}


