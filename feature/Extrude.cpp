#include "Extrude.h"
using namespace Hix::Features::Extrusion;
namespace ExtrudePrivate
{
	void moveContour(std::vector<QVector3D>& targetContour, QVector3D moveVector);
	void rotateContour(std::vector<QVector3D>& targetContour, QQuaternion rotation);
	void scaleContour(std::vector<QVector3D>& targetContour, float scale);

	void generateCylinderWall(const QVector3D & a0, const QVector3D & a1,
		const QVector3D & b0, const QVector3D & b1, std::vector<std::array<QVector3D, 3>>& output);
	void generateCylinderWalls(const std::vector<QVector3D> & from, const std::vector<QVector3D> & to,
		std::vector<std::array<QVector3D, 3>> & output);
}

void ExtrudePrivate::moveContour(std::vector<QVector3D>& targetContour, QVector3D moveVector)
{
	for (auto& each : targetContour)
	{
		each += moveVector;
	}
}

void ExtrudePrivate::rotateContour(std::vector<QVector3D>& targetContour, QQuaternion rotation)
{
	for (auto& each : targetContour)
	{
		each = rotation.rotatedVector(each);
	}
}

void ExtrudePrivate::scaleContour(std::vector<QVector3D>& targetContour, float scale)
{
	for (auto& each : targetContour)
	{
		each *= scale;
	}
}

//for cylinder in Z-axis, assume a contour is bottom. b is on the correct direction.
void ExtrudePrivate::generateCylinderWall(
	const QVector3D & a0, const QVector3D & a1, const QVector3D & b0, const QVector3D & b1, std::vector<std::array<QVector3D, 3>> & output)
{
	output.emplace_back(std::array<QVector3D, 3>{a0, a1, b1});
	output.emplace_back(std::array<QVector3D, 3>{a0, b1, b0});
}

void ExtrudePrivate::generateCylinderWalls(const std::vector<QVector3D> & from, const std::vector<QVector3D> & to,
	std::vector<std::array<QVector3D, 3>> & output)
{
	for (size_t i = 0; i < from.size(); ++i)
	{
		auto idx0 = i % from.size();
		auto idx1 = (i+1) % from.size();

		generateCylinderWall(from[idx0], from[idx1], to[idx0], to[idx1], output);
	}
}

using namespace ExtrudePrivate;

std::vector<QVector3D> Hix::Features::Extrusion::extrudeAlongPath(Engine3D::Mesh* destinationMesh, const QVector3D& normal,
	const std::vector<QVector3D>& contour, const std::vector<QVector3D>& path, const std::vector<float>* scale)
{
	if (contour.size() < 3 || path.size() < 2)
		return std::vector<QVector3D>();
	//joints including end cap contours
	std::vector<std::vector<QVector3D>> jointContours;
	jointContours.reserve(path.size());
	//compute segments
	std::vector<QVector3D> segs;
	segs.reserve(path.size() - 1);
	auto lastPathItr = path.end() - 1;
	for (auto itr = path.begin(); itr != lastPathItr; ++itr)
	{
		auto next = itr + 1;
		segs.emplace_back(*next - *itr);
	}

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

	//build joint contours

	for (size_t i = 0; i < path.size(); ++i)
	{
		jointContours.emplace_back(contour);
	}
	//order of transformation is scale, rotate, translate
	if (scale)
	{
		for (size_t i = 0; i < jointContours.size(); ++i)
		{
			scaleContour(jointContours[i], (*scale)[i]);
		}
	}
	for (size_t i = 0; i < jointContours.size(); ++i)
	{
		rotateContour(jointContours[i], QQuaternion::rotationTo(normal, jointDirs[i]));
		moveContour(jointContours[i], path[i]);
	}
	//apply scale if needed;
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

	//return information for the ending cap so that user can generate it if he wants it
	return jointContours.back();
}
