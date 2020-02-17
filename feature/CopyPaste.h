#pragma once

#include "interfaces/Mode.h"
#include "interfaces/DialogedMode.h"
#include "interfaces/Feature.h"
namespace Hix
{
	namespace Features
	{
		class CopyPasteMode : public DialogedMode
		{
		public:
			CopyPasteMode();
			virtual ~CopyPasteMode();
		private:
		};

		class CopyPaste : public Feature		
		{
		public:
			CopyPaste();
			virtual ~CopyPaste();
		protected:
			void undoImpl()override;
			void redoImpl()override;
			void runImpl()override;
		private:

		};
	}
}