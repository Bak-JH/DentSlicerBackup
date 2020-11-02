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
			ModalShell(QQuickItem* parent = nullptr);
			virtual ~ModalShell();
			void addButton(std::deque<ModalShellButtonArg>&& args);
			void setMessage(const std::string& message);
			Controls::Button& closeButton();
		signals:
			void modalmsgChanged();

		protected:
			void componentComplete() override;
			QQuickItem* getQItem()override;
			QString _msg;
			QQuickItem* _buttonArea;
			Controls::Button* _closeButton = nullptr;
			std::deque<ModalShellButtonArg> _buttonArgs;
			QQmlComponent _component;

		};


	}
}