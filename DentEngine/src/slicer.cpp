#include "slicer.h"
#include <QHash>
#include <QElapsedTimer>
#include <QTextStream>
#include <unordered_map>
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
		std::vector<std::vector<ContourSegment>> meshSlice(const Mesh* mesh); // totally k elements
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
		QVector3D midPoint(VertexConstItr& vtxA0, VertexConstItr& vtxA1, float z);
	}
}
using namespace Slicer::Private;



QVector3D Slicer::Private::midPoint(VertexConstItr& vtxA0, VertexConstItr& vtxA1, float z)
{
	float x, y, zRatio;
	zRatio = ((z - vtxA0->position.z()) / (vtxA1->position.z() - vtxA0->position.z()));
	x = (vtxA1->position.x() - vtxA0->position.x()) * zRatio
		+ vtxA0->position.x();
	y = (vtxA1->position.y() - vtxA0->position.y()) * zRatio
		+ vtxA0->position.y();
	QVector3D A(x, y, z);
	return A;
}


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

ContourSegment::ContourSegment()
{
}



void ContourSegment::init(const QVector3D& a, const QVector3D& b, FaceConstItr& face)
{
	auto AInt = toInt2DPt(a);
	auto BInt = toInt2DPt(b);
	//points are too close, ie dist < min_resolution
	if (AInt == BInt)
		return;

	//determine direction
	QVector3D faceNormal = face->fn;
	faceNormal.setZ(0.0f);
	faceNormal.normalize();

	QVector3D ABNormal = b - a;
	rotateCCW90(ABNormal);
	ABNormal.setZ(0.0f);
	ABNormal.normalize();

	QVector3D BANormal = a - b;
	rotateCCW90(BANormal);
	BANormal.setZ(0.0f);
	BANormal.normalize();

	//face normal projected over z-plane should still be normal for AB/BA vector.
	//Now determine which vector direction is correct by comparing CCW90 or CW90 normals to projected Face normal
	auto ABDiff = (ABNormal - faceNormal).lengthSquared();
	auto BADiff = (BANormal - faceNormal).lengthSquared();


#ifdef _STRICT_SLICER
	//since projected face normal is still a normal for the AB/BA vector
	//ABNormal == faceNormal or BANormal == faceNormal
	//hence minimum of those two diffs should be very close to be zero
	auto smallestDiff = std::min(ABDiff, BADiff);

	if (smallestDiff > 0.01f)
	{
		throw std::runtime_error("Slicing: contour segment normal do not match the original face normal");
	}
#endif
	//if normal is actually other way around
	if (ABDiff < BADiff)
	{
		//AB is correct direction
		to = AInt;
		from = BInt;
		normal = DoublePoint(ABNormal.x(), ABNormal.y());
	}
	else
	{
		//BA is correct direction
				//AB is correct direction
		to = BInt;
		from = AInt;
		normal = DoublePoint(BANormal.x(), BANormal.y());
	}
}


ContourSegment::ContourSegment(VertexConstItr& vtxA, VertexConstItr& vtxB, FaceConstItr& face)
{
	init(vtxA->position, vtxB->position, face);
}



ContourSegment::ContourSegment(VertexConstItr& vtxA0, VertexConstItr& vtxA1, VertexConstItr& vtxB, float z, FaceConstItr& face)
{

	QVector3D A = midPoint(vtxA0, vtxA1, z);
	init(A, vtxB->position, face);


}

ContourSegment::ContourSegment(VertexConstItr& vtxA0, VertexConstItr& vtxA1, VertexConstItr& vtxB0, VertexConstItr& vtxB1, float z, FaceConstItr& face)
{
	QVector3D A = midPoint(vtxA0, vtxA1, z);
	QVector3D B = midPoint(vtxB0, vtxB1, z);
	init(A, B, face);

}

bool ContourSegment::isValid()
{
	return to != from;
}



