#include "FeaturesLoader.h"
#include <string>
#include <functional>
#include "../Qml/components/FeatureMenu.h"
#include "interfaces/Mode.h"

#include "ModelBuilder/ModelBuilderMode.h"
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
#include "slice/sliceExport.h"
#include "printerServer.h"

#include "Qml/components/Buttons.h"
#include "Qml/util/QMLUtil.h"
#include <qquickitem.h>
#include <qqmlcomponent.h>



const std::string ICO_PRE("qrc:/Resource/menu_");
const std::string ICO_BASIC_SUF(".png");
const std::string ICO_SELECTED_SUF("_select_1.png");
const QUrl MENU_ITEM_URL = QUrl("qrc:/Qml/MenuItem.qml");
const QUrl DIV_URL = QUrl("qrc:/Qml/FeatureDivider.qml");

using namespace Hix::QML;
using namespace Hix::Features;


Hix::Features::FeaturesLoader::FeaturesLoader(QQmlEngine* e, Hix::QML::FeatureMenu* menu) : _component(e, MENU_ITEM_URL),_dividerComp(e, DIV_URL), _menu(menu)
{

#ifdef _DEBUG
	if (!_component.isReady())
	{
		qDebug() <<"_component error: " << _component.errors();
	}

	if (!_dividerComp.isReady())
	{
		qDebug() << "_dividerComp error: " << _dividerComp.errors();
	}
#endif
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
	auto divider = _dividerComp.create(qmlContext(&menu()));
	dynamic_cast<QQuickItem*>(divider)->setParentItem(_menu->featureItems());
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
	if (Hix::Application::ApplicationManager::getInstance().settings().liscense == Hix::Settings::PRO)
	{
		addButton<Hix::Features::ModelBuilderMode>("modelbuilder", this);
	}
	addDivider();

	addButton<Hix::Features::MoveMode>("move" ,this);
	addButton<Hix::Features::RotateMode>("rotate" ,this);
	if (Hix::Application::ApplicationManager::getInstance().settings().liscense == Hix::Settings::PRO)
	{
		addButton<Hix::Features::ScaleMode>("scale", this);
	}
	addDivider();

	if (Hix::Application::ApplicationManager::getInstance().settings().liscense == Hix::Settings::PRO)
	{
		addButton<Hix::Features::ModelCut>("cut", this);
		//addButton<Hix::Features::ShellOffsetMode>("shelloffset" ,this);
		addButton<Hix::Features::ExtendMode>("extend", this);
		addButton<Hix::Features::LabellingMode>("label", this);
	}

	addButton<Hix::Features::LayFlatMode>("layflat" ,this);
	//addButton<Hix::Features::AutoOrientateMode>("orient" ,this);
	addButton<Hix::Features::AutoArrangeMode>("arrange" ,this);
	addButton<Hix::Features::SupportMode>("support" ,this);
	addDivider();

	addButton<Hix::Features::STLExportMode>("save" ,this);
	addButton<Hix::Features::SliceExportMode>("slice" ,this);
	addButton<Hix::Features::PrinterServer>("printer", this);

}
