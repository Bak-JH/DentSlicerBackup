#pragma once
#include <qqmlcomponent.h>
#include <qurl.h>
#include "../../feature/interfaces/ModeDialog.h"
namespace Hix
{
	namespace QML
	{
		class FeaturePopupShell;
	}
	namespace Features
	{
		class ModeDialogQMLParsed: public ModeDialog
		{
		public:
			ModeDialogQMLParsed(Mode* mode, QUrl qmlSourcePath);
			virtual ~ModeDialogQMLParsed();
			Hix::QML::ControlOwner& controlOwner()override;
		protected:
			QQmlComponent _component;
			std::unique_ptr<Hix::QML::FeaturePopupShell> _popup;
			Mode* _mode;
		};
	}
}