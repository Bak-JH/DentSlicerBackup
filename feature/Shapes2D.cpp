#include "Shapes2D.h"
#include "../DentEngine/src/ContourBuilder.h"

using namespace ClipperLib;



std::vector<std::vector<QVector3D>> Hix::Shapes2D::gridCircle(float radius, float offset)
{
	size_t singleAxisPositive = (size_t)(radius / offset);
	size_t lineCnt = 1 + (singleAxisPositive * 2);
	lineCnt *= 2;
	std::vector<std::vector<QVector3D>> grid;
	grid.reserve(lineCnt);
	float radiusSqrd = std::pow(radius, 2);
	std::vector<float> otherVal;
	std::vector<float> offVal;
	otherVal.reserve(singleAxisPositive);
	offVal.reserve(singleAxisPositive);
	float currOff = offset;
	for (size_t i = 0; i < singleAxisPositive; ++i)
	{
		otherVal.emplace_back(std::sqrt(radiusSqrd - std::pow(currOff, 2)));
		offVal.emplace_back(currOff);
		currOff += offset;
	}
	//x axis, y axis
	grid.push_back({ QVector3D(0, radius, 0), QVector3D(0, -radius, 0) });
	grid.push_back({ QVector3D(radius, 0, 0), QVector3D(-radius, 0, 0) });
	//-x axis
	for (size_t i = 0; i < singleAxisPositive; ++i)
	{
		std::vector<QVector3D> line;
		line.reserve(2);
		line.emplace_back(QVector3D(-offVal[i], -otherVal[i], 0));
		line.emplace_back(QVector3D(-offVal[i], otherVal[i], 0));
		grid.emplace_back(std::move(line));
	}
	//+x axis
	for (size_t i = 0; i < singleAxisPositive; ++i)
	{
		std::vector<QVector3D> line;
		line.reserve(2);
		line.emplace_back(QVector3D(offVal[i], -otherVal[i], 0));
		line.emplace_back(QVector3D(offVal[i], otherVal[i], 0));
		grid.emplace_back(std::move(line));
	}
	//-y axis
	for (size_t i = 0; i < singleAxisPositive; ++i)
	{
		std::vector<QVector3D> line;
		line.reserve(2);
		line.emplace_back(QVector3D(-otherVal[i], -offVal[i], 0));
		line.emplace_back(QVector3D(otherVal[i], -offVal[i], 0));
		grid.emplace_back(std::move(line));
	}
	//+y axis
	for (size_t i = 0; i < singleAxisPositive; ++i)
	{
		std::vector<QVector3D> line;
		line.reserve(2);
		line.emplace_back(QVector3D(-otherVal[i], offVal[i], 0));
		line.emplace_back(QVector3D(otherVal[i], offVal[i], 0));
		grid.emplace_back(std::move(line));
	}
	return grid;
}
std::vector<std::vector<QVector3D>> Hix::Shapes2D::gridRect(float xLength, float yLength, float offset)
{
	float xRad = xLength / 2.0f;
	float yRad = yLength / 2.0f;
	//cnt for positive axis
	size_t xLineCnt = ((size_t)(xRad / offset));
	size_t yLineCnt = ((size_t)(yRad / offset));
	size_t lineCnt = xLineCnt*2 + yLineCnt*2 + 2;
	std::vector<std::vector<QVector3D>> grid;
	grid.reserve(lineCnt);
	//x axis, y axis
	grid.push_back({ QVector3D(0, yRad, 0), QVector3D(0, -yRad, 0) });
	grid.push_back({ QVector3D(xRad, 0, 0), QVector3D(-xRad, 0, 0) });
	//x axis
	for (size_t i = 1; i < xLineCnt + 1; ++i)
	{
		float x = i * offset;
		std::vector<QVector3D> line0;
		line0.reserve(2);
		line0.emplace_back(QVector3D(x, -yRad, 0));
		line0.emplace_back(QVector3D(x, yRad, 0));
		grid.emplace_back(std::move(line0));
		std::vector<QVector3D> line1;
		line1.reserve(2);
		line1.emplace_back(QVector3D(-x, -yRad, 0));
		line1.emplace_back(QVector3D(-x, yRad, 0));
		grid.emplace_back(std::move(line1));
	}
	//y axis
	for (size_t i = 1; i < yLineCnt + 1; ++i)
	{
		float y = i * offset;
		std::vector<QVector3D> line0;
		line0.reserve(2);
		line0.emplace_back(QVector3D(-xRad, y, 0));
		line0.emplace_back(QVector3D(xRad, y, 0));
		grid.emplace_back(std::move(line0));
		std::vector<QVector3D> line1;
		line1.reserve(2);
		line1.emplace_back(QVector3D(-xRad, -y, 0));
		line1.emplace_back(QVector3D(xRad,  -y, 0));
		grid.emplace_back(std::move(line1));
	}
	return grid;
}
void Hix::Shapes2D::rotateCW90(QVector2D& vec)
{
	//(y,-x)
	auto tmp = vec.x();
	vec.setX(vec.y());
	vec.setY(-tmp);
}
void Hix::Shapes2D::rotateCCW90(QVector2D& vec)
{
	//(-y,x)
	auto tmp = vec.x();
	vec.setX(-vec.y());
	vec.setY(tmp);
}


