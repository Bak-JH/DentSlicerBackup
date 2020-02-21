#include "ProgressManager.h"
#include "feature/interfaces/Progress.h"
#include "../application/ApplicationManager.h"


using namespace Hix::QML;

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
	std::function<void()> generate = [this]()
	{
		QQmlComponent* component = new QQmlComponent(&Hix::Application::ApplicationManager::getInstance().engine(), POPUP_URL);
		auto qmlInstance = component->create(qmlContext(qmlManager->popupArea));
		auto popupShell = dynamic_cast<Hix::QML::ProgressPopupShell*>(qmlInstance);
		_popup.reset(popupShell);
		_popup->setParentItem(qmlManager->popupArea);
	};
	QMetaObject::invokeMethod(qmlManager, generate, Qt::BlockingQueuedConnection);
}

void Hix::ProgressManager::addProgress(Hix::Progress* progress)
{
	std::function<void()> addItem = [this, &progress]()
	{
		_component = new QQmlComponent(&Hix::Application::ApplicationManager::getInstance().engine(), POPUP_ITEM_URL);
		auto item = dynamic_cast<QQuickItem*>(_component->create(qmlContext(_popup->featureLayout())));
		item->setProperty("featureName", QString::fromStdString(progress->getDisplayText()));
		item->setParentItem(_popup->featureLayout());
	};
	QMetaObject::invokeMethod(qmlManager, addItem, Qt::BlockingQueuedConnection);
}

void Hix::ProgressManager::deletePopup()
{
	std::function<void()> deletePopup = [this]()
	{
		_popup.reset();
	};
	QMetaObject::invokeMethod(qmlManager, deletePopup, Qt::BlockingQueuedConnection);
}

void Hix::ProgressManager::draw()
{
	
}
