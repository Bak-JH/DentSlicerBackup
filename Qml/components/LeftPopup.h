  #pragma once
#include <QtQuick/private/qquickrectangle_p.h>
#include <QtQuick/private/qquicktext_p.h>
#include "Buttons.h"
#include "Inputs.h"
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

		//class TextButton : public RoundButton
		//{
		//	Q_OBJECT

		//		Q_PROPERTY(QString btntext MEMBER _btntext NOTIFY btntextChanged)
		//		Q_PROPERTY(QColor btncolor MEMBER _btncolor NOTIFY btnColorChanged)

		//public:
		//	TextButton(QQuickItem* parent = nullptr);

		//signals:
		//	void btnColorChanged(const QColor& changedColor);
		//public slots:
		//	void onClick();
		//	void onEntered();
		//	void onExited();
		//	void btntextChanged();

		//private:
		//	QString _btntext = "Apply";
		//	QColor _btncolor;
		//};

		/*
		class FreeCutButton : public RoundButton
		{
			Q_OBJECT
			Q_PROPERTY(QColor btncolor MEMBER _btncolor NOTIFY btnColorChanged)

		public:
			FreeCutButton(QQuickItem* parent = nullptr);

		signals:
			void btnColorChanged(const QColor& changedColor);
			void clicked();
			void entered();
			void exited();

		public slots:
			void onClick();
			void onEntered();
			void onExited();

		private:
			QColor _btncolor;
		};

		class FlatCutButton : public RoundButton
		{
			Q_OBJECT

		public:
			FlatCutButton(QQuickItem* parent = nullptr);

		public slots:
			void onClick();
		};
		*/

		class LabelTextBox : public QQuickRectangle
		{
			Q_OBJECT

		};


	}
}