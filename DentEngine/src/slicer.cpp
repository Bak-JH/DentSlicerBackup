#include "slicer.h"
#include <QHash>
#include <QElapsedTimer>
#include <QTextStream>
#include "qmlmanager.h"
#include "../../utils/mathutils.h"
#include "polyclipping/QDebugPolyclipping.h"
#include "configuration.h"
#if defined(_DEBUG) || defined(QT_DEBUG )
#define _STRICT_SLICER
//#define _STRICT_MESH_NO_SELF_INTERSECTION
#endif
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
		std::vector<Paths> meshSlice(const Mesh* mesh); // totally k elements
		/****************** Helper Functions For Mesh Slicing Step *******************/
		std::vector<std::vector<FaceConstItr>> buildTriangleLists(const Mesh* mesh, std::vector<float> planes, float delta);
		std::vector<float> buildUniformPlanes(float z_min, float z_max, float delta);
		std::vector<float> buildAdaptivePlanes(float z_min, float z_max);
		/****************** Helper Functions For Contour Construction Step *******************/
		void insertPathHash(QHash<uint32_t, Path>& pathHash, IntPoint u, IntPoint v);
		/********************** Path Generation Functions **********************/
		IntPoint toInt2DPt(QVector3D pt);
		Paths3D intersectionPaths(ClipperLib::Path Contour, Plane target_plane);
		Path3D intersectionPath(Plane base_plane, Plane target_plane);
		ContourSegment intersectionPath(FaceConstItr& mf, float z);
	}
}
using namespace Slicer::Private;





/********************** Path Generation Functions **********************/

// converts float point to int in microns
void  Slicer::addPoint(float x, float y, ClipperLib::Path* path)
{
	IntPoint ip;
	ip.X = round(x * ClipperLib::INT_PT_RESOLUTION);
	ip.Y = round(y * ClipperLib::INT_PT_RESOLUTION);
	//qDebug() << "addPoint called with x " << x << " y " << y << " rounding " << ip.X;
	path->push_back(ip);
}


IntPoint  Slicer::Private::toInt2DPt(QVector3D pt)
{
	IntPoint ip;
	ip.X = round(pt.x() * ClipperLib::INT_PT_RESOLUTION);
	ip.Y = round(pt.y() * ClipperLib::INT_PT_RESOLUTION);
	return ip;
}


float minDistanceToContour(QVector3D from, ClipperLib::Path contour) {
	float min_distance = 0;
	for (int i = 0; i < contour.size() - 1; i++) {
		ClipperLib::Path temp_path;
		temp_path.push_back(contour[i]);
		temp_path.push_back(contour[i + 1]);
		QVector3D int2qv3 = QVector3D(((float)contour[i].X) / ClipperLib::INT_PT_RESOLUTION, ((float)contour[i].Y) / ClipperLib::INT_PT_RESOLUTION, from.z());
		IntPoint directionInt = contour[i + 1] - contour[i];
		QVector3D direction = QVector3D(directionInt.X / ClipperLib::INT_PT_RESOLUTION, directionInt.Y / ClipperLib::INT_PT_RESOLUTION, 0);
		float cur_distance = from.distanceToLine(int2qv3, direction);
		if (abs(min_distance) > cur_distance) {
			min_distance = cur_distance;
		}
	}
	return min_distance;
}


inline void rotateCCW90(QVector3D& vec)
{
	//(-y,x)
	auto tmp = vec.x();
	vec.setX(-1.0f * vec.y());
	vec.setY(tmp);
}

