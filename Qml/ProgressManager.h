#pragma once
#include "Qml/components/PopupShell.h"
#include <QQmlApplicationEngine>

namespace Hix
{
	class Progress;
	class ProgressManager
	{
	public:
		ProgressManager();
		virtual ~ProgressManager();

		void generatePopup();
		void deletePopup();

		void draw();

	private:
		std::unique_ptr<Hix::QML::ProgressPopupShell> _popup;
		QQmlComponent* _component;
	};
}
