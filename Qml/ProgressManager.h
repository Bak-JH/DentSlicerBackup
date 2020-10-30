#pragma once
#include "Qml/components/PopupShell.h"
#include <QQmlApplicationEngine>
#include <deque>
#include <optional>
namespace Hix
{
	class Progress;
	class ProgressManager
	{
	public:
		ProgressManager();
		virtual ~ProgressManager();

		void generatePopup();
		void generateErrorPopup(const char* message);
		void addProgress(Progress* progress);
		void deletePopup();
		void init(QQmlApplicationEngine* engine);

		void draw();

	private:
		std::optional<QQmlComponent> _errorComp;
		std::optional<QQmlComponent> _popupComp;
		std::optional<QQmlComponent> _progressComp;
		std::unique_ptr<Hix::QML::ProgressPopupShell> _popup;
	};
}
