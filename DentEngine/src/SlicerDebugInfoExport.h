#pragma once
#include <QPainter>
#include <deque>
#include <random>
#include "ContourBuilder.h"
namespace Hix
{
	namespace Slicer
	{
		namespace Debug
		{

			void outDebugSVGs(const std::deque<Hix::Slicer::Contour>& contours, size_t z);
			void outDebugIncompletePathsSVGs(const std::deque<Hix::Slicer::Contour>& contours, size_t z);

		}
	}
}

