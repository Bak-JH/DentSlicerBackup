#include "Extrude.h"
using namespace Hix::Shapes2D;
using namespace Hix::Features::Extrusion;




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

