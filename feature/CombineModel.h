#pragma once
#pragma once
#include "Mesh/mesh.h"
#include <qvector3d.h>
#include "interfaces/Mode.h"
#include "feature/interfaces/FlushSupport.h"
#include "interfaces/DialogedMode.h"
class GLModel;
namespace Hix
{
	namespace QML
	{
		namespace Controls
		{
			class InputSpinBox;
		}
	}

	namespace Features
	{
		class CombineModels : public Feature, public FlushSupport
		{
		public:
			CombineModels(std::unordered_set<GLModel*>& targets, std::string groupName, bool isDisplay = false);
			virtual ~CombineModels();
		protected:
			void undoImpl()override;
			void redoImpl()override;
			void runImpl()override;
		private:
			std::unordered_set<GLModel*> _targetModels;
			std::string _groupName;
			bool _isDisplay;
			std::unique_ptr<GLModel> _shellModel;
		};



		class CombineMode : public InstantMode
		{
		public:
			CombineMode(std::unordered_set<GLModel*>& targets, std::string groupName, bool isDisplay = false);
			virtual ~CombineMode();
			//void applyButtonClicked()override;
		};
	}
}
