#pragma once
#include <optional>
#include <vector>
#include <qvector3d.h>
#include "Interconnect.h"

namespace Hix
{
	namespace Support
	{
		/// <summary>
		/// model created by extruding along a contour path
		/// </summary>
		class ContourModel
		{
		public:
			virtual std::optional<std::array<QVector3D, 2>> verticalSegment();
		protected:
			std::vector<QVector3D> _contour;
		};

	}
}
