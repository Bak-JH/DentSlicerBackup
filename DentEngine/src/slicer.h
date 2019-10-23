#pragma once
#include "../../render/SceneEntity.h"
//#include "support.h"
//#include "infill.h"
#include <list>
#include <QThread>
#include <QDebug>
#include <QVector2D>
using namespace ClipperLib;
using namespace Hix::Engine3D;
class OverhangPoint;



namespace Hix
{
	namespace Render
	{

	}
	namespace Slicer
	{
		class Planes;

		class Slice { // extends Paths (total paths)
		public:
			float z;
			PolyTree polytree; // containment relationship per slice
			PolyTree overhang;

			Paths closedContours;
		};


		class Slices : public std::deque<Slice> {
		public:
			Slices(size_t size);
			void containmentTreeConstruct();

		};

		/****************** Entire Slicing Step *******************/
		void slice(const Hix::Render::SceneEntity& entitiy, const Planes* planes, Slices* slices);
	};
}


namespace Hix
{
	namespace Debug
	{
		QDebug operator<< (QDebug d, const Hix::Slicer::Slice& obj);
		QDebug operator<< (QDebug d, const Hix::Slicer::Slices& obj);
	}
}
