#pragma once
#include "../../common/Singleton.h"
#include "../../common/Debug.h"

#include <QString>

namespace Hix
{
	namespace Slicer
	{
		namespace Debug
		{

			class SlicerDebug:public Common::Singleton<SlicerDebug>
			{
			public:
#ifdef _DEBUG
				bool enableDebug = true;
#else
				//users should be allowed to debug svgs...for pros
				bool enableDebug = false;
#endif
				QString debugFilePath;
			};

		}
	}
}



