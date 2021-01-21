#pragma once
#include "polyclipping/polyclipping.h"
#include <QQuaternion>

namespace Hix
{
	namespace Engine3D
	{
		class Mesh;
	}
	namespace Slicer
	{
		class Contour;
	}
	namespace Shapes2D
	{

		template<typename ItrType>
		void move3D(QVector3D offset, ItrType itr, ItrType end)
		{
			for (; itr != end; ++itr)
			{
				(*itr) += offset;
			}
		}
		template<typename ItrType>
		void setZ(float offset, ItrType itr, ItrType end)
		{
			for (; itr != end; ++itr)
			{
				itr->setZ(offset);
			}
		}
		std::vector<std::vector<QVector3D>> gridCircle(float radius, float offset);
		std::vector<std::vector<QVector3D>> gridRect(float xLength, float yLength, float offset);

		void rotateCW90(QVector2D& vec);
		void rotateCCW90(QVector2D& vec);

		void circleToTri(Hix::Engine3D::Mesh* supportMesh, const std::vector<QVector3D>& endCapOutline, const QVector3D& center, bool oppDir);
		std::vector<QVector3D> generateCircle(float radius, size_t segCount);
		std::vector<QVector3D> generateHexagon(float radius);
		std::vector<QVector3D> generateSquare(float radius);
		
		std::vector<QVector2D> to2DShape(const std::vector<QVector3D>& input);
		std::vector<QVector3D> to3DShape(float zPos, const std::vector<QVector2D>& input);

		std::vector<QVector2D> PolylineToArea(float thickness, const std::vector<QVector2D>& polyline);

		std::vector<std::vector<QVector3D>> combineContour(const std::vector<std::vector<QVector3D>>& contours);

		void moveContour(std::vector<QVector3D>& targetContour, const QVector3D& moveVector);
		void rotateContour(std::vector<QVector3D>& targetContour,const QQuaternion& rotation);
		void scaleContour(std::vector<QVector3D>& targetContour, float scale);
		void scaleContourVtxNormal(std::vector<QVector3D>& targetContour, float scale);

		bool isClockwise(const Hix::Slicer::Contour& contour);

		void scaleContourAroundCentoid(std::vector<QVector3D>& targetContour, float scale, QVector2D& centoid);

		template<typename QVectorType>
		bool isClockwise(const std::vector<QVectorType>& contour)
		{
			if (contour.size() < 2)
				return true;
			QVector3D prevPt = contour.back();
			double sum = 0;
			for (auto& pt : contour)
			{
				sum += (pt.x() - prevPt.x()) * (pt.y() + prevPt.y());
				prevPt = pt;
			}
			return sum >= 0;
		}
		template<typename QVectorType>
		void ensureOrientation(bool isClockwise, std::vector<QVectorType>& contour)
		{
			if (Hix::Shapes2D::isClockwise(contour) == isClockwise)
				return;
			std::reverse(contour.begin(), contour.end());
		}

	}
}