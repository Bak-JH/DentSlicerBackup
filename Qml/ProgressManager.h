#pragma once
#include "Qml/components/PopupShell.h"
#include <QQmlApplicationEngine>
#include <deque>

namespace Hix
{
	class Progress;
	class ProgressManager
	{
	public:
		ProgressManager();
		virtual ~ProgressManager();

		void generatePopup();
		void addProgress(Progress* progress);
		void deletePopup();

		void draw();

	private:
		std::unique_ptr<Hix::QML::ProgressPopupShell> _popup;
		QQmlComponent* _component;
		std::deque<std::string> _featureList;
	};
}
