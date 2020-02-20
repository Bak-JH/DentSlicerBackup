#include "FeatureMenu.h"
#include "../../qmlmanager.h"
#include "../util/QMLUtil.h"
#include <QtQuick/private/qquickrectangle_p.h>
#include "Buttons.h"
#include "../../feature/FeaturesLoader.h"
#include <qqmlcomponent.h>
#include "../../application/ApplicationManager.h"

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
	getItemByID(this, _featureItems, "featureItems");
	Hix::Features::FeaturesLoader loader(&Hix::Application::ApplicationManager::getInstance().engine(), this);
	loader.loadFeatureButtons();
}
