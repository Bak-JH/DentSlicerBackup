#include "slicer.h"
#include <QHash>
#include <QElapsedTimer>
#include <QTextStream>
#include <unordered_map>
#include "../../utils/mathutils.h"
#include "polyclipping/QDebugPolyclipping.h"
#include "ContourBuilder.h"
#include "Planes.h"
#include "SlicerDebug.h"
#include "SlicerDebugInfoExport.h"
#if defined(_DEBUG) || defined(QT_DEBUG )
#define _STRICT_SLICER
//#define _STRICT_MESH_NO_SELF_INTERSECTION
#endif
using namespace ClipperLib;
using namespace Hix::Debug;
using namespace Hix::Slicer;
QDebug Hix::Debug::operator<< (QDebug d, const Slice& obj) {
	d << "polytree: " << *obj.polytree;

	return d;
}
QDebug Hix::Debug::operator<< (QDebug d, const std::vector<Slice>& obj) {
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







std::vector<LayerGroup> Hix::Slicer::slice(const Hix::Render::SceneEntity& entity, const Planes& planes)
{
	std::vector<LayerGroup> layers(planes.getPlanesVector().size());
	auto mesh = entity.getMesh();
	if (mesh->getFaces().size() ==0){
        return layers;
    }

	auto zPlanes = planes.getPlanesVector();
	auto intersectingFaces = planes.buildTriangleLists(mesh);
	for (int i = 0; i < zPlanes.size(); i++) {
		qDebug() << "slicing: " << i;
		auto& currLayer = layers[i];
		auto& singleSlice = currLayer.slices.emplace_back();
		ContourBuilder contourBuilder(mesh, intersectingFaces[i], zPlanes[i]);
		auto contours = contourBuilder.buildContours();
		std::move(contours.begin(), contours.end(), std::back_inserter(singleSlice.closedContours));
		if (Hix::Slicer::Debug::SlicerDebug::getInstance().enableDebug)
		{
			auto incompleteContours = contourBuilder.flushIncompleteContours();
			std::move(incompleteContours.begin(), incompleteContours.end(), std::back_inserter(singleSlice.incompleteContours));
		}
	}
    return layers;
}




void Hix::Slicer::containmentTreeConstruct(const Slice& slice){
	Clipper clpr;
	clpr.StrictlySimple(true);
	for (auto& each : slice.closedContours)
	{
		clpr.AddPath(each.toPath(), ptSubject, true);
	}
    clpr.Execute(ctUnion, *slice.polytree, pftNonZero, pftNonZero);
}

Hix::Slicer::Slice::Slice():polytree(new PolyTree())
{
}

bool Hix::Slicer::LayerGroup::empty() const
{
	size_t cnt = 0;
	for (auto& s : slices)
	{
		cnt += s.polytree->ChildCount() + s.polytree->Contour.size();
	}
	return cnt == 0;
}