std::vector<QVector3D> Hix::Shapes2D::generateCircle(float radius, size_t segCount)
{

	std::vector<QVector3D>circle;
	circle.reserve(segCount);

	constexpr  QVector3D normal(0, 0, 1);
	float arcAngle = 360.0f / (float)segCount;
	auto rot = QQuaternion::fromAxisAndAngle(normal, arcAngle);
	auto pt = QVector3D(radius, 0, 0);

	for (size_t i = 0; i < segCount; ++i)
	{

		circle.emplace_back(pt);
		pt = rot.rotatedVector(pt);
		pt.setZ(0);
	}
	return circle;
}

std::vector<QVector3D> Hix::Shapes2D::generateHexagon(float radius)
{

	return generateCircle(radius, 6);
}


void Hix::Shapes2D::circleToTri(Hix::Engine3D::Mesh* supportMesh, const std::vector<QVector3D>& endCapOutline, const QVector3D& center, bool oppDir)
{
	size_t seg = endCapOutline.size();
	if (oppDir)
	{
		for (size_t i = 0; i < seg; ++i)
		{
			supportMesh->addFace(center, endCapOutline[(i + 1) % seg], endCapOutline[i % seg]);
		}
	}
	else
	{
		for (size_t i = 0; i < seg; ++i)
		{
			supportMesh->addFace(center, endCapOutline[i % seg], endCapOutline[(i + 1) % seg]);
		}
	}

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

std::vector<QVector2D> Hix::Shapes2D::to2DShape(const std::vector<QVector3D>& input)
{
	std::vector<QVector2D>output;
	output.reserve(input.size());
	std::transform(std::begin(input), std::end(input), std::back_inserter(output),
		[](const QVector3D& each)-> QVector2D {
			return QVector2D(each);
		});
	return output;
}

std::vector<QVector3D> Hix::Shapes2D::to3DShape(float zPos, const std::vector<QVector2D>& input)
{
	std::vector<QVector3D>output;
	output.reserve(input.size());
	std::transform(std::begin(input), std::end(input), std::back_inserter(output),
		[zPos](const QVector2D& each)-> QVector3D {
			return QVector3D(each, zPos);
		});
	return output;
}

std::vector<QVector2D> Hix::Shapes2D::PolylineToArea(float thickness, const std::vector<QVector2D>& polyline)
{
	std::vector<QVector2D> areaContour;
	if (polyline.size() > 1)
	{
		auto radius = thickness / 2.0f;
		auto vtcsCnt = polyline.size();
		areaContour.reserve(vtcsCnt * 2);
		//same code as extrude, but 2d...TODO: templateize 
		//compute segments
		std::vector<QVector2D> segs;
		segs.reserve(vtcsCnt - 1);
		auto lastPathItr = polyline.end() - 1;
		for (auto itr = polyline.begin(); itr != lastPathItr; ++itr)
		{
			auto next = itr + 1;
			segs.emplace_back(*next - *itr);
		}

		//compute joint directions, joint direction is direction between two segments ie) sum of them
		std::vector<QVector2D> jointDirs;
		jointDirs.reserve(vtcsCnt);
		jointDirs.emplace_back(segs[0].normalized());
		auto lastSegItr = segs.end() - 1;
		for (auto itr = segs.begin(); itr != lastSegItr; ++itr)
		{
			auto next = itr + 1;
			jointDirs.emplace_back((*next + *itr).normalized());
		}
		jointDirs.emplace_back(segs.back().normalized());
		for (auto& each : jointDirs)
		{
			each *= radius;
		}

		for (size_t i = 0; i < vtcsCnt; ++i)
		{
			auto offset = jointDirs[i];
			Hix::Shapes2D::rotateCW90(offset);
			areaContour.emplace_back(polyline[i] + offset);
		}
		//other side of the polyline
		for (int i = vtcsCnt - 1; i >= 0 ; --i)
		{
			auto offset = jointDirs[i];
			Hix::Shapes2D::rotateCCW90(offset);
			areaContour.emplace_back(polyline[i] + offset);
		}
	}
	return areaContour;
}



std::vector<std::vector<QVector3D>> Hix::Shapes2D::combineContour(const std::vector<std::vector<QVector3D>>& contours)
{
	std::vector<std::vector<QVector3D>> result;
	ClipperLib::Clipper clipper;
	std::vector<Path> clPaths;
	std::unordered_map<ClipperLib::IntPoint, QVector3D> intToFloatMap;
	clPaths.reserve(contours.size());
	for (auto& each : contours)
	{
		clPaths.emplace_back(Hix::Polyclipping::toCLPath(each));
	}
	//map int point to float
	for (size_t i = 0; i < clPaths.size(); ++i)
	{
		auto& path = clPaths[i];
		auto& fPath = contours[i];

		for (size_t j = 0; j < path.size(); ++j)
		{
			intToFloatMap[path[j]] = fPath[j];
		}
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
			//clean path
			auto& contour = child->Contour;
			ClipperLib::Paths cleanedPolys;
			ClipperLib::SimplifyPolygon(contour, cleanedPolys, pftNonZero);

			for (auto& p : cleanedPolys)
			{
				std::vector<QVector3D> currPath;
				for (auto& pt : p)
				{
					try
					{
						auto& floatPt = intToFloatMap.at(pt);
						currPath.emplace_back(floatPt);
					}
					catch (...)
					{
						auto flPt = Hix::Polyclipping::toFloatPt(pt);
						currPath.emplace_back(flPt);
					}
				}
				result.emplace_back(std::move(currPath));
			}

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
