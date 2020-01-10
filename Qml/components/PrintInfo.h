#pragma once
#include <QtQuick/private/qquickrectangle_p.h>
#include <QtQuick/private/qquickmousearea_p.h>
#include <QtQuick/private/qquickevents_p_p.h>
#include <QtQuick/private/qquicktext_p.h>

namespace Hix
{
	namespace QML
	{
		class PrintInfoText : public QQuickRectangle
		{
			Q_OBJECT
			Q_PROPERTY(QString printtime MEMBER _printtime NOTIFY printtimeChanged)
			Q_PROPERTY(QString printlayer MEMBER _printlayer NOTIFY printlayerChanged)
			Q_PROPERTY(QString printsize MEMBER _printsize NOTIFY printsizeChanged)
			Q_PROPERTY(QString printvolume MEMBER _printvolume NOTIFY printvolumeChanged)

		public:
			PrintInfoText(QQuickItem* parent = nullptr);

		public slots:
			void printtimeChanged();
			void printlayerChanged();
			void printsizeChanged();
			void printvolumeChanged();

		protected:
			QString _printtime = "0hr 0min";
			QString _printlayer = "0hr 0min";
			QString _printsize = "0hr 0min";
			QString _printvolume = "0hr 0min";
		};		
	}
}