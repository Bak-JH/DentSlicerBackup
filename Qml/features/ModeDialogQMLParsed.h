#pragma once
#include "../../feature/interfaces/ModeDialog.h"
#include <qstring.h>
#include <qqmlcomponent.h>
#include <qquickitem.h>
namespace Hix
{
	namespace QML
	{
		class LeftPopupShell;
	}
	namespace Features
	{
		class ModeDialogQMLParsed: public ModeDialog
		{
		public:
			ModeDialogQMLParsed(QUrl qmlSourcePath);
			virtual ~ModeDialogQMLParsed();
			Hix::QML::ControlOwner& controlOwner()override;
		protected:
			QQmlComponent _component;
			std::unique_ptr<Hix::QML::LeftPopupShell> _popup;
		};
	}
}