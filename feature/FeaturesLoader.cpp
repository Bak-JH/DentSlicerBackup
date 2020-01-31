#include "FeaturesLoader.h"
#include <string>
#include <functional>
#include "../Qml/components/FeatureMenu.h"
#include "interfaces/Mode.h"
#include "../qmlmanager.h"

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


const std::string ICO_PRE("qrc:/Resource/menu_");
const std::string ICO_BASIC_SUF(".png");
const std::string ICO_SELECTED_SUF("_select_1.png");

using namespace Hix::QML;
using namespace Hix::Features;

MenuButtonArg createIconArg(const std::string& name)
{
	MenuButtonArg arg{
		name,
		ICO_PRE + name + ICO_BASIC_SUF,
		ICO_PRE + name + ICO_SELECTED_SUF };
	return arg;
}

template<typename ModeType>
std::function<void()> openFeatureModeFunctor()
{
	static_assert(std::is_base_of<Hix::Features::Mode, ModeType>{});
	std::function<void()> functor = []() {
		if (!qmlManager->isFeatureActive())
		{
			try
			{
				ModeType* newMode = new ModeType();
				qmlManager->setMode(newMode);
			}
			catch (...)
			{
				qDebug() << "mode creation failed";
				qmlManager->setMode(nullptr);
			}

		}
		else if (qmlManager->isActive<ModeType>())
		{
			qmlManager->setMode(nullptr);
		}
	};
	return functor;
}

template<typename ModeType>
MenuButtonArg createButtonArg(const std::string& name)
{
	MenuButtonArg arg = createIconArg(name);
	arg.functor = openFeatureModeFunctor<ModeType>();
	return arg;
}

void Hix::Features::FeaturesLoader::loadFeatureButtons(Hix::QML::FeatureMenu& menu)
{
	
	menu.addButton(createButtonArg<Hix::Features::ImportModelMode>("open"));
	menu.addDivider();

	menu.addButton(createButtonArg<Hix::Features::MoveMode>("move"));
	menu.addButton(createButtonArg<Hix::Features::RotateMode>("rotate"));
	menu.addButton(createButtonArg<Hix::Features::ScaleMode>("scale"));
	menu.addDivider();

	menu.addButton(createButtonArg<Hix::Features::ModelCut>("cut"));
	menu.addButton(createButtonArg<Hix::Features::ShellOffsetMode>("shelloffset"));
	menu.addButton(createButtonArg<Hix::Features::ExtendMode>("extend"));
	menu.addButton(createButtonArg<Hix::Features::LabellingMode>("label"));
	menu.addButton(createButtonArg<Hix::Features::LayFlatMode>("layflat"));
	menu.addButton(createButtonArg<Hix::Features::AutoOrientateMode>("orient"));
	menu.addButton(createButtonArg<Hix::Features::AutoArrangeMode>("arrange"));
	
	menu.addButton(createButtonArg<Hix::Features::SupportMode>("support"));
	menu.addDivider();

	menu.addButton(createButtonArg<Hix::Features::STLExportMode>("save"));
	menu.addButton(createButtonArg<Hix::Features::SliceExportMode>("extract"));
}
