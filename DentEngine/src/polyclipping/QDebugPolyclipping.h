#pragma once

#include "clipper/clipper.hpp"
#include "poly2tri/poly2tri.h"
#include <QDebug>


namespace Hix
{
	namespace Debug
	{
		QDebug operator<< (QDebug d, const PolyNode& obj) {
			d << "Contour: " << obj.Contour;
			for (const auto& each : obj.Childs)
			{
				d << "child: " << each;
			}
			d << "isHole: " << obj.IsHole();
			d << "IsOpen: " << obj.IsOpen();
			return d;
		}


		QDebug operator<< (QDebug d, const PolyTree& obj) {
			d << "total: " << obj.Total();
			const auto* node = dynamic_cast<const PolyNode*>(&obj);
			d << "as a node:" << *node;
			return d;
		}

		QDebug operator<< (QDebug d, const IntPoint& obj) {

			d << "IntPoint: " << obj.X << obj.Y;
			return d;
		}


		QDebug operator<< (QDebug d, const Path& obj) {

			d << "Path: ";
			for (const auto each : obj)
			{
				d << "path point at: " << each;
			}

			return d;
		}


		QDebug operator<< (QDebug d, const Paths& obj) {

			d << "Paths: ";
			for (const auto each : obj)
			{
				d << each;
			}

			return d;
		}

	}
}
