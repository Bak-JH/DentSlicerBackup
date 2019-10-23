#pragma once

#include "../DentEngine/src/mesh.h"
#include "Shapes2D.h"

namespace Hix
{

	namespace Features
	{
		namespace Extrusion
		{

			void generateCylinderWall(const QVector3D& a0, const QVector3D& a1,
				const QVector3D& b0, const QVector3D& b1, std::vector<std::array<QVector3D, 3>> & output);
			void generateCylinderWalls(const std::vector<QVector3D>& from, const std::vector<QVector3D>& to,
				std::vector<std::array<QVector3D, 3>> & output);

			//extrude along a parth, returns endcap contour, given contour is on the same direction (CW, CCW) as the final end cap
			//ie) when cylinder is complete, starting contour faces wrong way and it's normal is towards the inside of cylinder
			//does not generate end caps though.
			template<typename ScaleArg>
			std::vector<QVector3D> extrudeAlongPath(Engine3D::Mesh* destinationMesh, const QVector3D& normal,
				const std::vector<QVector3D>& contour, const std::vector<QVector3D>& path, std::vector<std::vector<QVector3D>>& jointContours,
				const std::vector<ScaleArg>* scales = nullptr, std::function<void (std::vector<QVector3D>&,  ScaleArg)>* scaleFunctor = nullptr)
			{

				if (contour.size() < 3 || path.size() < 2 || (scales && (!scaleFunctor || scales->size() != path.size())))
					return std::vector<QVector3D>();
				//joints including end cap contours
				jointContours.clear();
				jointContours.reserve(path.size());
				//compute segments
				std::vector<QVector3D> segs;
				segs.reserve(path.size() - 1);
				auto lastPathItr = path.end() - 1;
				for (auto itr = path.begin(); itr != lastPathItr; ++itr)
				{
					auto next = itr + 1;
					segs.emplace_back(*next - *itr);
				}

				//compute joint directions, joint direction is direction between two segments ie) sum of them
				std::vector<QVector3D> jointDirs;
				jointDirs.reserve(path.size());
				jointDirs.emplace_back(segs[0]);
				auto lastSegItr = segs.end() - 1;
				for (auto itr = segs.begin(); itr != lastSegItr; ++itr)
				{
					auto next = itr + 1;
					jointDirs.emplace_back(*next + *itr);
				}
				jointDirs.emplace_back(segs.back());
				//build joint contours
				for (size_t i = 0; i < path.size(); ++i)
				{
					jointContours.emplace_back(contour);
				}
				//order of transformation is scale, rotate, translate
				if (scales)
				{
					for (size_t i = 0; i < jointContours.size(); ++i)
					{
						(*scaleFunctor)(jointContours[i], (*scales)[i]);
					}
				}
				for (size_t i = 0; i < jointContours.size(); ++i)
				{
					Hix::Shapes2D::rotateContour(jointContours[i], QQuaternion::rotationTo(normal, jointDirs[i]));
					Hix::Shapes2D::moveContour(jointContours[i], path[i]);
				}
				//apply scale if needed;
				//we got all the joint contours, so we can now fill the cylinder walls;
				std::vector<std::array<QVector3D, 3>> trigVertices;
				//amount of triangles needed = cylinder wall count * 2 * number of cylinders
				trigVertices.reserve((jointContours.size() - 1) * contour.size() * 2);
				auto lastJointItr = jointContours.end() - 1;
				for (auto itr = jointContours.begin(); itr != lastJointItr; ++itr)
				{
					auto next = itr + 1;
					generateCylinderWalls(*itr, *next, trigVertices);
				}
				for (auto& trig : trigVertices)
				{
					destinationMesh->addFace(trig[0], trig[1], trig[2]);
				}

				//return information for the ending cap so that user can generate it if he wants it
				return jointContours.back();
			}

		}


	}
}