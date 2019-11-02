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

		std::vector<QVector3D> generateHexagon(float radius);
		std::vector<QVector3D> generateSquare(float radius);


		std::vector<ClipperLib::Path> combineContour(const std::vector<std::vector<QVector3D>>& contours);

		void moveContour(std::vector<QVector3D>& targetContour, const QVector3D& moveVector);
		void rotateContour(std::vector<QVector3D>& targetContour,const QQuaternion& rotation);
		void scaleContour(std::vector<QVector3D>& targetContour, float scale);
		void scaleContourVtxNormal(std::vector<QVector3D>& targetContour, float scale);

		bool isClockwise(const std::vector<QVector3D>& contour);
		bool isClockwise(const Hix::Slicer::Contour& contour);
		void ensureOrientation(bool isClockwise, std::vector<QVector3D>& contour);

		void scaleContourAroundCentoid(std::vector<QVector3D>& targetContour, float scale, QVector2D& centoid);
	}
}