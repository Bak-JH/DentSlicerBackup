#pragma once
#include "../DentEngine//src/polyclipping/polyclipping.h"
#include <QQuaternion>

namespace Hix
{
	namespace Slicer
	{
		class Contour;
	}
	namespace Shapes2D
	{
		void rotateCW90(QVector2D& vec);
		void rotateCCW90(QVector2D& vec);

		std::vector<QVector3D> generateHexagon(float radius);
		std::vector<QVector3D> generateSquare(float radius);
		
		std::vector<QVector2D> to2DShape(const std::vector<QVector3D>& input);
		std::vector<QVector3D> to3DShape(float zPos, const std::vector<QVector2D>& input);

		std::vector<QVector2D> PolylineToArea(float thickness, const std::vector<QVector2D>& polyline);

		std::vector<ClipperLib::Path> combineContour(const std::vector<std::vector<QVector3D>>& contours);

		
		void generateCapZPlane(Hix::Engine3D::Mesh* mesh, const std::vector<QVector2D>& contour, float zPos, bool isReverse);
		void generateCapZPlane(Hix::Engine3D::Mesh* mesh, const std::vector<QVector3D>& contour, bool isReverse);

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