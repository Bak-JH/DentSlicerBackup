#include "FeatureMenu.h"
#include "../../qmlmanager.h"
#include "../util/QMLUtil.h"
#include <QtQuick/private/qquickrectangle_p.h>
#include "Buttons.h"
#include "../../feature/FeaturesLoader.h"


using namespace Hix::QML;

const QUrl MENU_ITEM_URL = QUrl("qrc:/Qml/MenuItem.qml");



Hix::QML::FeatureMenu::FeatureMenu(QQuickItem* parent) : QQuickItem(parent)
{

}

Hix::QML::FeatureMenu::~FeatureMenu()
{
}

void Hix::QML::FeatureMenu::addButton(const MenuButtonArg& args)
{
	auto button = dynamic_cast<Hix::QML::Controls::Button*>(_component->create(qmlContext(this)));
	button->setParentItem(_featureItems);
	button->setProperty("iconBasic", QString::fromStdString(args.defaultButtonImgPath));
	button->setProperty("iconSelected", QString::fromStdString(args.activeButtonImgPath));
	button->setProperty("featureName", QString::fromStdString(args.featureName));
	QObject::connect(button, &Hix::QML::Controls::Button::clicked, args.functor);
}

void Hix::QML::FeatureMenu::addDivider()
{
	auto rect = new QQuickRectangle();
	rect->setProperty("width", 1);
	rect->setProperty("height", 80);
	rect->setProperty("color", "#dddddd");
	rect->setParentItem(_featureItems);


}



void Hix::QML::FeatureMenu::componentComplete()
{
	__super::componentComplete();
	_featureItems = findChildItemByID(this, "featureItems");
	_component.emplace(qmlManager->engine, MENU_ITEM_URL);
	Hix::Features::FeaturesLoader loader;
	loader.loadFeatureButtons(*this);
	//TODO: temp, move this to license manager and application loader
	

}
