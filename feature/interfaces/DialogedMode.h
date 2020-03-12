#pragma once
#include "Mode.h"
#include <unordered_set>
#include "../../Qml/components/ControlForwardInclude.h"
class QUrl;
namespace Hix
{
	namespace Features
	{

		class DialogedMode : virtual public Mode
		{
		public:
			DialogedMode(const QUrl& dialogUrl);
			virtual ~DialogedMode();
			Hix::QML::ControlOwner& controlOwner();
			virtual void applyButtonClicked() = 0;
		protected:
			std::unique_ptr<Hix::QML::FeaturePopupShell> _popup;
			bool isReady()const;
		};

	}
}

