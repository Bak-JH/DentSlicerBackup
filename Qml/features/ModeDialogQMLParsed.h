#pragma once
#include "../../feature/interfaces/ModeDialog.h"
#include <qqmlcomponent.h>

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
			ModeDialogQMLParsed(const QUrl qmlSourcePath);
			virtual ~ModeDialogQMLParsed();
			Hix::QML::ControlOwner& controlOwner()override;
		protected:
			QQmlComponent _component;
			std::unique_ptr<Hix::QML::FeaturePopupShell> _popup;
		};
	}
}