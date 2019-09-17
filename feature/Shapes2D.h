#include "../DentEngine//src/polyclipping/polyclipping.h"
#include <QQuaternion>

namespace Hix
{
	namespace Shapes2D
	{

		std::vector<QVector3D> generateHexagon(float radius);

		std::vector<ClipperLib::Path> combineContour(const std::vector<std::vector<QVector3D>>& contours);

		void moveContour(std::vector<QVector3D>& targetContour, const QVector3D& moveVector);
		void rotateContour(std::vector<QVector3D>& targetContour,const QQuaternion& rotation);
		void scaleContour(std::vector<QVector3D>& targetContour, float scale);
		void scaleContourAroundCentoid(std::vector<QVector3D>& targetContour, float scale, QVector2D& centoid);
	}
}