#include "slicer.h"
#include <QHash>
#include <QElapsedTimer>
#include <QTextStream>
#include <unordered_map>
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

	d << "outershell: " << obj.outershell;
	d << "infill: " << obj.infill;
	d << "support: " << obj.support;
	d << "Area: " << obj.Area;
	return d;
}
QDebug Hix::Debug::operator<< (QDebug d, const Slices& obj) {
	d << "mesh: " << obj.mesh;
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
		std::vector<std::vector<Contour>> meshSlice(const Mesh* mesh); // totally k elements

		/****************** Helper Functions For Contour Construction Step *******************/
		void insertPathHash(QHash<uint32_t, Path>& pathHash, IntPoint u, IntPoint v);
		/********************** Path Generation Functions **********************/
		IntPoint toInt2DPt(const QVector2D& pt);
		Paths3D intersectionPaths(ClipperLib::Path Contour, Plane target_plane);
		Path3D intersectionPath(Plane base_plane, Plane target_plane);
		std::vector<Contour> calculateContour(const Mesh* mesh, std::vector<FaceConstItr>& intersectingFaces, float z);
	}
}
using namespace Slicer::Private;



//QVector2D Slicer::Private::midPoint2D(VertexConstItr& vtxA0, VertexConstItr& vtxA1, float z)
//{
//	float x, y, zRatio;
//	zRatio = ((z - vtxA0->position.z()) / (vtxA1->position.z() - vtxA0->position.z()));
//	x = (vtxA1->position.x() - vtxA0->position.x()) * zRatio
//		+ vtxA0->position.x();
//	y = (vtxA1->position.y() - vtxA0->position.y()) * zRatio
//		+ vtxA0->position.y();
//	return QVector2D(x, y);
//}

