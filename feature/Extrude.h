#pragma once

#include "../DentEngine/src/mesh.h"

namespace Hix
{

	namespace Features
	{
		namespace Extrusion
		{
			//extrude, can use starting face as end cap and remove starting faces.
			void extrudeFaces(Engine3D::Mesh* destinationMesh, const std::unordered_set<Engine3D::FaceConstItr>& faces,
				const std::unordered_set<Engine3D::FaceConstItr>& hintBoundaryFaces,
				const QVector3D& direction, float length, bool removeStartingFaces, bool copyEndcap);


			//extrude along a parth, returns endcap contour, given contour is on the same direction (CW, CCW) as the final end cap
			//ie) when cylinder is complete, starting contour faces wrong way and it's normal is towards the inside of cylinder
			//does not generate end caps though.
			std::vector<QVector3D> extrudeAlongPath(Engine3D::Mesh* destinationMesh, const QVector3D& normal,
				const std::vector<QVector3D>& contour, const std::vector<QVector3D>& path, std::vector<std::vector<QVector3D>>& outJointContours, 
				const std::vector<float>* scale = nullptr);
		
		}


	}
}