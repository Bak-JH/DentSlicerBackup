#pragma once
#include <QtQuick/private/qquickrectangle_p.h>
#include <QtQuick/private/qquicktext_p.h>
#include "ControlOwner.h"

class GLModel;

namespace Hix
{
	namespace QML
	{

		class ModeFactory;

		class FeaturePopupShell : public QQuickItem, public ControlOwner
		{
			Q_OBJECT
			Q_PROPERTY(QString title MEMBER _title)

		public:
			FeaturePopupShell(QQuickItem* parent = nullptr);
			virtual ~FeaturePopupShell();

			template<typename CtrlType, typename... InitArgs>
			CtrlType* findAndInitControl(std::string ctrlID, InitArgs ...)
			{
				CtrlType* control = blablabal();
				control->initialize(InitArgs ...);
			}

		protected:
			QQuickItem* getQItem()override;

		private:
			QString _title = "Name";
			//std::unique_ptr<ModeFactory> _modeFactory;
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