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

	d << "closedContours: " << obj.closedContours;

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









float minDistanceToContour(QVector3D from, ClipperLib::Path contour) {
	float min_distance = 0;
	for (int i = 0; i < contour.size() - 1; i++) {
		ClipperLib::Path temp_path;
		temp_path.push_back(contour[i]);
		temp_path.push_back(contour[i + 1]);
		QVector3D int2qv3 = QVector3D(((float)contour[i].X) / Hix::Polyclipping::INT_PT_RESOLUTION, ((float)contour[i].Y) / Hix::Polyclipping::INT_PT_RESOLUTION, from.z());
		IntPoint directionInt = contour[i + 1] - contour[i];
		QVector3D direction = QVector3D(directionInt.X / Hix::Polyclipping::INT_PT_RESOLUTION, directionInt.Y / Hix::Polyclipping::INT_PT_RESOLUTION, 0);
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







void Hix::Slicer::slice(const Hix::Render::SceneEntity& entity, const Planes* planes, Slices* slices){
    
	for (auto childNode : entity.childNodes())
	{
		auto sliceableModel = dynamic_cast<const Hix::Render::SceneEntity*>(childNode);
		if (sliceableModel)
		{
			slice(*sliceableModel, planes, slices);
		}
	}
	auto mesh = entity.getMesh();
	if (mesh->getFaces().size() ==0){
        return;
    }
	auto zPlanes = planes->getPlanesVector();
	auto intersectingFaces = planes->buildTriangleLists(mesh);
	for (int i = 0; i < zPlanes.size(); i++) {
		auto& currSlice = (*slices)[i];
		ContourBuilder contourBuilder(mesh, intersectingFaces[i], zPlanes[i]);
		auto contours = contourBuilder.buildContours();
		for (auto& each : contours)
		{
			currSlice.closedContours.push_back(each.toPath());
		}
	}
    return;
}

/****************** Mesh Slicing Step *******************/




Hix::Slicer::Slices::Slices(size_t size): std::vector<Slice>(size)
{
}

/****************** Deprecated functions *******************/
void Slices::containmentTreeConstruct(){
    Clipper clpr;
    for (int idx=0; idx<this->size(); idx++){ // divide into parallel threads
        Slice* slice = &((*this)[idx]);
			clpr.Clear();
        clpr.AddPaths(slice->closedContours, ptSubject, true);
        clpr.Execute(ctUnion, slice->polytree, pftNonZero, pftNonZero);
    }
}

