#pragma once
#include "../../render/SceneEntity.h"
#include "ContourBuilder.h"
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
			Slice();

			float z;
			std::unique_ptr<PolyTree> polytree; // containment relationship per slice

			std::deque<Contour> closedContours;
			std::deque<Contour> incompleteContours;

		};


		/****************** Entire Slicing Step *******************/
		void containmentTreeConstruct(std::vector<Slice>& slices);

		void slice(const Hix::Render::SceneEntity& entitiy, const Planes& planes, std::vector<Slice>& slices);
	};
}


namespace Hix
{
	namespace Debug
	{
		QDebug operator<< (QDebug d, const Hix::Slicer::Slice& obj);
		QDebug operator<< (QDebug d, const  std::vector<Hix::Slicer::Slice> & obj);
	}
}
