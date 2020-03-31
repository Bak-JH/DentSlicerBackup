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
	QQmlComponent* component = new QQmlComponent(&Hix::Application::ApplicationManager::getInstance().engine(), POPUP_URL);
	auto popupArea = Hix::Application::ApplicationManager::getInstance().modalDialogManager().popupArea();
	auto qmlInstance = component->create(qmlContext(popupArea));
	auto popupShell = dynamic_cast<Hix::QML::ProgressPopupShell*>(qmlInstance);
	_popup.reset(popupShell);
	_popup->setParentItem(popupArea);

}

void Hix::ProgressManager::generateErrorPopup(const char* message)
{
	QQmlComponent* component = new QQmlComponent(&Hix::Application::ApplicationManager::getInstance().engine(), ERROR_POPUP_URL);
	auto popupArea = Hix::Application::ApplicationManager::getInstance().modalDialogManager().popupArea();
	auto qmlInstance = component->create(qmlContext(popupArea));
	auto popupShell = dynamic_cast<Hix::QML::ProgressPopupShell*>(qmlInstance);

	_popup.reset(popupShell);
	_popup->setParentItem(popupArea);
	_popup->setProperty("errorMessage", QString(message));
}

void Hix::ProgressManager::addProgress(Hix::Progress* progress)
{
	qDebug() << progress->getDisplayText();
	QQmlComponent* component = new QQmlComponent(&Hix::Application::ApplicationManager::getInstance().engine(), POPUP_ITEM_URL);
	auto item = dynamic_cast<QQuickItem*>(component->create(qmlContext(_popup->featureLayout())));
	item->setProperty("featureName", progress->getDisplayText());
	item->setParentItem(_popup->featureLayout());

}

void Hix::ProgressManager::deletePopup()
{
	_popup.reset();
}

void Hix::ProgressManager::draw()
{
	
}
