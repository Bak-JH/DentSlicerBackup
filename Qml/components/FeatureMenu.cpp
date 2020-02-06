#include "FeatureMenu.h"
#include "../../qmlmanager.h"
#include "../util/QMLUtil.h"
#include <QtQuick/private/qquickrectangle_p.h>
#include "Buttons.h"
#include "../../feature/FeaturesLoader.h"
#include <qqmlcomponent.h>


using namespace Hix::QML;




Hix::QML::FeatureMenu::FeatureMenu(QQuickItem* parent) : QQuickItem(parent)
{

}

Hix::QML::FeatureMenu::~FeatureMenu()
{
}

QQuickItem* Hix::QML::FeatureMenu::featureItems()
{
	return _featureItems;
}




void Hix::QML::FeatureMenu::componentComplete()
{
	__super::componentComplete();
	_featureItems = findChildItemByID(this, "featureItems");
	Hix::Features::FeaturesLoader loader(qmlManager->engine, this);
	loader.loadFeatureButtons();
	//TODO: temp, move this to license manager and application loader
	

}
