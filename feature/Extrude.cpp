#include "Extrude.h"
using namespace Hix::Shapes2D;
using namespace Hix::Features::Extrusion;
#include "Shapes2D.h"



std::vector<QVector3D> segNormals(const std::vector<QVector3D>& path)
{
	std::vector<QVector3D> segs;
	segs.reserve(path.size() - 1);
	auto lastPathItr = path.end() - 1;
	for (auto itr = path.begin(); itr != lastPathItr; ++itr)
	{
		auto next = itr + 1;
		segs.emplace_back(*next - *itr);
	}
	return segs;
}

std::vector<QVector3D> Hix::Features::Extrusion::interpolatedJointNormals(const std::vector<QVector3D>& path)
{
	std::vector<QVector3D> segs = segNormals(path);
	//compute joint directions, joint direction is direction between two segments ie) sum of them
	std::vector<QVector3D> jointDirs;
	jointDirs.reserve(path.size());
	jointDirs.emplace_back(segs[0]);
	auto lastSegItr = segs.end() - 1;
	for (auto itr = segs.begin(); itr != lastSegItr; ++itr)
	{
		auto next = itr + 1;
		jointDirs.emplace_back(*next + *itr);
	}
	jointDirs.emplace_back(segs.back());
	return jointDirs;
}

//for cylinder in Z-axis, assume a contour is bottom. b is on the correct direction.
void Hix::Features::Extrusion::generateCylinderWall(
	const QVector3D & a0, const QVector3D & a1, const QVector3D & b0, const QVector3D & b1, std::vector<std::array<QVector3D, 3>> & output)
{
	output.emplace_back(std::array<QVector3D, 3>{a0, a1, b1});
	output.emplace_back(std::array<QVector3D, 3>{a0, b1, b0});
}

void Hix::Features::Extrusion::generateCylinderWalls(const std::vector<QVector3D> & from, const std::vector<QVector3D> & to,
	std::vector<std::array<QVector3D, 3>> & output)
{
	for (size_t i = 0; i < from.size(); ++i)
	{
		auto idx0 = i % from.size();
		auto idx1 = (i+1) % from.size();

		generateCylinderWall(from[idx0], from[idx1], to[idx0], to[idx1], output);
	}
}

std::vector<std::vector<QVector3D>> Hix::Features::Extrusion::extrudeAlongPath(Engine3D::Mesh* destinationMesh, const QVector3D& contourNormal,
	const std::vector<QVector3D>& contour, const std::vector<QVector3D>& path, const std::vector<QVector3D>& jointDirs, ContourScaler* scaler)
{
	std::vector<std::vector<QVector3D>> jointContours;
	if (contour.size() < 3 || path.size() < 2)
		return jointContours;
	//joints including end cap contours
	jointContours.clear();
	jointContours.reserve(path.size());

	//order of transformation is scale, rotate, translate
	//apply scale if needed;
	for (size_t i = 0; i < path.size(); ++i)
	{
		jointContours.emplace_back(contour);
	}
	if (scaler)
	{
		for (size_t i = 0; i < jointContours.size(); ++i)
		{
			scaler->scale(jointContours[i], i);
		}
	}

	for (size_t i = 0; i < jointContours.size(); ++i)
	{
		Hix::Shapes2D::rotateContour(jointContours[i], QQuaternion::rotationTo(contourNormal, jointDirs[i]));
		Hix::Shapes2D::moveContour(jointContours[i], path[i]);
	}

	//we got all the joint contours, so we can now fill the cylinder walls;
	std::vector<std::array<QVector3D, 3>> trigVertices;
	//amount of triangles needed = cylinder wall count * 2 * number of cylinders
	trigVertices.reserve((jointContours.size() - 1) * contour.size() * 2);
	auto lastJointItr = jointContours.end() - 1;
	for (auto itr = jointContours.begin(); itr != lastJointItr; ++itr)
	{
		auto next = itr + 1;
		generateCylinderWalls(*itr, *next, trigVertices);
	}
	for (auto& trig : trigVertices)
	{
		destinationMesh->addFace(trig[0], trig[1], trig[2]);
	}
	return jointContours;

}

using namespace Hix::Shapes2D;

Hix::Features::Extrusion::VtxNormalScaler::~VtxNormalScaler()
{
}

void Hix::Features::Extrusion::VtxNormalScaler::scale(Contour& contour, size_t index)const
{
	auto sc = _scales[index];
	scaleContourVtxNormal(contour, sc);
}

Hix::Features::Extrusion::UniformScaler::~UniformScaler()
{
}

void Hix::Features::Extrusion::UniformScaler::scale(Contour& contour, size_t index)const
{
	auto sc = _scales[index];
	scaleContour(contour, sc);
}

Hix::Features::Extrusion::FloatScaler::FloatScaler(const std::vector<float>& scales): _scales(scales)
{
}

Hix::Features::Extrusion::FloatScaler::~FloatScaler()
{
}
