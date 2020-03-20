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
			Q_PROPERTY(QString printsize MEMBER _printsize  NOTIFY printsizeChanged)

		public:
			PrintInfo(QQuickItem* parent = nullptr);
			void printVolumeChanged(QVector3D bound);
		signals:
			void printsizeChanged();
		protected:
			QString _printsize = "0mm x 0mm x 0mm";
		};		
	}
}