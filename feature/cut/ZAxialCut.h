#pragma once
#include <unordered_set>
#include "../../DentEngine/src/ContourBuilder.h"
#include "../../common/Task.h"

class GLModel;
namespace Hix
{
	namespace Engine3D
	{
		class Mesh;
	}

	namespace Features
	{
		namespace Cut
		{
			class ZAxialCut
			{
			public:
				ZAxialCut(GLModel* subject, float cuttingPlane, bool fill);
			private:
				void divideTriangles();
				void generateCutContour();
				void generateCaps();
				void fillOverlap(const Hix::Slicer::ContourSegment& seg);

				bool _fill;
				float _cuttingPlane;
				const Engine3D::Mesh* _origMesh;
				Engine3D::Mesh* _bottomMesh;
				Engine3D::Mesh* _topMesh;
				std::unordered_set<Hix::Engine3D::FaceConstItr> _botFaces;
				std::unordered_set<Hix::Engine3D::FaceConstItr> _overlapFaces;
				std::unordered_set<Hix::Engine3D::FaceConstItr> _topFaces;
				std::vector<Hix::Slicer::Contour> _contours;
			};
		}
	}
}
