#pragma once
namespace Hix
{
	namespace QML
	{
		class ControlOwner;
	}
	namespace Features
	{
		class ModeDialog
		{
		public:
			ModeDialog();
			virtual ~ModeDialog();
			virtual Hix::QML::ControlOwner& controlOwner() = 0;
		protected:
		};
	}
}