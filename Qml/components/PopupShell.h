#pragma once
#include <QtQuick/private/qquickrectangle_p.h>
#include <QtQuick/private/qquicktext_p.h>
#include "Buttons.h"
#include "Inputs.h"
#include "QtQml/private/qqmllistmodel_p.h"
//#include <QComboBox>

namespace Hix
{
	namespace QML
	{
		class CloseButton;
		class RoundButton;
		class LeftPopupContent;

		class LeftPopupShell : public QQuickItem
		{
			Q_OBJECT
			Q_PROPERTY(QString title MEMBER _title NOTIFY titleChanged)

		public:
			LeftPopupShell(QQuickItem* parent = nullptr);


		signals:
			void titleChanged();

		private:
			QString _title = "Name";
		};

		class ProgressPopupShell : public QQuickRectangle
		{
			Q_OBJECT
			Q_PROPERTY(QString title MEMBER _title NOTIFY titleChanged)

		public:
			ProgressPopupShell(QQuickItem* parent = nullptr);
			virtual ~ProgressPopupShell();
			Q_INVOKABLE void appendFeature(QString featureName);

		signals:
			void titleChanged();

		private:
			QString _title = "Name";
		};
	}
}