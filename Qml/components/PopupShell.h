#pragma once
#include <QtQuick/private/qquickrectangle_p.h>
#include <QtQuick/private/qquicktext_p.h>
#include "ControlOwner.h"

#include "QtQml/private/qqmllistmodel_p.h"
//#include <QComboBox>

namespace Hix
{
	namespace QML
	{
		class ModeFactory;
		class LeftPopupShell : public QQuickItem, public ControlOwner
		{
			Q_OBJECT
			Q_PROPERTY(QString title MEMBER _title)

		public:
			LeftPopupShell(QQuickItem* parent = nullptr);
			virtual ~LeftPopupShell();

		signals:
			void titleChanged();
		protected:
			QQuickItem* getQItem()override;
		private:
			QString _title = "Name";
			//std::unique_ptr<ModeFactory> _modeFactory;
		};

		class ProgressPopupShell : public QQuickRectangle
		{
			Q_OBJECT
		public:
			ProgressPopupShell(QQuickItem* parent = nullptr);
			virtual ~ProgressPopupShell();
			Q_INVOKABLE void appendFeature(QString featureName);
			template<typename CtrlType, typename... InitArgs>
			CtrlType* findAndInitControl(std::string ctrlID, InitArgs ...)
			{
				CtrlType* control = blablabal();
				control->initialize(InitArgs ...);
			}

		};
	}
}