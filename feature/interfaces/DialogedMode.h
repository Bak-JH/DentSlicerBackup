#pragma once
#include "Mode.h"
#include <unordered_set>
class QUrl;
namespace Hix
{
	namespace QML
	{
		class FeaturePopupShell;
		class ControlOwner;
	}
	namespace Features
	{

		class DialogedMode : virtual public Mode
		{
		public:
			DialogedMode(const QUrl& dialogUrl);
			virtual ~DialogedMode();
			Hix::QML::ControlOwner& controlOwner();
		protected:
			std::unique_ptr<Hix::QML::FeaturePopupShell> _popup;
		};

	}
}

