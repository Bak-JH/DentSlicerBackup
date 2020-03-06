#include "HixQML.h"
#include "PopupShell.h"
#include "Toast.h"
#include "SlideBar.h"
#include "ViewMode.h"
#include "PrintInfo.h"
#include "Buttons.h"
#include "Inputs.h"
#include "FeatureMenu.h"
#include "PartList.h"
#include "ModalShell.h"
#include "MainWindow.h"
void Hix::QML::registerTypes()
{
	qmlRegisterType<Hix::QML::MainWindow>("hix.qml", 1, 0, "MainWindow");


	qmlRegisterType<Hix::QML::FeaturePopupShell>("hix.qml", 1, 0, "FeaturePopupShell");
	qmlRegisterType<Hix::QML::ProgressPopupShell>("hix.qml", 1, 0, "ProgressPopupShell");
	qmlRegisterType<Hix::QML::ToastShell>("hix.qml", 1, 0, "ToastShell");
	qmlRegisterType<Hix::QML::SlideBarShell>("hix.qml", 1, 0, "SlideBarShell");
	qmlRegisterType<Hix::QML::RangeSlideBarShell>("hix.qml", 1, 0, "RangeSlideBarShell");
	qmlRegisterType<Hix::QML::ModalShell>("hix.qml", 1, 0, "ModalShell");
	qmlRegisterType<Hix::QML::ViewModeShell>("hix.qml", 1, 0, "ViewModeShell");

	qmlRegisterType<Hix::QML::PartList>("hix.qml", 1, 0, "PartList");
	qmlRegisterType<Hix::QML::PartListItem>("hix.qml", 1, 0, "PartListItem");

	qmlRegisterType<Hix::QML::PrintInfo>("hix.qml", 1, 0, "PrintInfoText");

	qmlRegisterType<Hix::QML::FeatureMenu>("hix.qml", 1, 0, "FeatureMenu");

	qmlRegisterType<Hix::QML::Controls::Button>("hix.qml", 1, 0, "Button");
	qmlRegisterType<Hix::QML::Controls::ToggleSwitch>("hix.qml", 1, 0, "ToggleSwitch");

	qmlRegisterType<Hix::QML::Controls::DropdownBox>("hix.qml", 1, 0, "DropdownBox");
	qmlRegisterType<Hix::QML::Controls::InputSpinBox>("hix.qml", 1, 0, "InputSpinBox");
	qmlRegisterType<Hix::QML::Controls::TextInputBox>("hix.qml", 1, 0, "TextInputBox");
}
