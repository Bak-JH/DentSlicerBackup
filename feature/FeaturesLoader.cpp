#include "FeaturesLoader.h"
#include <string>
#include <functional>
#include "../Qml/components/FeatureMenu.h"
#include "interfaces/Mode.h"
#include "../qmlmanager.h"

#include "ImportModel.h"



const std::string ICO_PRE("qrc:/Resource/");
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
	static_assert(std::is_base_of<Hix::Features::Mode, ModeType>);
	std::function<void()> functor = []() {
		if (!qmlManager->isFeatureActive())
		{
			qmlManager->setMode(new ModeType());
		}
	};
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
}