ContourSegment::ContourSegment(VertexConstItr& vtxA, VertexConstItr& vtxB, FaceConstItr& face)
{
	IntPoint a = toInt2DPt(vtxA->position);
	IntPoint b = toInt2DPt(vtxB->position);
	//QVector3D origNormal = vtxA->vn + vtxB->vn;
	//origNormal.normalize();

	auto origFn = face->fn;
	auto mvs = face->meshVertices();
	auto DE = mvs[0]->position - mvs[1]->position;
	auto EF = mvs[1]->position - mvs[2]->position;
	auto calcFn = QVector3D::crossProduct(DE, EF);
	auto calcFn2 = QVector3D::crossProduct(EF, DE);
	auto AB = vtxB->position - vtxA->position;
	auto BA = vtxA->position - vtxB->position;
	auto vA = vtxA->position;
	auto vB = vtxB->position;


	QVector3D faceNormal = face->fn;
	faceNormal.setZ(0.0f);
	faceNormal.normalize();

	QVector3D ABNormal = vtxB->position - vtxA->position;
	rotateCCW90(ABNormal);
	ABNormal.setZ(0.0f);
	ABNormal.normalize();

	QVector3D BANormal = vtxA->position - vtxB->position;
	rotateCCW90(BANormal);
	BANormal.setZ(0.0f);
	BANormal.normalize();

}

ContourSegment::ContourSegment(VertexConstItr& vtxA0, VertexConstItr& vtxA1, VertexConstItr& vtxB, float z, FaceConstItr& face)
{
}

ContourSegment::ContourSegment(VertexConstItr& vtxA0, VertexConstItr& vtxA1, VertexConstItr& vtxB0, VertexConstItr& vtxB1, float z, FaceConstItr& face)
{
}



Path3D  Slicer::Private::intersectionPath(Plane base_plane, Plane target_plane)
{
	Path3D p;

	std::vector<QVector3D> upper;
	std::vector<QVector3D> lower;
	for (int i = 0; i < 3; i++) {
		if (target_plane[i].distanceToPlane(base_plane[0], base_plane[1], base_plane[2]) > 0) {
			upper.push_back(target_plane[i]);
		}
		else {
			lower.push_back(target_plane[i]);
		}
	}

	std::vector<QVector3D> majority;
	std::vector<QVector3D> minority;

	bool flip = false;
	if (upper.size() == 2) {
		majority = upper;
		minority = lower;
	}
	else if (lower.size() == 2) {
		flip = true;
		majority = lower;
		minority = upper;
	}
	else {
		qDebug() << "wrong faces";
		// size is 0
		return p;
	}

	float minority_distance = abs(minority[0].distanceToPlane(base_plane[0], base_plane[1], base_plane[2]));
	float majority1_distance = abs(majority[0].distanceToPlane(base_plane[0], base_plane[1], base_plane[2]));
	float majority2_distance = abs(majority[1].distanceToPlane(base_plane[0], base_plane[1], base_plane[2]));

	// calculate intersection points
	MeshVertex mv1, mv2;
	mv1.position = minority[0] + (majority[0] - minority[0]) * (minority_distance / (majority1_distance + minority_distance));
	mv2.position = minority[0] + (majority[1] - minority[0]) * (minority_distance / (majority2_distance + minority_distance));

	if (flip) {
		p.push_back(mv1);
		p.push_back(mv2);
	}
	else {
		p.push_back(mv2);
		p.push_back(mv1);
	}
	return p;
}



