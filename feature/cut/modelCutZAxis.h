#pragma once
#include "../../common/Task.h"
#include "../../DentEngine/src/mesh.h"
#include "../../DentEngine/src/ContourBuilder.h"

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
			class ZAxisCutTask: public Hix::Tasking::Task
			{
			public:
				ZAxisCutTask(const Engine3D::Mesh* originalMesh, Engine3D::Mesh* botMesh, Engine3D::Mesh* topMesh, float z, bool fillCutSurface);
				tf::Taskflow& getFlow()override;

			private:
				void divideTriangles();
				void generateCutContour();
				void generateCaps();
				void fillOverlap(const Hix::Slicer::ContourSegment& seg);

				float _cuttingPlane;
				const Engine3D::Mesh* _origMesh;
				Engine3D::Mesh* _bottomMesh;
				Engine3D::Mesh* _topMesh;
				bool _fillCuttingSurface;

				std::unordered_set<Hix::Engine3D::FaceConstItr> _botFaces;
				std::unordered_set<Hix::Engine3D::FaceConstItr> _overlapFaces;
				std::unordered_set<Hix::Engine3D::FaceConstItr> _topFaces;
				std::vector<Hix::Slicer::Contour> _contours;
				tf::Taskflow _flow;


			};
		}
	}
}