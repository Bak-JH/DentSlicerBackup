#include "FeaturesLoader.h"
#include <string>
#include <functional>
#include "../Qml/components/FeatureMenu.h"
#include "interfaces/Mode.h"


#include "ImportModel.h"
#include "move.h"
#include "rotate.h"
#include "scale.h"
#include "cut/modelcut.h"
#include "shelloffset.h"

#include "extension.h"
#include "label/Labelling.h"
#include "layFlat.h"
#include "autoorientation.h"
#include "arrange/autoarrange.h"
#include "SupportFeature.h"
#include "stlexport.h"
#include "sliceExport.h"

#include "Qml/components/Buttons.h"
#include "Qml/util/QMLUtil.h"

const std::string ICO_PRE("qrc:/Resource/menu_");
const std::string ICO_BASIC_SUF(".png");
const std::string ICO_SELECTED_SUF("_select_1.png");
const QUrl MENU_ITEM_URL = QUrl("qrc:/Qml/MenuItem.qml");

using namespace Hix::QML;
using namespace Hix::Features;


Hix::Features::FeaturesLoader::FeaturesLoader(QQmlEngine* e, Hix::QML::FeatureMenu* menu) : _component(e, MENU_ITEM_URL), _menu(menu)
{
}



template<typename ModeType>
std::function<void()> closeFeatureModeFunctor(Hix::QML::Controls::ToggleSwitch* button)
{
	static_assert(std::is_base_of<Hix::Features::Mode, ModeType>{});
	std::function<void()> functor = [button]() {
			Hix::Application::ApplicationManager::getInstance().featureManager().setMode(nullptr);
	};
	return functor;
}


void setStr(const std::string& name, Hix::QML::Controls::ToggleSwitch* button)
{
	button->setProperty("iconBasic", QString::fromStdString(ICO_PRE + name + ICO_BASIC_SUF));
	button->setProperty("iconSelected", QString::fromStdString(ICO_PRE + name + ICO_SELECTED_SUF));
	button->setProperty("featureName", QString::fromStdString(name));
}

template<typename ModeType>
void addButton(const std::string& name, FeaturesLoader* loader)
{
	auto& menu = loader->menu();
	auto button = dynamic_cast<Hix::QML::Controls::ToggleSwitch*>(loader->parsedComp().create(qmlContext(&menu)));
	button->setParentItem(menu.featureItems());
	setStr(name, button);
	auto openfunctor = openFeatureModeFunctor<ModeType>(button);
	auto closefunctor = closeFeatureModeFunctor<ModeType>(button);
	QObject::connect(button, &Hix::QML::Controls::ToggleSwitch::checked, openfunctor);
	QObject::connect(button, &Hix::QML::Controls::ToggleSwitch::unchecked, closefunctor);
}

void Hix::Features::FeaturesLoader::addDivider()
{
	auto rect = new QQuickRectangle();
	rect->setProperty("width", 1);
	rect->setProperty("height", 80);
	rect->setProperty("color", "#dddddd");
	rect->setParentItem(_menu->featureItems());
}

QQmlComponent& Hix::Features::FeaturesLoader::parsedComp()
{
	return _component;
}

Hix::QML::FeatureMenu& Hix::Features::FeaturesLoader::menu()
{
	return *_menu;
}

void Hix::Features::FeaturesLoader::loadFeatureButtons()
{
	addButton<Hix::Features::ImportModelMode>("open",this);
	addDivider();

	addButton<Hix::Features::MoveMode>("move" ,this);
	addButton<Hix::Features::RotateMode>("rotate" ,this);
	addButton<Hix::Features::ScaleMode>("scale" ,this);
	addDivider();

	addButton<Hix::Features::ModelCut>("cut" ,this);
	addButton<Hix::Features::ShellOffsetMode>("shelloffset" ,this);
	addButton<Hix::Features::ExtendMode>("extend" ,this);
	addButton<Hix::Features::LabellingMode>("label" ,this);
	addButton<Hix::Features::LayFlatMode>("layflat" ,this);
	addButton<Hix::Features::AutoOrientateMode>("orient" ,this);
	addButton<Hix::Features::AutoArrangeMode>("arrange" ,this);
	addButton<Hix::Features::SupportMode>("support" ,this);
	addDivider();

	addButton<Hix::Features::STLExportMode>("save" ,this);
	addButton<Hix::Features::SliceExportMode>("slice" ,this);
}
