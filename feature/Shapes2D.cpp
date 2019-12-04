#include "Shapes2D.h"
#include "../DentEngine/src/ContourBuilder.h"
#include "agCDT/CDT.h"
//#include "../cork/src/isct/triangle.h"

using namespace ClipperLib;


namespace std
{

	template<>
	struct hash<CDT::V2d<double>>
	{
		//2D only!
		std::size_t operator()(const CDT::V2d<double>& pt)const
		{
			size_t x = (size_t)(pt.x);
			size_t y = (size_t)(pt.y);
			size_t digest = x | y << 32;
			return digest;
		}
	};
}

template <typename QVectorType>
std::vector<CDT::V2d<double>> QVectorToCDTVtx(const std::vector<QVectorType>& input)
{
	std::vector<CDT::V2d<double>> v2d;
	v2d.reserve(input.size());
	for (auto& each : input)
	{
		v2d.emplace_back(CDT::V2d<double>::make(each.x(), each.y()));
	}

	std::unordered_set<CDT::V2d<double>> test;
	for (auto& each : v2d)
	{
		test.insert(each);
	}
	if (test.size() != v2d.size())
	{
		return v2d;
	}

	return v2d;
}
template <typename QVectorType>
std::vector<CDT::Edge> QVectorToCDTEdge(const std::vector<QVectorType>& input)
{
	std::vector<CDT::Edge> edges;
	if (input.size() < 2)
		return edges;
	auto edgeCnt = input.size();
	edges.reserve(edgeCnt);
	--edgeCnt;
	edges.emplace_back(0, edgeCnt);
	for (size_t i = 0; i < edgeCnt; ++i)
	{
		edges.emplace_back(i, i + 1);
	}
	return edges;
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


void generateCapZPlaneImpl(Hix::Engine3D::Mesh* mesh, float zPos, const std::vector<CDT::V2d<double>>& vtcs, const std::vector<CDT::Edge> edges, bool isReverse)
{
	CDT::Triangulation<double> cdt = CDT::Triangulation<double>(CDT::FindingClosestPoint::BoostRTree);
	// ... same as above
	cdt.insertVertices(vtcs);
	cdt.insertEdges(edges);
	cdt.eraseOuterTriangles();
	//TODO: is input vtcs different from output vtcs?
	auto& vtcsOut = cdt.vertices;
	auto& tris = cdt.triangles;

	//as long as intial contour is not modified in the p2t library, float->double->float should be lossless
	for (auto& tri : tris)
	{
		auto& triVtcs = tri.vertices;
		CDT::V2d<double> pt0, pt1,pt2;
		if (isReverse)
		{
			pt0 = vtcsOut[triVtcs[2]].pos;
			pt1 = vtcsOut[triVtcs[1]].pos;
			pt2 = vtcsOut[triVtcs[0]].pos;

		}
		else
		{
			pt0 = vtcsOut[triVtcs[0]].pos;
			pt1 = vtcsOut[triVtcs[1]].pos;
			pt2 = vtcsOut[triVtcs[2]].pos;
		}

		vtcsOut[triVtcs[2]].pos.x;
		mesh->addFace(
			QVector3D(pt0.x, pt0.y, zPos),
			QVector3D(pt1.x, pt1.y, zPos),
			QVector3D(pt2.x, pt2.y, zPos));
	}

}

void Hix::Shapes2D::generateCapZPlane(Hix::Engine3D::Mesh* mesh, const std::vector<QVector3D>& contour, bool isReverse)
{
	if (contour.size() < 2)
		return;
	auto vtcs = QVectorToCDTVtx(contour);
	auto edges = QVectorToCDTEdge(contour);
	float z = contour.front().z();
	generateCapZPlaneImpl(mesh, z, vtcs, edges, isReverse);
}

void Hix::Shapes2D::generateCapZPlane(Hix::Engine3D::Mesh* mesh, const std::vector<QVector2D>& contour, float zPos, bool isReverse)
{
	if (contour.size() < 2)
		return;
	auto vtcs = QVectorToCDTVtx(contour);
	auto edges = QVectorToCDTEdge(contour);
	generateCapZPlaneImpl(mesh, zPos, vtcs, edges, isReverse);
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
			std::vector<QVector3D> currPath;
			for (auto& pt : child->Contour)
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
