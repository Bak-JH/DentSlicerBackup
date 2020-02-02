#pragma once
#include <QtQuick/private/qquickrectangle_p.h>
#include <QtQuick/private/qquicktext_p.h>
#include "ControlOwner.h"

#include "QtQml/private/qqmllistmodel_p.h"
#include <string>
#include "glmodel.h"

namespace Hix
{
	namespace QML
	{

		class ModeFactory;

		class FeaturePopupShell : public QQuickItem, public ControlOwner
		{
			Q_OBJECT
			Q_PROPERTY(QString title MEMBER _title)
			Q_PROPERTY(bool applyReady MEMBER _applyReady)

		public:
			FeaturePopupShell(QQuickItem* parent = nullptr);
			virtual ~FeaturePopupShell();
			Controls::Button& closeButton();
			Controls::Button& applyButton();

		protected:
			void componentComplete() override;
			QQuickItem* getQItem()override;


		private:
			QString _title = "Name";
			bool _applyReady = false;
			Controls::Button* _closeButton;
			Controls::Button* _applyButton;

		};

		class ProgressPopupShell : public QQuickItem
		{
			Q_OBJECT
			Q_PROPERTY(int percent MEMBER _percent NOTIFY percentChanged)
		public:
			ProgressPopupShell(QQuickItem* parent = nullptr);
			virtual ~ProgressPopupShell();
			void appendFeature(std::string featureName);
			void appendFeatureList(QStringList featureName);
			void setPercentage(int percent);

		signals:
			void percentChanged();

		private:
			int _percent = 0;
		};	

		class ModalShell : public QQuickItem
		{
			Q_OBJECT
			Q_PROPERTY(QString modalMessage MEMBER _modalmsg NOTIFY modalmsgChanged)

		public:
			ModalShell(QQuickItem* parent = nullptr);
			virtual ~ModalShell();
			void setMessage(std::string message);

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