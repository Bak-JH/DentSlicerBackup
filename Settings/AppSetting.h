#pragma once
#include "PrinterSetting.h"
#include "DeployInfo.h"
#include "BasicSetting.h"
#include "JSONWriteSetting.h"
#include "SliceSetting.h"
#include "SupportSetting.h"
#include "LabelSetting.h"

namespace Hix
{
	namespace Settings
	{
		enum Liscense {
			NONE,
			BASIC,
			PRO
		};

		class AppSetting
		{
		public:
			AppSetting();
			~AppSetting();
			DeployInfo deployInfo; //this has to be first!
			BasicSetting basicSetting;
			SliceSetting sliceSetting;
			SupportSetting supportSetting;
			PrinterSetting printerSetting;
			LabelSetting labelSetting;

			std::string version;
			Liscense liscense = NONE;
			void parse();
			void toDefault();
		};
	}
}
