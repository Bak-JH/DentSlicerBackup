#pragma once
#include "Mode.h"
#include <unordered_set>
#include "../../DentEngine/src/mesh.h"
#include "../../Qml/features/ModeDialogQMLParsed.h"	
class GLModel;
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
			
		protected:
			ModeDialogQMLParsed _dialog;
		};

	}
}

