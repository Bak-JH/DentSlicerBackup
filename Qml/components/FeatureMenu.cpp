#include "FeatureMenu.h"
#include "../../qmlmanager.h"
#include "../util/QMLUtil.h"
#include <QtQuick/private/qquickrectangle_p.h>
using namespace Hix::QML;

const QUrl MENU_ITEM_URL = QUrl("qrc:/Qml/MenuItem.qml");

Hix::QML::FeatureMenu::FeatureMenu(QQuickItem* parent) : QQuickItem(parent), _component(qmlManager->engine, MENU_ITEM_URL)
{

}

Hix::QML::FeatureMenu::~FeatureMenu()
{
}

void Hix::QML::FeatureMenu::addButton(MenuButtonArg& args)
{
	auto button = dynamic_cast<QQuickItem*>(_component.create(qmlContext(this)));
	button->setParentItem(_featureItems);
	button->setProperty("iconBasic", QString::fromStdString(args.defaultButtonImgPath));
	button->setProperty("iconSelected", QString::fromStdString(args.activeButtonImgPath));
	button->setProperty("featureName", QString::fromStdString(args.featureName));
}

void Hix::QML::FeatureMenu::addDivider()
{
	auto rect = new QQuickRectangle();
	rect->setParentItem(_featureItems);
	rect->setProperty("width", 1);
	rect->setProperty("height", 80);
	rect->setProperty("color", "#dddddd");

}

void Hix::QML::FeatureMenu::componentComplete()
{
	__super::componentComplete();
	_featureItems = findChildItemByID(this, "featureItems");
}
