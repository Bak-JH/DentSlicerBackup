#include "ProgressManager.h"
#include "feature/interfaces/Progress.h"
#include "qmlmanager.h"

using namespace Hix::QML;

const QUrl POPUP_URL = QUrl("qrc:/Qml/ProgressPopup.qml");
const QUrl POPUP_ITEM_URL = QUrl("qrc:/Qml/ProgressFeatureItem.qml");
Hix::ProgressManager::ProgressManager()
{
}

Hix::ProgressManager::~ProgressManager()
{
}

void Hix::ProgressManager::generatePopup()
{
	QQmlComponent* component = new QQmlComponent(qmlManager->engine, POPUP_URL);
	auto qmlInstance = component->create(qmlContext(qmlManager->popupArea));
	auto popupShell = dynamic_cast<Hix::QML::ProgressPopupShell*>(qmlInstance);
	_popup.reset(popupShell);
	_popup->setParentItem(qmlManager->popupArea);
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
	_component = new QQmlComponent(qmlManager->engine, POPUP_ITEM_URL);

	auto layout = _popup->featureLayout();
	auto item = dynamic_cast<QQuickItem*>(_component->create(qmlContext(layout)));
	item->setParentItem(layout);

	auto item1 = dynamic_cast<QQuickItem*>(_component->create(qmlContext(layout)));
	item1->setParentItem(layout);
}
