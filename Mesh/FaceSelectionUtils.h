#pragma once
#include <deque>
#include <vector>
#include <QVector3D>
#include "../Mesh/mesh.h"

namespace Hix
{
	namespace Engine3D
	{
		std::deque<std::deque<HalfEdgeConstItr>> boundaryPath(const std::unordered_set<FaceConstItr>& faces);
		//normal and distance in model coord
		std::vector<QVector3D> extendAlongOutline(QVector3D normal, double distance, const std::deque<HalfEdgeConstItr>& path);

	}
}