#pragma once
#include "ControlOwner.h"
#include <qquickitem.h>
#include <string>
namespace Hix
{
	namespace QML
	{


		class FeaturePopupShell : public QQuickItem, public ControlOwner
		{
			Q_OBJECT
		public:
			FeaturePopupShell(QQuickItem* parent = nullptr);
			virtual ~FeaturePopupShell();
			Controls::Button& closeButton();
			Controls::Button& applyButton();

		protected:
			void componentComplete() override;
			QQuickItem* getQItem()override;


		private:
			//bool _applyReady = false;
			Controls::Button* _closeButton = nullptr;
			Controls::Button* _applyButton = nullptr;

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


	}
}