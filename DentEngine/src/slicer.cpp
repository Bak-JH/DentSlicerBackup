#include "slicer.h"
#include <QHash>
#include <QElapsedTimer>
#include <QTextStream>
#include <unordered_map>
#include "../../utils/mathutils.h"
#include "polyclipping/QDebugPolyclipping.h"
#include "configuration.h"
#include "ContourBuilder.h"
#include "Planes.h"
#if defined(_DEBUG) || defined(QT_DEBUG )
#define _STRICT_SLICER
//#define _STRICT_MESH_NO_SELF_INTERSECTION
#endif
using namespace ClipperLib;
using namespace Hix::Debug;
using namespace Hix::Slicer;
QDebug Hix::Debug::operator<< (QDebug d, const Slice& obj) {
	d << "z: " << obj.z;
	d << "polytree: " << obj.polytree;

	d << "outershell: " << obj.outershell;
	return d;
}
QDebug Hix::Debug::operator<< (QDebug d, const Slices& obj) {
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

		/****************** Helper Functions For Contour Construction Step *******************/
		void insertPathHash(QHash<uint32_t, Path>& pathHash, IntPoint u, IntPoint v);
		/********************** Path Generation Functions **********************/
		Path3D intersectionPath(Plane base_plane, Plane target_plane);
	}
}
using namespace Slicer::Private;



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





void Hix::Slicer::slice(const Mesh* mesh, const Planes* planes, Slices* slices){
    if (mesh->getFaces().size() ==0){
        return;
    }
	auto zPlanes = planes->getPlanesVector();
	auto intersectingFaces = planes->buildTriangleLists(mesh);
	for (int i = 0; i < zPlanes.size(); i++) {
		ContourBuilder contourBuilder(mesh, intersectingFaces[i], zPlanes[i]);
		std::vector<Contour> contours = contourBuilder.buildContours();
		if (!contours.empty())
		{
			auto& currSlice = (*slices)[i];
			for (auto& each : contours)
			{
				currSlice.outershell.push_back(each.toPath());
			}
		}
	}
    return;
}

/****************** Mesh Slicing Step *******************/




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


Hix::Slicer::Slices::Slices(size_t size): std::vector<Slice>(size)
{
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
}

