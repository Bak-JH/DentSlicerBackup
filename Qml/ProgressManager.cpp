#include "ProgressManager.h"
#include "feature/interfaces/Progress.h"
#include "../application/ApplicationManager.h"
#include "../glmodel.h"

using namespace Hix::QML;
using namespace Hix::Application;
const QUrl ERROR_POPUP_URL = QUrl("qrc:/Qml/ProgressException.qml");
const QUrl POPUP_URL = QUrl("qrc:/Qml/ProgressPopup.qml");
const QUrl POPUP_ITEM_URL = QUrl("qrc:/Qml/ProgressItem.qml");
Hix::ProgressManager::ProgressManager()
{
}

Hix::ProgressManager::~ProgressManager()
{
}

void Hix::ProgressManager::generatePopup()
{
	auto popupArea = Hix::Application::ApplicationManager::getInstance().modalDialogManager().popupArea();
	auto qmlInstance = _popupComp->create(qmlContext(popupArea));
	auto popupShell = dynamic_cast<Hix::QML::ProgressPopupShell*>(qmlInstance);
	_popup.reset(popupShell);
	_popup->setParentItem(popupArea);

}

void Hix::ProgressManager::generateErrorPopup(const char* message)
{
	auto popupArea = Hix::Application::ApplicationManager::getInstance().modalDialogManager().popupArea();
	auto qmlInstance = _errorComp->create(qmlContext(popupArea));
	auto popupShell = dynamic_cast<Hix::QML::ProgressPopupShell*>(qmlInstance);

	_popup.reset(popupShell);
	_popup->setParentItem(popupArea);
	_popup->setProperty("errorMessage", QString(message));
}

void Hix::ProgressManager::addProgress(Hix::Progress* progress)
{
	qDebug() << progress->getDisplayText();
	auto item = dynamic_cast<QQuickItem*>(_progressComp->create(qmlContext(_popup->featureLayout())));
	item->setProperty("featureName", progress->getDisplayText());
	item->setParentItem(_popup->featureLayout());

}

void Hix::ProgressManager::deletePopup()
{
	_popup.reset();
}

void Hix::ProgressManager::init(QQmlApplicationEngine* engine)
{
	_popupComp.emplace(engine, POPUP_URL);
	_errorComp.emplace(engine, ERROR_POPUP_URL);
	_progressComp.emplace(engine, POPUP_ITEM_URL);

#ifdef _DEBUG
	if (!_popupComp->isReady())
	{
		qDebug() << "_popupComp error: " << _popupComp->errors();
	}
	if (!_errorComp->isReady())
	{
		qDebug() << "_errorComp error: " << _errorComp->errors();
	}	
	if (!_progressComp->isReady())
	{
		qDebug() << "_progressComp error: " << _progressComp->errors();
	}
#endif

}

void Hix::ProgressManager::draw()
{
	
}