// construct closed contour using segments created from meshSlice step
std::vector<Contour> ContourGen::contourConstruct(const std::vector<ContourSegment>& segments) {
	
	std::vector<Contour> contours;
	//use hash for god sake!
	//key = ContourSegment.from, value = pointer to segment
	std::unordered_map<IntPoint, const ContourSegment*> availableSegs;
	for (auto& each : segments)
	{
		availableSegs[each.from] = &each;
	}
	while (!availableSegs.empty())
	{
		auto startSeg = availableSegs.begin()->second;
		Contour currContour(startSeg);
		availableSegs.erase(availableSegs.begin());
		while (!currContour.isClosed())
		{
			auto itr = availableSegs.find(currContour.getDestination());
#ifdef _STRICT_SLICER
			if (itr == availableSegs.end())
			{
				throw std::runtime_error("Slicing:unclosed contour found while slicing");
			}
#endif
			currContour.addNext(itr->second);
			availableSegs.erase(itr);
		}
		contours.push_back(currContour);
	}
	return contours;



	//Paths contourList;

	//QHash<uint32_t, Path> pathHash;

	//if (pathList.size() == 0)
	//	return contourList;

	//int pathCnt = 0;
	//for (int i = 0; i < pathList.size(); i++) {
	//	pathCnt += pathList[i].size();
	//}
	//qDebug() << pathCnt;
	//pathHash.reserve(pathCnt * 10);

	//int debug_count = 0;

	//// pathHash Construction
	//for (int i = 0; i < pathList.size(); i++) {
	//	Path p = pathList[i];
	//	//insertPathHash(pathHash, p[0], p[1]); // inserts opposite too

	//	if (p[0] == p[1])
	//		continue;

	//	uint32_t path_hash_u = intPoint2Hash(p[0]);
	//	uint32_t path_hash_v = intPoint2Hash(p[1]);

	//	if (!pathHash.contains(path_hash_u)) {
	//		debug_count++;
	//		pathHash[path_hash_u].push_back(p[0]);
	//	}
	//	if (!pathHash.contains(path_hash_v)) {
	//		debug_count++;
	//		pathHash[path_hash_v].push_back(p[1]);
	//	}
	//	pathHash[path_hash_u].push_back(p[1]);
	//	pathHash[path_hash_v].push_back(p[0]);
	//}

	//// remove duplicate IntPoint Loop xyzzw (5) / zww (3)
	//QHashIterator<uint32_t, Path> i(pathHash);
	//while (i.hasNext()) {
	//	i.next();
	//	if (i.value().size() == 3 && i.value()[1] == i.value()[2]) {
	//		qDebug() << "same one found ";
	//		for (IntPoint ip : i.value()) {
	//			qDebug() << "ip value : " << ip.X << ip.Y;
	//		}
	//		std::vector<IntPoint>* dest = &(pathHash[intPoint2Hash(i.value()[1])]);
	//		std::vector<IntPoint>::iterator dest_it = dest->begin();
	//		while (dest_it != dest->end()) {
	//			if ((*dest_it) == i.value()[0])
	//				dest_it = dest->erase(dest_it);
	//			else
	//				++dest_it;
	//		}

	//		if (dest->size() == 1) {
	//			pathHash.remove(intPoint2Hash(*(dest->begin())));
	//		}
	//		pathHash.remove(intPoint2Hash(i.value()[0]));
	//	}
	//}

	//// Build Polygons
	//while (pathHash.size() > 0) {
	//	Path contour;
	//	IntPoint start, pj_prev, pj, pj_next, last;

	//	QHash<uint32_t, Path>::iterator smallestPathHash = pathHash.begin();//findSmallestPathHash(pathHash);
	//	if (smallestPathHash.value().size() == 0) {
	//		pathHash.erase(smallestPathHash);
	//		break;
	//	}
	//	pj_prev = smallestPathHash.value()[0];
	//	start = pj_prev;
	//	contour.push_back(pj_prev);
	//	std::vector<IntPoint>* dest = &(smallestPathHash.value());

	//	if (dest->size() == 0) {
	//		qDebug() << "dest->size() == 0";
	//		pathHash.remove(intPoint2Hash(pj_prev));
	//		continue;
	//	}
	//	else if (dest->size() == 1) {
	//		qDebug() << "dest->size() == 1";
	//		pathHash.remove(intPoint2Hash(pj_prev));
	//		continue;
	//	}
	//	else if (dest->size() == 2) {
	//		qDebug() << "dest->size() == 2";
	//		pj = (*dest)[1];
	//		last = (*dest)[0]; // pj_prev itself
	//		pathHash.remove(intPoint2Hash(pj_prev));
	//	}
	//	else {
	//		pj = (*dest)[1];
	//		last = (*dest)[2];

	//		dest->erase(dest->begin() + 1);
	//		dest->erase(dest->begin() + 1);
	//		if (dest->size() == 1) {
	//			pathHash.remove(intPoint2Hash(pj_prev));
	//			//pj_next = last;
	//			//contour.push_back(pj);
	//		}
	//	}
	//	while (pj_next != last) {
	//		contour.push_back(pj);
	//		dest = &(pathHash[intPoint2Hash(pj)]);

	//		if (dest->size() == 0) {
	//			qDebug() << "dest->size() == 0 from loop";
	//			pathHash.remove(intPoint2Hash(pj));
	//			break;
	//		}
	//		else if (dest->size() == 1) {
	//			qDebug() << "dest->size() == 1 from loop";
	//			pathHash.remove(intPoint2Hash(pj));
	//			break;
	//		}
	//		else if (dest->size() == 2) {
	//			qDebug() << "dest->size() == 2 from loop";
	//			start = (*dest)[0]; // itself
	//			/*uint32_t endHash = intPoint2Hash((*dest)[1]);
	//			if (pathHash.contains(endHash))
	//				pathHash.remove(endHash);*/ // maybe needless

	//			pathHash.remove(intPoint2Hash(pj));
	//			pj_next = last;
	//			pj = pj_next;
	//			pj_prev = contour[0];
	//			last = start;
	//			reverse(contour.begin(), contour.end());
	//			continue;
	//		}

	//		// find pj_prev and choose another pj_next and remove pj_prev, pj_next from H[pj]
	//		for (int d = 1; d < dest->size(); d++) {
	//			if ((*dest)[d] == pj_prev) {
	//				dest->erase(dest->begin() + d);
	//				break;
	//			}
	//		}

	//		pj_next = (*dest)[1];
	//		dest->erase(dest->begin() + 1);
	//		if (dest->size() == 1)
	//			pathHash.remove(intPoint2Hash(pj));


	//		pj_prev = pj;
	//		pj = pj_next;
	//	}

	//	contour.push_back(last);
	//	contour.push_back(start);

	//	// collect last vertex's connectedness
	//	uint32_t last_hash = intPoint2Hash(last);
	//	if (pathHash.contains(last_hash)) {
	//		dest = &(pathHash[last_hash]);
	//		for (int d = 1; d < dest->size(); d++) {
	//			if ((*dest)[d] == pj_prev) {
	//				dest->erase(dest->begin() + d);
	//				break;
	//			}
	//		}
	//		for (int d = 1; d < dest->size(); d++) {
	//			if ((*dest)[d] == start) {
	//				dest->erase(dest->begin() + d);
	//				break;
	//			}
	//		}
	//		if (dest->size() == 1)
	//			pathHash.remove(last_hash);
	//	}

	//	// remove 2-vertices-contours
	//	if (contour.size() == 2)
	//		continue;

	//	/*if (Orientation(contour)){
	//		ReversePath(contour);
	//	}*/

	//	contourList.push_back(contour);
	//}


	//return contourList;
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
	std::vector<VertexConstItr> majority;
	std::vector<VertexConstItr> minority;

	//two edges intersect
	if (middle.size() == 0)
	{
		if (upper.size() == 2 && lower.size() == 1) {
			majority = upper;
			minority = lower;
		}
		else {
			majority = lower;
			minority = upper;
		}
		return ContourSegment(majority[0], minority[0], majority[1], minority[0], z, mf);
	}
	else {
		//when only a single tip touches with no intersecting edges
		if (lower.size() != upper.size() && middle.size() == 1) {
			return ContourSegment();
		}
		//1 edge interesecting, 1 vertice on the plane
		else if (upper.size() == 1 && lower.size() == 1 && middle.size() == 1) {
			return 	ContourSegment(upper[0], lower[0], middle[0], z, mf);
		}
		else if (middle.size() == 2) {
			return 	ContourSegment(middle[0], middle[1], mf);
		}
		//face == plane
		else if (middle.size() == 3) {
			return  ContourSegment();
		}
	}
}
bool Slicer::slice(const Mesh* mesh, Slices* slices){
    slices->mesh = mesh;

    if (mesh == nullptr || mesh->getFaces().size() ==0){
        return true;
    }

    // mesh slicing step
    std::vector<std::vector<ContourSegment>> meshslices = meshSlice(mesh);


    printf("meshslice done\n");
    fflush(stdout);

    // contour construction step per layer
	std::vector<std::vector<Contour>> slicesCCCCC;
    for (int i=0; i< meshslices.size(); i++){
		auto slice = ContourGen::contourConstruct(meshslices[i]);
		slicesCCCCC.push_back(slice);
        //Slice meshslice;
        //meshslice.outershell = contourConstruct(meshslices[i]);
        //int prev_size = meshslice.outershell.size();
        //meshslice.z = scfg->layer_height*i;

        //// flaw exists if contour overlaps
        //slices->push_back(meshslice);
        //qDebug() << i << "th meshslice.outershell.size() = " << meshslice.outershell.size();

        //for (int j=0; j<meshslice.outershell.size(); j++){
        //    qDebug() << meshslice.outershell[j].size();
        //}
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
std::vector<std::vector<ContourSegment>> Slicer::Private::meshSlice(const Mesh* mesh){
    float delta = scfg->layer_height;
	std::vector<std::vector<ContourSegment>> segmentsPerPlane;
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

    for (int i=0; i<planes.size(); i++){
        std::vector<ContourSegment> planeSegments;
		auto& facesAtPlane = triangleLists.at(i);
		float currPlane = planes[i];
		for (auto face : facesAtPlane)
		{
			// compute intersection including on same line 2 points or 1 point
			ContourSegment seg = intersectionPath(face, currPlane);
			if (seg.isValid())
			{
				planeSegments.push_back(seg);
			}
			//Path intersection =
			//if (intersection.size() > 0) {
			//	paths.push_back(intersection);
			//}
		}
		segmentsPerPlane.push_back(planeSegments);
    }
    return segmentsPerPlane;
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

Contour::Contour(const ContourSegment* start)
{
	segments.push_back(*start);
	checkBound(start->from);
	checkBound(start->to);

}

bool Contour::isClosed()
{
	if (segments.size() > 1)
	{
		return segments.front().from == segments.back().to;
	}
	return false;
}

//can throw when empty
IntPoint Contour::getDestination()
{
	return segments.back().to;
}

void Contour::addNext(const ContourSegment* seg)
{
#ifdef _STRICT_SLICER
	if (getDestination() != seg->from)
		throw std::runtime_error("mismatching segment added to contour");
#endif
	segments.push_back(*seg);
	checkBound(seg->to);
}

void Contour::checkBound(const IntPoint& pt)
{
	if (_xMax < pt.X)
	{
		_xMax = pt.X;
	}
	if (_xMin > pt.X)
	{
		_xMin = pt.X;
	}

	if (_yMax < pt.Y)
	{
		_yMax = pt.Y;
	}
	if (_yMin > pt.Y)
	{
		_yMin = pt.Y;
	}
}
