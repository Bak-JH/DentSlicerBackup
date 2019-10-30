#include "Shapes2D.h"
#include "../DentEngine/src/ContourBuilder.h"
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


std::vector<QVector3D> Hix::Shapes2D::generateSquare(float radius)
{
	std::vector<QVector3D>square;
	square.reserve(4);
	square.emplace_back(QVector3D(radius, radius, 0));
	square.emplace_back(QVector3D(radius, -radius, 0));
	square.emplace_back(QVector3D(-radius, -radius, 0));
	square.emplace_back(QVector3D(-radius, radius, 0));
	return square;
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


void Hix::Shapes2D::scaleContourVtxNormal(std::vector<QVector3D>& targetContour, float scale)
{
	auto inputContour = targetContour;
	auto size = targetContour.size();
	if (size < 3)
		return;
	for (size_t i = 0; i < size; ++i)
	{
		const auto& a = inputContour[i];
		const auto& b = inputContour[(i + 1) % size];
		const auto& c = inputContour[(i + 2) % size];

		auto ab = b - a;
		auto bc = c - b;
		QVector3D abNorm(ab.y(), -ab.x(), 0);
		QVector3D bcNorm(bc.y(), -bc.x(), 0);
		abNorm.normalize();
		bcNorm.normalize();
		auto vtxNormal = abNorm + bcNorm;
		vtxNormal.normalize();
		targetContour[(i + 1) % size] += scale * vtxNormal;
	}
}

bool Hix::Shapes2D::isClockwise(const std::vector<QVector3D>& contour)
{
	if (contour.size() < 2)
		return true;
	QVector3D prevPt = contour.back();
	double sum = 0;
	for (auto& pt : contour)
	{
		sum += (pt.x() - prevPt.x()) * (pt.y() + prevPt.y());
		prevPt = pt;
	}
	return sum >= 0;
}

bool Hix::Shapes2D::isClockwise(const Hix::Slicer::Contour& contour)
{
	if (contour.segments.size() < 2)
		return true;
	QVector3D prevPt = contour.segments.back().to;
	double sum = 0;
	for (auto& pt : contour.segments)
	{
		sum += (pt.to.x() - prevPt.x()) * (pt.to.y() + prevPt.y());
		prevPt = pt.to;
	}
	return sum >= 0;
}

void Hix::Shapes2D::ensureOrientation(bool isClockwise, std::vector<QVector3D>& contour)
{
	if (Hix::Shapes2D::isClockwise(contour) == isClockwise)
		return;
	std::reverse(contour.begin(), contour.end());
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
