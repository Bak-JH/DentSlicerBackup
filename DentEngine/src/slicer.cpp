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