QVector2D ContourBuilder::midPoint2D(VertexConstItr vtxA0, VertexConstItr vtxA1)
{
	QVector2D result;
	//A0.z > A1.z
	if (vtxA0->position.z() < vtxA1->position.z())
	{
		std::swap(vtxA0, vtxA1);
	}
	auto fullEdge = std::make_pair(vtxA0, vtxA1);
	auto preCalc = _midPtLUT.find(fullEdge);
	if (preCalc == _midPtLUT.end())
	{
		float x, y, zRatio;
		zRatio = ((_plane - vtxA0->position.z()) / (vtxA1->position.z() - vtxA0->position.z()));
		x = (vtxA1->position.x() - vtxA0->position.x()) * zRatio
			+ vtxA0->position.x();
		y = (vtxA1->position.y() - vtxA0->position.y()) * zRatio
			+ vtxA0->position.y();
		result = QVector2D(x, y);
		_midPtLUT[fullEdge] = result;
	}
	else
	{
		result = preCalc->second;
	}
	return result;
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


IntPoint  Slicer::Private::toInt2DPt(const QVector2D& pt)
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


inline void rotateCW90(QVector3D& vec)
{
	//(-y,x)
	auto tmp = vec.x();
	vec.setX( vec.y());
	vec.setY(-1.0f * tmp);
}
inline void rotateCW90(QVector2D& vec)
{
	//(-y,x)
	auto tmp = vec.x();
	vec.setX(vec.y());
	vec.setY(-1.0f * tmp);
}
ContourSegment::ContourSegment()
{
}



bool ContourSegment::isValid()const
{
	return to != from;
}

float ContourSegment::dist()const
{
	return to.distanceToPoint(from);
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





bool Slicer::slice(const Mesh* mesh, Slices* slices){
    slices->mesh = mesh;

    if (mesh == nullptr || mesh->getFaces().size() ==0){
        return true;
    }
	std::vector<std::vector<Contour>> meshslices;
	try
	{
		// mesh slicing step
		meshslices = meshSlice(mesh);
	}
	catch (std::runtime_error& e)
	{
		qDebug() << e.what();
	}
	for (int i = 0; i < meshslices.size(); i++) {
		Slice meshslice;
		for (auto& contour : meshslices[i])
		{
			meshslice.outershell.push_back(contour.toPath());

		}
		meshslice.z = scfg->layer_height * i;
		slices->push_back(meshslice);

		// flaw exists if contour overlaps
		//meshslice.outerShellOffset(-(scfg->wall_thickness+scfg->nozzle_width)/2, jtRound);

	}



	slices->containmentTreeConstruct();

    return true;
}

/****************** Mesh Slicing Step *******************/

// slices mesh into segments
std::vector<std::vector<Contour>> Slicer::Private::meshSlice(const Mesh* mesh){
    float delta = scfg->layer_height;
	std::vector<std::vector<Contour>> contoursPerPlane;
    std::vector<float> planes;

    if (scfg->slicing_mode == SlicingConfiguration::SlicingMode::Uniform) {
        planes = buildUniformPlanes(mesh->z_min(), mesh->z_max(), delta);
    } 
	//else if (scfg->slicing_mode == "adaptive") {
	 //       // adaptive slice
	 //       planes = buildAdaptivePlanes(mesh->z_min(), mesh->z_max());
	 //   }

    // build triangle list per layer height
    std::vector<std::unordered_set<FaceConstItr>> planeFaces = buildTriangleLists(mesh, planes, delta);

    for (int i=0; i<planes.size(); i++){
		ContourBuilder contourBuilder(mesh, planeFaces[i], planes[i]);
		std::vector<Contour> incompleteContours;
		std::vector<Contour> contours = contourBuilder.buildContours(incompleteContours);
		contoursPerPlane.push_back(contours);

		////force close contours
		//for (auto& each : incompleteContours)
		//{
		//	each.forceClose();
		//}
		//contoursPerPlane.push_back(incompleteContours);


    }
    return contoursPerPlane;
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

std::vector<float> Slicer::buildUniformPlanes(float z_min, float z_max, float delta) {
	std::vector<float> planes;

	int idx_max = ceil((z_max - z_min) / delta);
	for (int i = 0; i < idx_max; ++i)
	{
		float plane_z = z_min + delta * i;
		planes.push_back(plane_z);
		qDebug() << "build Uniform Planes at height z " << plane_z;
	}

	return planes;
}


// builds std::vector of std::vector of triangle idxs sorted by z_min
std::vector<std::unordered_set<FaceConstItr>>  Slicer::buildTriangleLists(const Mesh* mesh, std::vector<float> planes, float delta){
	size_t faceCnt = mesh->getFaces().size();
    // Create List of list
    std::vector<std::unordered_set<FaceConstItr>> faceStates;
    for (int l_idx=0; l_idx < planes.size(); l_idx ++){
		faceStates.push_back(std::unordered_set<FaceConstItr>());
    }

    // Uniform Slicing O(n)
    if (delta>0){
		auto& faces = mesh->getFaces();
		for(auto mf = faces.cbegin(); mf != faces.cend(); ++mf)
		{
			auto sortedZ = mf->sortZ();
            float z_min = sortedZ[0];
			float z_mid = sortedZ[1];
			float z_max = sortedZ[2];
			int minIdx = (int)((z_min - planes[0]) / delta) + 1;
			int maxIdx = (int)((z_max - planes[0]) / delta);
			minIdx = std::max(0, minIdx);
			maxIdx = std::min((int)planes.size() - 1, maxIdx);
			if (minIdx > maxIdx)
				continue;
			if (z_min == z_max)
				continue;
			//just the tip~
			if (planes[minIdx] == z_min && z_min != z_mid)
			{
				++minIdx;
			}
			else if(planes[maxIdx] == z_max && z_max != z_mid)
			{
				--maxIdx;
			}
			for (int i = minIdx; i <= maxIdx; ++i)
			{
				faceStates[i].insert(mf);
			}
        }
    }
    // General Case
    else {
#ifdef _STRICT_SLICER
		throw std::runtime_error("zero delta when building triangle list for slicing");
#endif
    }
    return faceStates;
}



std::vector<float> Slicer::buildAdaptivePlanes(float z_min, float z_max){
    std::vector<float> planes;
    return planes;
}


/****************** Helper Functions For Contour Construction Step *******************/

void Slicer::Private::insertPathHash(QHash<uint32_t, Path>& pathHash, IntPoint u, IntPoint v){
    QVector3D u_qv3 = QVector3D(u.X, u.Y, 0);
    QVector3D v_qv3 = QVector3D(v.X, v.Y, 0);

    uint32_t path_hash_u = std::hash<QVector3D>()(u_qv3);
    uint32_t path_hash_v = std::hash<QVector3D>()(v_qv3);

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

//Contour::Contour(const ContourSegment* start)
//{
//	segments.push_back(*start);
//	checkBound(start->from);
//	checkBound(start->to);
//
//}
//
bool Contour::isClosed()
{
	if (segments.size() > 1)
	{
		if (segments.front().from == segments.back().to)
		{
			return true;
		}
//		else if (Utils::Math::floatAreSame(segments.front().from.x(), segments.back().to.x())
//			&& Utils::Math::floatAreSame(segments.front().from.x(), segments.back().to.x()))
//		{
//#ifdef _STRICT_SLICER
//			//make sure next segment is indeed begining segment
//			if (!segments.back().face->isNeighborOf(segments.front().face))
//			{
//				throw std::runtime_error("contour is closed but starting and ending segment faces don't meet.");
//			}
//#endif
//			return true;
//		}
	}
	return false;
}


void Contour::forceClose()
{
#ifdef _STRICT_SLICER
	if (isClosed())
	{
		throw std::runtime_error("force close on already closed contour");
	}
#endif
	ContourSegment closer;
	closer.from = segments.back().to;
	closer.to = segments.front().from;
}
////can throw when empty
//IntPoint Contour::getDestination()
//{
//	return segments.back().to;
//}
//
void Contour::addNext(const ContourSegment& seg)
{
#ifdef _STRICT_SLICER
	//if (getDestination() != seg->from)
	//	throw std::runtime_error("mismatching segment added to contour");
#endif
	segments.push_back(seg);
	//checkBound(seg->to);
}
void Contour::addPrev(const ContourSegment& seg)
{
#ifdef _STRICT_SLICER
	//if (getDestination() != seg->from)
	//	throw std::runtime_error("mismatching segment added to contour");
#endif
	segments.push_front(seg);
	//checkBound(seg->to);
}

float Contour::dist()const
{
	float totalDist = 0;
	for (auto& each : segments)
	{
		totalDist += each.dist();
	}
	return totalDist;
}

Path Contour::toPath()const
{
	Path path;
	if (!segments.empty())
	{
		path.push_back(toInt2DPt(segments.front().from));
		for (auto& each : segments)
		{
			path.push_back(toInt2DPt(each.to));
		}
	}

	return path;
}


//
//void Contour::checkBound(const IntPoint& pt)
//{
//	if (_xMax < pt.X)
//	{
//		_xMax = pt.X;
//	}
//	if (_xMin > pt.X)
//	{
//		_xMin = pt.X;
//	}
//
//	if (_yMax < pt.Y)
//	{
//		_yMax = pt.Y;
//	}
//	if (_yMin > pt.Y)
//	{
//		_yMin = pt.Y;
//	}
//}



//
//void ContourSegment::init(const QVector3D& a, const QVector3D& b, FaceConstItr& face)
//{
//	auto AInt = toInt2DPt(a);
//	auto BInt = toInt2DPt(b);
//	//points are too close, ie dist < min_resolution
//	if (AInt == BInt)
//		return;
//
//	//determine direction
//	QVector3D faceNormal = face->fn;
//	faceNormal.setZ(0.0f);
//	faceNormal.normalize();
//
//	QVector3D ABNormal = b - a;
//	rotateCCW90(ABNormal);
//	ABNormal.setZ(0.0f);
//	ABNormal.normalize();
//
//	QVector3D BANormal = a - b;
//	rotateCCW90(BANormal);
//	BANormal.setZ(0.0f);
//	BANormal.normalize();
//
//	//face normal projected over z-plane should still be normal for AB/BA vector.
//	//Now determine which vector direction is correct by comparing CCW90 or CW90 normals to projected Face normal
//	auto ABDiff = (ABNormal - faceNormal).lengthSquared();
//	auto BADiff = (BANormal - faceNormal).lengthSquared();
//
//
//#ifdef _STRICT_SLICER
//	//since projected face normal is still a normal for the AB/BA vector
//	//ABNormal == faceNormal or BANormal == faceNormal
//	//hence minimum of those two diffs should be very close to be zero
//	auto smallestDiff = std::min(ABDiff, BADiff);
//
//	if (smallestDiff > 0.01f)
//	{
//		throw std::runtime_error("Slicing: contour segment normal do not match the original face normal");
//	}
//#endif
//	//if normal is actually other way around
//	if (ABDiff < BADiff)
//	{
//		//AB is correct direction
//		to = AInt;
//		from = BInt;
//		normal = DoublePoint(ABNormal.x(), ABNormal.y());
//	}
//	else
//	{
//		//BA is correct direction
//				//AB is correct direction
//		to = BInt;
//		from = AInt;
//		normal = DoublePoint(BANormal.x(), BANormal.y());
//	}
//}
//
//
//ContourSegment::ContourSegment(VertexConstItr& vtxA, VertexConstItr& vtxB, FaceConstItr& face)
//{
//	init(vtxA->position, vtxB->position, face);
//}
//
//
//
//ContourSegment::ContourSegment(VertexConstItr& vtxA0, VertexConstItr& vtxA1, VertexConstItr& vtxB, float z, FaceConstItr& face)
//{
//
//	QVector3D A = midPoint(vtxA0, vtxA1, z);
//	init(A, vtxB->position, face);
//
//
//}
//
//ContourSegment::ContourSegment(VertexConstItr& vtxA0, VertexConstItr& vtxA1, VertexConstItr& vtxB0, VertexConstItr& vtxB1, FaceConstItr& face)
//{
//	QVector3D A = midPoint(vtxA0, vtxA1, z);
//	QVector3D B = midPoint(vtxB0, vtxB1, z);
//	init(A, B, face);
//
//}
//
bool ContourSegment::calcNormalAndFlip()
{

	//determine direction
	QVector3D faceNormal = face->fn;
	faceNormal.setZ(0.0f);
	faceNormal.normalize();

	QVector2D ABNormal = to - from;
	rotateCW90(ABNormal);
	ABNormal.normalize();

	QVector2D BANormal = from - to;
	rotateCW90(BANormal);
	BANormal.normalize();

	//face normal projected over z-plane should still be normal for AB/BA vector.
	//Now determine which vector direction is correct by comparing CCW90 or CW90 normals to projected Face normal
	auto ABDiff = (ABNormal - faceNormal).lengthSquared();
	auto BADiff = (BANormal - faceNormal).lengthSquared();


	//since projected face normal is still a normal for the AB/BA vector
	//ABNormal == faceNormal or BANormal == faceNormal
	//hence minimum of those two diffs should be very close to be zero
	auto smallestDiff = std::min(ABDiff, BADiff);

	if (smallestDiff > 0.01f)
	{
		unknownDirection = true;
		return false;
	}

	if (ABDiff < BADiff)
	{
		//AB is correct direction
		normal = ABNormal;
		return false;
	}
	else
	{
		//BA is correct direction
		auto tmp = to;
		to = from;
		from = tmp;
		normal = BANormal;
		return true;
	}
}
void ContourSegment::flip()
{
	auto tmp = to;
	to = from;
	from = tmp;
}


ContourBuilder::ContourBuilder(const Mesh* mesh, std::unordered_set<FaceConstItr>& intersectingFaces, float z)
	:_mesh(mesh), _plane(z), _intersectList(intersectingFaces)
{
}


std::variant<VertexConstItr, HalfEdgeConstItr> toHEdgeOrVtxHint(
	const FaceConstItr& mf, const std::variant<VertexConstItr, std::pair<VertexConstItr, VertexConstItr>>& edgeOrVtx)
{
	if (edgeOrVtx.index() == 0)
	{
		return std::get<0>(edgeOrVtx);
	}
	else
	{
		auto vtxPair = std::get<1>(edgeOrVtx);
		//hint is edge
		HalfEdgeConstItr nextEdge;
		if (mf->getEdgeWithVertices(nextEdge, vtxPair.first, vtxPair.second))
		{
			return nextEdge;
		}
		else
		{
			throw std::runtime_error("edge not found");
		}
	}
}

ContourSegment  ContourBuilder::calculateStartingSegment(FaceConstItr& mf,
	std::variant<VertexConstItr, HalfEdgeConstItr>& outHintTo, std::variant<VertexConstItr, HalfEdgeConstItr>& outHintFrom)
{
	ContourSegment segment;
	segment.face = mf;
	std::vector<VertexConstItr> upper;
	std::vector<VertexConstItr> middle;
	std::vector<VertexConstItr> lower;

	auto mfVertices = mf->meshVertices();
	for (int i = 0; i < 3; i++) {
		if (mfVertices[i]->position.z() > _plane) {
			upper.push_back(mfVertices[i]);
		}
		else if (mfVertices[i]->position.z() == _plane) {
			middle.push_back(mfVertices[i]);
		}
		else
			lower.push_back(mfVertices[i]);
	}
	std::vector<VertexConstItr> majority;
	std::vector<VertexConstItr> minority;
	//hints from both directions
	std::variant<VertexConstItr, std::pair<VertexConstItr, VertexConstItr>> fromHint;
	std::variant<VertexConstItr, std::pair<VertexConstItr, VertexConstItr>> toHint;
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
		auto a = midPoint2D(majority[0], minority[0]);
		auto b = midPoint2D(majority[1], minority[0]);
		segment.from = a;
		segment.to = b;
		//hint for next segment is connecting EDGE
		fromHint = std::make_pair(majority[0], minority[0]);
		toHint = std::make_pair(majority[1], minority[0]);
	}
	else {
		//1 edge interesecting, 1 vertice on the plane
		if (upper.size() == 1 && lower.size() == 1 && middle.size() == 1) {
			auto a = midPoint2D(upper[0], lower[0]);
			segment.from = a;
			segment.to = QVector2D(middle[0]->position.x(), middle[0]->position.y());
			fromHint = std::make_pair(upper[0], lower[0]);
			toHint = middle[0];
		}
		else if (middle.size() == 2) {
			segment.from = QVector2D(middle[0]->position.x(), middle[0]->position.y());
			segment.to = QVector2D(middle[1]->position.x(), middle[1]->position.y());
			fromHint = middle[0];
			toHint = middle[1];

		}
		//face == plane
	}
	//hint needs to be in correct direction
	bool isFlipped = segment.calcNormalAndFlip();
	if (isFlipped)
	{
		auto tmp = toHint;
		toHint = fromHint;
		fromHint = tmp;
	}
	outHintTo = toHEdgeOrVtxHint(mf, toHint);
	outHintFrom = toHEdgeOrVtxHint(mf, fromHint);

	return segment;
}

std::vector<Contour> ContourBuilder::buildContours(std::vector<Contour>& incompleteContours)
{
	auto faces = _mesh->getFaces();
	std::vector<Contour> contours;

	//when calculating intersect normals 
	//std::unordered_map<FaceConstItr, ContourSegment> tooShortFaces;
	std::vector<FaceConstItr> startCandidates(_intersectList.begin(), _intersectList.end());
	auto startingFace = startCandidates.begin();

	while (!_intersectList.empty())
	{
		while (_intersectList.find(*startingFace) == _intersectList.end())
		{
			++startingFace;
			if (startingFace == startCandidates.end())
			{
				//valid starting candidates ran out.
						//only very tiny segs remain....
#ifdef _STRICT_SLICER
				if (!_intersectList.empty())
				{
					float sum;
					std::variant<VertexConstItr, HalfEdgeConstItr> hint0, hint1;
					for (auto each : _intersectList)
					{
						auto seg = calculateStartingSegment(each, hint0, hint1);
						sum += seg.dist();
					}
					if (sum > 0.001)
					{
						throw std::runtime_error("tiny segments should not be ignored.");
					}
				}
#endif
				return contours;
			}
		}

		Contour currContour;
		//no dead-end or hole yet, so don't reverse
		//add first segment
		ContourSegment currSeg, prevSeg;
		//hint for next seg
		std::variant<VertexConstItr, HalfEdgeConstItr> hint;
		std::variant<VertexConstItr, HalfEdgeConstItr> toHint;
		std::variant<VertexConstItr, HalfEdgeConstItr> fromHint;

		currSeg = calculateStartingSegment(*startingFace, toHint, fromHint);
		if (currSeg.unknownDirection)
		{
			++startingFace;
			continue;
		}
		//forward direction initially
		hint = toHint;
		_reverse = false;
		_intersectList.erase(*startingFace);
		do
		{
			if (currSeg.isValid())
			{
				if (_reverse)
				{
					currContour.addPrev(currSeg);
				}
				else
				{
					currContour.addNext(currSeg);
				}
				prevSeg = currSeg;
			}
			else
			{
				if (_reverse)
				{
					//dead end found on both end, leave the contour incomplete.
					incompleteContours.push_back(currContour);
					break;
				}
				else
				{
					//dead end found on the tail end, so reverse direction
					_reverse = true;
					hint = fromHint;
					prevSeg = currContour.segments.front();

				}
			}
			if (hint.index() == 0)
			{
				//vertex
				currSeg = doNextSeg(std::get<0>(hint), prevSeg, hint);
			}
			else
			{
				//edge
				currSeg = doNextSeg(std::get<1>(hint), prevSeg, hint);
			}
		}
		while (!currContour.isClosed());
		//if a complete contour is found, no dead ends
		if (!_reverse)
		{
			contours.push_back(currContour);
		}

	}

	//link un-closed contours together if possible to minimize un-closed contour counts
	if (!incompleteContours.empty())
	{
		for (auto& each : incompleteContours)
		{
			auto beginFace = each.segments.front().face;
			auto lastFace = each.segments.back().face;
			qDebug() << "incomplete contour found:";
			qDebug() << "starting face: \n" << beginFace;
			qDebug() << "last face:  \n" << lastFace;
		}
	}

	//repair remaining un-closed contorus
	return contours;
}


//at this point we know which face interesects, and which don't
ContourSegment ContourBuilder::doNextSeg(VertexConstItr from, const ContourSegment& prevSeg, std::variant<VertexConstItr, HalfEdgeConstItr>& to)
{
	ContourSegment newSeg;
	newSeg.from = QVector2D(from->position.x(), from->position.y());
	FaceConstItr curr;
	auto connectedFaces = from->connectedFaces();
	std::unordered_set<FaceConstItr> candidates;
	std::unordered_set<FaceConstItr> pool;
		
	for (auto each : connectedFaces)
	{
		pool.insert(each);
		if (_intersectList.find(each) != _intersectList.end())
		{
			candidates.insert(each);
		}
	}
	if (candidates.size() > 1)
	{
		bool intersectFound = Hix::Engine3D::findCommonManifoldFace(curr, prevSeg.face, candidates, pool);
		if (!intersectFound)
		{
			qDebug() << "no proper intersecting face found from contour builder";
			return ContourSegment();

		}
	}
	else
	{
		curr = *candidates.begin();
	}


	//set current intersecting face as explored
	_intersectList.erase(curr);
	newSeg.face = curr;

	VertexConstItr upper;
	VertexConstItr mid;
	VertexConstItr lower;
	bool isEdge = true;
	auto mfVertices = curr->meshVertices();
	for (int i = 0; i < 3; i++) {
		if (mfVertices[i]->position.z() > _plane) {
			upper = mfVertices[i];
		}
		else if (mfVertices[i]->position.z() == _plane) {
			if (mfVertices[i] != from)
			{
				mid = mfVertices[i];
				isEdge = false;
				break;
			}
		}
		else
			lower = mfVertices[i];
	}

	if (isEdge)
	{
		newSeg.to = midPoint2D(upper, lower);
		HalfEdgeConstItr nextEdge;
		if (curr->getEdgeWithVertices(nextEdge, upper, lower))
		{
			to = nextEdge;
		}
		else
		{
			throw std::runtime_error("edge not found");
		}
	}
	else
	{
		newSeg.to = QVector2D(mid->position.x(), mid->position.y());
		to = mid;

	}
	if (_reverse)
		newSeg.flip();
	return newSeg;
}


ContourSegment ContourBuilder::doNextSeg(HalfEdgeConstItr from, const ContourSegment& prevSeg, std::variant<VertexConstItr, HalfEdgeConstItr>& to)
{
	ContourSegment newSeg;
	FaceConstItr curr;
	if (_reverse)
	{
		newSeg.from = prevSeg.from;
	}
	else
	{
		newSeg.from = prevSeg.to;
	}
	auto neighborFaces = from->neighborFaces();
	bool nextFound = false;
	for (auto twin : neighborFaces)
	{
		if (_intersectList.find(twin) != _intersectList.end())
		{
			curr = twin;
			nextFound = true;
			break;
		}
	}
	if (!nextFound)
	{
		qDebug() << "incomplete contour during slicing";
		return ContourSegment();
	}
	newSeg.face = curr;
	//set current intersecting face as explored
	_intersectList.erase(curr);
	VertexConstItr fromEdgeUpper;
	VertexConstItr fromEdgeLower;
	if (from->from->position.z() < from->to->position.z())
	{
		fromEdgeUpper = from->to;
		fromEdgeLower = from->from;
	}
	else
	{
		fromEdgeUpper = from->from;
		fromEdgeLower = from->to;
	}
	VertexConstItr upper = fromEdgeUpper;
	VertexConstItr lower = fromEdgeLower;
	VertexConstItr mid;
	bool isEdge = true;
	auto mfVertices = curr->meshVertices();
	for (int i = 0; i < 3; i++) {
		if (mfVertices[i]->position.z() > _plane) {
			if (mfVertices[i] != fromEdgeUpper)
			{
				upper = mfVertices[i];
			}
		}
		else if (mfVertices[i]->position.z() == _plane) {
			mid = mfVertices[i];
			isEdge = false;
			break;
		}
		else
		{
			if (mfVertices[i] != fromEdgeLower)
			{
				lower = mfVertices[i];
			}
		}
	}

	if (isEdge)
	{
		newSeg.to = midPoint2D(upper, lower);
		HalfEdgeConstItr nextEdge;
		if (curr->getEdgeWithVertices(nextEdge, upper, lower))
		{
			to = nextEdge;
		}
		else
		{
			throw std::runtime_error("edge not found");
		}
	}
	else
	{
		newSeg.to = QVector2D(mid->position.x(), mid->position.y());
		to = mid;
	}
	if (_reverse)
		newSeg.flip();
	return newSeg;
}