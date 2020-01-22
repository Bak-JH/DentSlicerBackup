#pragma once
#include <QtQuick/private/qquickrectangle_p.h>
#include <QtQuick/private/qquicktext_p.h>
#include "Buttons.h"
#include "Inputs.h"
#include "QtQml/private/qqmllistmodel_p.h"
#include <string>
#include "glmodel.h"

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
			Q_PROPERTY(QString title MEMBER _title)

		public:
			LeftPopupShell(QQuickItem* parent = nullptr);
			virtual ~LeftPopupShell();

		signals:
			void titleChanged();

		private:
			QString _title = "Name";
		};

		class ProgressPopupShell : public QQuickRectangle
		{
			Q_OBJECT
		public:
			ProgressPopupShell(QQuickItem* parent = nullptr);
			virtual ~ProgressPopupShell();
			void appendFeature(std::string featureName);
			void appendFeatureList(QStringList featureName);
		};

		class ModalShell : public QQuickItem
		{
			Q_OBJECT
			Q_PROPERTY(QString modalMessage MEMBER _modalmsg NOTIFY modalmsgChanged)

		public:
			ModalShell(QQuickItem* parent = nullptr);
			virtual ~ModalShell();

		signals:
			void modalmsgChanged();

		protected:
			QString _modalmsg = "Are you sure?";
		};

		class PartList : public QQuickItem
		{
			Q_OBJECT
		public:
			PartList(QQuickItem* parent = nullptr);
			virtual ~PartList();
			void appendModel(GLModel* model);
			Q_INVOKABLE void deleteModels();
		};
	}
}