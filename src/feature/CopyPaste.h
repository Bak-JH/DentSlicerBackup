#pragma once
#include "interfaces/Mode.h"
#include "interfaces/DialogedMode.h"
#include "interfaces/Feature.h"
#include "addModel.h"
#include <unordered_set>
class GLModel;
namespace Hix
{
	namespace Features
	{
		class CopyPasteMode : public Mode
		{
		public:
			CopyPasteMode(const std::unordered_set<GLModel*>& targets);
			virtual ~CopyPasteMode();
			void apply();
		private:
			std::unordered_set<GLModel*> _targets;
		};

		class CopyPaste : public ListModel		
		{
		public:
			CopyPaste(GLModel* target);
			virtual ~CopyPaste();
		protected:
			void runImpl()override;
		private:
			GLModel* _target;
		};
	}
}