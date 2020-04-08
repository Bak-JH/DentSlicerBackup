#pragma once
#include "../../render/SceneEntity.h"
#include "ContourBuilder.h"
#include <list>
#include <vector>
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

		struct Slice { // extends Paths (total paths)
			Slice();
			std::unique_ptr<PolyTree> polytree; // containment relationship per slice
			std::deque<Contour> closedContours;
			std::deque<Contour> incompleteContours;

		};
		struct LayerGroup
		{
			float z;
			std::vector<Slice> slices;
			bool empty()const;
		};


		/****************** Entire Slicing Step *******************/
		void containmentTreeConstruct(const Slice& slice);
		std::vector<LayerGroup> slice(const Hix::Render::SceneEntity& entitiy, const Planes& planes);
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
