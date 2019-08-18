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
		std::vector<Contour> contours = contourBuilder.buildContours();
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

void Slice::getOverhang(const Slice* prevSlice, PolyTree& result)const
{
	if (prevSlice != nullptr)
	{
		Clipper clpr;
		clpr.AddPaths(outershell, ptSubject, true);
		clpr.AddPaths(prevSlice->outershell, ptClip, true);
		clpr.Execute(ctDifference, result, pftNonZero, pftNonZero);
	}
}

void Slice::getOverhangMN(const Slice* prevSlice, std::unordered_set<const PolyNode*>& result)const
{
	//auto prvSliceNode = dynamic_cast<PolyNode*>(prevSlice->outershell);
	//if (prevSlice != nullptr)
	//{


	//}
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
        clpr.Execute(ctUnion, slice->polytree, pftNonZero, pftNonZero);
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
bool Contour::isClosed()const
{
	if (segments.front().from == segments.back().to)
	{
		return true;
	}
	return false;
}


bool Contour::tryClose()
{
#ifdef _STRICT_SLICER
	if (isClosed())
	{
		throw std::runtime_error("force close on already closed contour");
	}
#endif
	if (segments.size() < 2)
		return false;
	float totalDist = dist();
	float gap = segments.front().from.distanceToPoint(segments.back().to);
	if (gap < std::numeric_limits<float>::epsilon() || gap < totalDist / 5)
	{
		ContourSegment closer;
		closer.from = segments.back().to;
		closer.to = segments.front().from;
		addNext(closer);
#ifdef _STRICT_SLICER
		if (!isClosed())
		{
			throw std::runtime_error("force close failed");
		}
#endif
		return true;
	}
	return false;
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

QVector2D Contour::from()const
{
	return segments.front().from;
}
QVector2D Contour::to()const
{
	return segments.back().to;
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
void Contour::append(const Contour& appended)
{
#ifdef _STRICT_SLICER
	if (to() != appended.from())
	{
		throw std::runtime_error("trying to append two disjoint contours");
	}
#endif
	for (auto each : appended.segments)
	{
		addNext(each);
	}
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
ContourSegment::FlipResult ContourSegment::calcNormalAndFlip()
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
		return FlipResult::UnknownDirection;
	}

	if (ABDiff < BADiff)
	{
		//AB is correct direction
		normal = ABNormal;
		return FlipResult::NotFlipped;
	}
	else
	{
		//BA is correct direction
		auto tmp = to;
		to = from;
		from = tmp;
		normal = BANormal;
		return FlipResult::Flipped;
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

void ContourBuilder::buildSegment(const FaceConstItr& mf)
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

	}
	else {
		//1 edge interesecting, 1 vertice on the plane
		if (upper.size() == 1 && lower.size() == 1 && middle.size() == 1) {
			auto a = midPoint2D(upper[0], lower[0]);
			segment.from = a;
			segment.to = QVector2D(middle[0]->position.x(), middle[0]->position.y());
		}
		else if (middle.size() == 2) {
			segment.from = QVector2D(middle[0]->position.x(), middle[0]->position.y());
			segment.to = QVector2D(middle[1]->position.x(), middle[1]->position.y());

		}
		//face == plane
	}
	//hint needs to be in correct direction
	auto flipResult = segment.calcNormalAndFlip();

	//insert to container
	_segments[mf] = segment;
	auto& newSeg = _segments[mf];


	//segment is too small and direction cannot be determinied
	if (flipResult == ContourSegment::FlipResult::UnknownDirection)
	{
		_unknownHash.insert(std::make_pair(newSeg.from, &newSeg));
		_unknownHash.insert(std::make_pair(newSeg.to, &newSeg));
	}
	else
	{
		_fromHash.insert(std::make_pair(newSeg.from, &newSeg));
		_toHash.insert(std::make_pair(newSeg.to, &newSeg));
	}


}

std::vector<Contour> ContourBuilder::buildContours()
{
	auto faces = _mesh->getFaces();
	std::vector<Contour> contours;
	_incompleteContours.clear();
	_unexplored = _intersectList;

	//create segments
	for (const auto& each : _intersectList)
	{
		buildSegment(each);
	}
	//remove segments of unknown direction from stating segment candidates.
	for (auto each : _unknownSegs)
	{
		_unexplored.erase(each->face);
	}

	while (!_unexplored.empty())
	{

		Contour currContour;
		//add first segment, pop it from unexplored
		ContourSegment& firstSeg = _segments[*_unexplored.begin()];
		currContour.addNext(firstSeg);
		_unexplored.erase(_unexplored.begin());


		//add rest
		uint8_t deadEndCnt = 0;
		while (deadEndCnt != 2)
		{
			if (!buildContour(currContour, deadEndCnt == 1))
			{
				++deadEndCnt;
			}
			//if contour is complete
			if (currContour.isClosed())
			{
				break;
			}
		}
		//if unclosed contour
		if (deadEndCnt == 2)
		{
			_incompleteContours.push_back(std::move(currContour));
		}
		else
		{
			if (isArea(currContour))
			{
				contours.push_back(std::move(currContour));
			}
		}


	}
	//link un-closed contours together if possible to minimize un-closed contour counts
	if (!_incompleteContours.empty())
	{

		auto closedContours = joinOrCloseIncompleteContours();
		for (auto each : closedContours)
		{
			if (isArea(*each))
			{
				contours.push_back(*each);
			}
		}

	}

	//repair remaining un-closed contorus
	return contours;
}
bool ContourBuilder::isArea(const Contour& contour)
{
	if (contour.segments.size() > 2)
		return true;
	else
		return false;
}



bool DFSIncompleteContour(std::unordered_set<Contour*>& unusedContours, Contour* start,
	const std::unordered_multimap<QVector2D, Contour*>& map)
{
	std::unordered_map<Contour*, Contour*> traverseMap;
	std::unordered_set<Contour*> explored;
	bool success = false;
	std::vector<Contour*> s;
	s.push_back(start);
	Contour* current;
	while (!s.empty())
	{
		current = s.back();
		s.pop_back();
		if (explored.find(current) == explored.end())
		{
			explored.insert(current);
			if (current->to() == start->from())
			{
				success = true;
				break;
			}
			else
			{
				auto childrenRange =  map.equal_range(current->to());
				for (auto it = childrenRange.first; it != childrenRange.second; ++it) {
					traverseMap[it->second] = current;
					s.push_back(it->second);
				}
			}
		}
	}

	if (success)
	{
		std::vector<Contour*> rPath;
		rPath.push_back(current);
		auto parent = traverseMap.find(current);
		if (!traverseMap.empty())
		{
			//backtrace DFS to get the stitched contour -now closed.
			do
			{
				rPath.push_back(parent->second);
				parent = traverseMap.find(parent->second);


			} while (parent != traverseMap.end());
		}
			
		//remove start contour form rPath
		rPath.pop_back();

		//append paths
		for (auto rItr = rPath.crbegin(); rItr != rPath.crend(); ++rItr)
		{
			start->append(**rItr);
			unusedContours.erase(*rItr);
		}
		//usually this happens when contour is made of one very small segment
		if (!start->isClosed())
		{
			return false;
		}
	}
	return success;


}

std::unordered_set<Contour*> ContourBuilder::joinOrCloseIncompleteContours()
{
	std::unordered_set<Contour*> remainingContours;
	std::unordered_set<Contour*> unjoinableContours;
	std::unordered_set<Contour*> closedContours;

	std::unordered_multimap<QVector2D, Contour*> map;
	for (auto& each : _incompleteContours)
	{
		remainingContours.insert(&each);
		closedContours.insert(&each);
		map.insert(std::make_pair(each.from(), &each));
	}
	while (!remainingContours.empty())
	{
		auto current = *remainingContours.begin();
		remainingContours.erase(current);
		bool stitchSuccess = DFSIncompleteContour(remainingContours, current, map);
		if (!stitchSuccess)
		{
			unjoinableContours.insert(current);
		}
	}
	//attempt to close unjoinable contours
	size_t i = 0;
	for (auto& each : unjoinableContours)
	{
		if (!each->tryClose())
		{
			++i;
			closedContours.erase(each);
		}
	}
	if (i != 0)
	{
		qDebug() << "unfixable contours";
	}
	return closedContours;
}

bool ContourBuilder::buildContour(Contour& current, bool reverse)
{
	std::unordered_multimap<QVector2D, ContourSegment*>* map;
	std::unordered_multimap<QVector2D, ContourSegment*>* oppMap;

	QVector2D from;
	if (reverse)
	{
		map = &_toHash;
		oppMap = &_fromHash;
		from = current.from();
	}
	else
	{
		map = &_fromHash;
		oppMap = &_toHash;
		from = current.to();
	}
	ContourSegment* next = nullptr;
	auto oppRange = oppMap->equal_range(from);
	auto range = map->equal_range(from);
	//range is of size 1
	if (range.first != range.second && range.first == (--range.second)
		&& oppRange.first != oppRange.second && oppRange.first == (--oppRange.second))
	{
		next = range.first->second;
		if (reverse)
		{
			current.addPrev(*next);
		}
		else
		{
			current.addNext(*next);
		}
		//mark explored
		_unexplored.erase(next->face);
		return true;
	}
	return false;
}
