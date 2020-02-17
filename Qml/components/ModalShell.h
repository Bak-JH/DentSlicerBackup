#pragma once
#include "ControlOwner.h"
#include <qquickitem.h>
#include <string>
#include <qcolor.h>
#include <functional>
#include <deque>
namespace Hix
{
	namespace QML
	{
		struct ModalShellButtonArg
		{
			std::string buttonName;
			QColor defaultColor;
			QColor hoverColor;
			std::function<void()> onClickFunctor;
		};
		class ModalShell : public QQuickItem, public ControlOwner
		{
			Q_OBJECT
				Q_PROPERTY(QString modalMessage MEMBER _msg NOTIFY modalmsgChanged)
		public:
			ModalShell(QQuickItem* parent, const std::string&msg, std::deque<ModalShellButtonArg>&& args);
			ModalShell(QQuickItem* parent = nullptr);
			virtual ~ModalShell();
			void addButton(const std::string& buttonName, const QColor& color, const QColor& highlightColor, std::function<void()>&& functor);
		signals:
			void modalmsgChanged();

		protected:
			void componentComplete() override;
			QQuickItem* getQItem()override;
			QString _msg;
			QQuickItem* _buttonArea;
			std::deque<ModalShellButtonArg> _buttonArgs;

		};


	}
}