ContourSegment  Slicer::Private::intersectionPath(FaceConstItr& mf, float z)
{
	std::vector<VertexConstItr> upper;
	std::vector<VertexConstItr> middle;
	std::vector<VertexConstItr> lower;

	auto mfVertices = mf->meshVertices();
	for (int i = 0; i < 3; i++) {
		if (mfVertices[i]->position.z() > z) {
			upper.push_back(mfVertices[i]);
		}
		else if (mfVertices[i]->position.z() == z) {
			middle.push_back(mfVertices[i]);
		}
		else
			lower.push_back(mfVertices[i]);
	}
	if (middle.size() == 2)
	{
		ContourSegment seg(middle[0], middle[1], mf);
		return seg;
	}
	return ContourSegment(mfVertices[0], mfVertices[1], mf);
	//std::vector<VertexConstItr> majority;
	//std::vector<VertexConstItr> minority;

	////two edges intersect
	//if (middle.size() == 0)
	//{
	//	if (upper.size() == 2 && lower.size() == 1) {
	//		majority = upper;
	//		minority = lower;
	//	}
	//	else {
	//		majority = lower;
	//		minority = upper;
	//	}
	//}
	//else {
	//	if (lower.size() != upper.size() && middle.size() == 1) {
	//		return p;
	//	}
	//	else if (upper.size() == 1 && lower.size() == 1 && middle.size() == 1) {
	//		addPoint(middle[0]->position.x(), middle[0]->position.y(), &p);
	//		//add intermediate position
	//		float x_0, y_0;
	//		x_0 = ((upper[0]->position.z() - z) * lower[0]->position.x() + (z - lower[0]->position.z()) * upper[0]->position.x()) / (upper[0]->position.z() - lower[0]->position.z());
	//		y_0 = ((upper[0]->position.z() - z) * lower[0]->position.y() + (z - lower[0]->position.z()) * upper[0]->position.y()) / (upper[0]->position.z() - lower[0]->position.z());

	//		addPoint(x_0, y_0, &p);

	//		return p;
	//	}
	//	else if (middle.size() == 2) {
	//		addPoint(middle[0]->position.x(), middle[0]->position.y(), &p);
	//		addPoint(middle[1]->position.x(), middle[1]->position.y(), &p);
	//		return p;
	//	}
	//	else if (middle.size() == 3) {
	//		return p;
	//	}
	//	return p;
	//}

	//float x_0, y_0, x_1, y_1;
	//x_0 = (minority[0]->position.x() - majority[0]->position.x())
	//	* ((z - majority[0]->position.z()) / (minority[0]->position.z() - majority[0]->position.z()))
	//	+ majority[0]->position.x();
	//y_0 = (minority[0]->position.y() - majority[0]->position.y())
	//	* ((z - majority[0]->position.z()) / (minority[0]->position.z() - majority[0]->position.z()))
	//	+ majority[0]->position.y();
	//x_1 = (minority[0]->position.x() - majority[1]->position.x())
	//	* ((z - majority[1]->position.z()) / (minority[0]->position.z() - majority[1]->position.z()))
	//	+ majority[1]->position.x();
	//y_1 = (minority[0]->position.y() - majority[1]->position.y())
	//	* ((z - majority[1]->position.z()) / (minority[0]->position.z() - majority[1]->position.z()))
	//	+ majority[1]->position.y();

	//addPoint(x_0, y_0, &p);
	//addPoint(x_1, y_1, &p);
	//return p;
}
bool Slicer::slice(const Mesh* mesh, Slices* slices){
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
std::vector<Paths> Slicer::Private::meshSlice(const Mesh* mesh){
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
    std::vector<std::vector<FaceConstItr>> triangleLists = buildTriangleLists(mesh, planes, delta);
    std::vector<Paths> pathLists;

    for (int i=0; i<planes.size(); i++){
        Paths paths;
		auto& facesAtPlane = triangleLists.at(i);
		float currPlane = planes[i];
		for (auto face : facesAtPlane)
		{
			// compute intersection including on same line 2 points or 1 point
			intersectionPath(face, currPlane);

			//Path intersection =
			//if (intersection.size() > 0) {
			//	paths.push_back(intersection);
			//}
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
std::vector<std::vector<FaceConstItr>> Slicer::Private::buildTriangleLists(const Mesh* mesh, std::vector<float> planes, float delta){

    // Create List of list
    std::vector<std::vector<FaceConstItr>> list_list_triangle;
    for (int l_idx=0; l_idx < planes.size(); l_idx ++){
        list_list_triangle.push_back(std::vector<FaceConstItr>());
    }

    // Uniform Slicing O(n)
    if (delta>0){
		auto& faces = mesh->getFaces();
		for(auto mf = faces.cbegin(); mf != faces.cend(); ++mf)
		{
            float z_min = mesh->getFaceZmin(*mf);
			float z_max = mesh->getFaceZmax(*mf);
			int minIdx = (int)((z_min - planes[0]) / delta) + 1;
			int maxIdx = (int)((z_max - planes[0]) / delta);
			minIdx = std::max(0, minIdx);
			maxIdx = std::min((int)planes.size() - 1, maxIdx);
			if (z_min == z_max)
				continue;
			for (int i = minIdx; i <= maxIdx; ++i)
			{
				list_list_triangle[i].push_back(mf);
			}
        }
    }
    // General Case
    else {
#ifdef _STRICT_SLICER
		throw std::runtime_error("zero delta when building triangle list for slicing");
#endif
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

