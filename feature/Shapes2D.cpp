#include "Shapes2D.h"
using namespace ClipperLib;
std::vector<QVector3D> Hix::Shapes2D::generateHexagon(float radius)
{
	std::vector<QVector3D>hexagon;
	hexagon.reserve(6);

	constexpr  QVector3D normal(0, 0, 1);
	auto rot = QQuaternion::fromAxisAndAngle(normal, 60);
	auto pt = QVector3D(radius, 0, 0);

	for (size_t i = 0; i < 6; ++i)
	{

		hexagon.emplace_back(pt);
		pt = rot.rotatedVector(pt);
	}
	return hexagon;
}





std::vector<ClipperLib::Path> Hix::Shapes2D::combineContour(const std::vector<std::vector<QVector3D>>& contours)
{
	std::vector<ClipperLib::Path> result;
	ClipperLib::Clipper clipper;
	std::vector<Path> clPaths;
	clPaths.reserve(contours.size());
	for (auto& each : contours)
	{
		clPaths.emplace_back(Hix::Polyclipping::toCLPath(each));
	}
	for (auto& each : clPaths)
	{
		clipper.AddPath(each, ptSubject, true);

	}
	ClipperLib::PolyTree tree;
	clipper.Execute(ctUnion, tree, pftNonZero, pftNonZero);

	if (!tree.Childs.empty())
	{
		for (auto& child : tree.Childs)
		{
			result.emplace_back(child->Contour);

		}
	}
	return result;
}



void Hix::Shapes2D::moveContour(std::vector<QVector3D>& targetContour, const QVector3D& moveVector)
{
	for (auto& each : targetContour)
	{
		each += moveVector;
	}
}

void Hix::Shapes2D::rotateContour(std::vector<QVector3D>& targetContour, const QQuaternion & rotation)
{
	for (auto& each : targetContour)
	{
		each = rotation.rotatedVector(each);
	}
}

void Hix::Shapes2D::scaleContour(std::vector<QVector3D>& targetContour, float scale)
{
	for (auto& each : targetContour)
	{
		each *= scale;
	}
}


void Hix::Shapes2D::scaleContourAroundCentoid(std::vector<QVector3D>& targetContour, float scale, QVector2D& centoid)
{
	for (auto& each : targetContour)
	{
		each -= centoid;
	}
	scaleContour(targetContour, scale);
	for (auto& each : targetContour)
	{
		each += centoid;
	}
}
