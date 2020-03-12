#pragma once
#include <qquickitem.h>
class QVector3D;
namespace Hix
{
	namespace QML
	{
		class PrintInfo : public QQuickItem
		{
			Q_OBJECT
			Q_PROPERTY(QString printsize MEMBER _printsize)
			Q_PROPERTY(QString printvolume MEMBER _printvolume)

		public:
			PrintInfo(QQuickItem* parent = nullptr);
			void printVolumeChanged(const QVector3D& bound);

		protected:
			QString _printsize = "0mm x 0mm x 0mm";
			QString _printvolume = "0mL";
		};		
	}
}