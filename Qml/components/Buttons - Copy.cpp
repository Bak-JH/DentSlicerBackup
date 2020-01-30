#include "FeatureMenu.h"
#include "../../qmlmanager.h"
using namespace Hix::QML;

const QUrl MENU_ITEM_URL = QUrl("qrc:/Qml/MenuItem.qml");

/// Button baseclass ///
Hix::QML::FeatureMenu::FeatureMenu(QQuickItem* parent) : QQuickItem(parent), _component(qmlManager->engine, MENU_ITEM_URL)
{
}

Hix::QML::FeatureMenu::~FeatureMenu()
{
}

void Hix::QML::FeatureMenu::addButton(MenuButtonArg& args)
{
}
