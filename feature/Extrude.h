#pragma once

#include "../Mesh/mesh.h"
#include "Shapes2D.h"
#include <optional>
namespace Hix
{

	namespace Features
	{
		namespace Extrusion
		{


			//given 3 points, or 2 straign segments, calculate the normal of the joints: start, joint, end
			std::vector<QVector3D> interpolatedJointNormals(const std::vector<QVector3D>& path);
			//std::vector<QVector3D> segNormals(const std::vector<QVector3D>& path);

			void generateCylinderWall(const QVector3D& a0, const QVector3D& a1,
				const QVector3D& b0, const QVector3D& b1, std::vector<std::array<QVector3D, 3>> & output);
			void generateCylinderWalls(const std::vector<QVector3D>& from, const std::vector<QVector3D>& to,
				std::vector<std::array<QVector3D, 3>> & output);

			namespace detail {
				auto noScale = [](std::vector<std::vector<QVector3D>>& contours ) {
					//do nothing
					return;
					 };
			}

			using Contour = std::vector<QVector3D>;
			class ContourScaler
			{
			public:
				virtual ~ContourScaler() {};
				virtual void scale(Contour& contour, size_t index)const = 0;
			};
			class FloatScaler : public ContourScaler
			{
			public:
				FloatScaler(const std::vector<float>& scales);
				~FloatScaler()override;
			protected:
				std::vector<float> _scales;
			};

			class VtxNormalScaler : public FloatScaler
			{
			public:
				using FloatScaler::FloatScaler;
				~VtxNormalScaler()override;
				void scale(Contour& contour, size_t index)const override;
			
			};
			//from origin
			class UniformScaler : public FloatScaler
			{
			public:
				using FloatScaler::FloatScaler;
				~UniformScaler()override;
				void scale(Contour& contour, size_t index)const override;
			};

			//extrude along a parth, returns endcap contour, given contour is on the same direction (CW, CCW) as the final end cap
			//ie) when cylinder is complete, starting contour faces wrong way and it's normal is towards the inside of cylinder
			//does not generate end caps though.
			//contourNormal normal of original contour, doesn't calculate for optimizing away float error usually positive Z-axis
			//template<typename ScalerFunctorType>
			std::vector<std::vector<QVector3D>> extrudeAlongPath(Engine3D::Mesh* destinationMesh, const QVector3D& contourNormal, const std::vector<QVector3D>& contour,
				const std::vector<QVector3D>& path, const std::vector<QVector3D>& jointDirs, ContourScaler *scaler = nullptr);


		}


	}
}