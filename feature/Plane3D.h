#pragma once
#include <QVector3D>
#include <vector>
namespace Hix
{
	namespace Plane3D
	{
		struct PDPlane
		{
			QVector3D point;
			QVector3D normal;
		};
		PDPlane bestFittingPlane(const std::vector<QVector3D>& points);

	}
}

