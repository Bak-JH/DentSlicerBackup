#include "SlicingOptBackend.h"
#include "../qmlmanager.h"
#include "../DentEngine/src/configuration.h"
#include <regex>

//constexpr for mapping enum to string and vise versa
//TODO: c++20 static reflection when it becomes available
constexpr std::array<std::string_view, 2> SupportTypeStr{
	"None"		,
	"Vertical"	
};	
constexpr std::array<std::string_view, 3> InfillTypeStr{
	"None"		,
	"Full"	,
	"General"
};

constexpr std::array<std::string_view, 2> RaftTypeStr{
	"None"		,
	"General"
};
constexpr std::array<std::string_view, 2> SlicingModeStr{
	"Uniform"		,
	"Adaptive"
};

constexpr std::array<std::string_view, 4> ResolutionStr{
	"2560*1440",
	"1920*1080",
	"1280*800",
	"1024*768"
};
constexpr std::array<std::string_view, 3> LayerHeightStr{
	"0.1","0.2","0.05"
};


constexpr std::array<std::string_view, 3> ResinTypeStr{
	"Temporary",
	"Clear",
	"Castable"
};


constexpr std::array<std::string_view, 11> SupportRadiusMin{
	"0.5",
	"0.3",
	"0.2",
	"0.1",
	"0.08",
	"0.06",
	"0.05",
	"0.04",
	"0.03",
	"0.02",
	"0.01"
};

constexpr std::array<std::string_view, 11> SupportRadiusMax{
	"2.0",
	"1.5",
	"1.2",
	"1.0",
	"0.8",
	"0.6",
	"0.5",
	"0.4",
	"0.3",
	"0.2",
	"0.1"
};
constexpr std::array<std::string_view, 2> PrinterVendorType{
	"Hix",
	"3D'Light"
};

constexpr std::array<std::string_view, 2> SliceInvertStr{
	"Invert X-axis",
	"No inversion"
};


void resStringToInt(std::string_view str, int& x, int& y)
{
	std::string strCopy(str);
	std::regex ws_re("\\*"); // star
	std::sregex_token_iterator tItr(strCopy.begin(), strCopy.end(), ws_re, -1);
	x = std::stoi(*tItr);
	++tItr;
	y = std::stoi(*tItr);

}

SlicingOptBackend::SlicingOptBackend(QmlManager* qmlManager, SlicingConfiguration* config): _qmlManager(qmlManager), _config(config)
{

}


template<std::size_t arraySizeType>
void SlicingOptBackend::addOptionDialogCombo(QString opName, std::array<std::string_view, arraySizeType> content, int defaultIdx)
{
	QStringList enumStrings;
	for (auto& each : content)
	{
		enumStrings.push_back(each.data());
	}
	QMetaObject::invokeMethod(_qmlManager->ltso, "addOptionElementCombo",
		Q_ARG(QVariant, opName),
		Q_ARG(QVariant, enumStrings),
		Q_ARG(QVariant, defaultIdx)
	);
}

void SlicingOptBackend::createSlicingOptControls()
{

	addOptionDialogCombo(QString("Resolution"), ResolutionStr, 0);
	addOptionDialogCombo(QString("Layer height"), LayerHeightStr, 0);
	addOptionDialogCombo(QString("Resin type"), ResinTypeStr, 0);
	addOptionDialogCombo(QString("Raft type"), RaftTypeStr, 1);
	addOptionDialogCombo(QString("Support type"), SupportTypeStr, 1);
	addOptionDialogCombo(QString("Infill type"), InfillTypeStr, 1);
	addOptionDialogCombo(QString("Slicing mode"), SlicingModeStr, 0);
	addOptionDialogCombo(QString("Support minimum radius"), SupportRadiusMin, 2);
	addOptionDialogCombo(QString("Support maximum radius"), SupportRadiusMax, 3);
	addOptionDialogCombo(QString("Printer vendor"), PrinterVendorType, 0);
	addOptionDialogCombo(QString("Slice image inversion"), SliceInvertStr, 0);
	addOptionDialogPercentage(QString("Support density"), 50);
	QObject::connect(_qmlManager->ltso, SIGNAL(optionChanged(QString, QVariant)), this, SLOT(onOptionChanged(QString, QVariant)));

}

void SlicingOptBackend::addOptionDialogPercentage(QString opName, int defaultVal)
{
	QMetaObject::invokeMethod(_qmlManager->ltso, "addOptionElementPercentage",
		Q_ARG(QVariant, opName),
		Q_ARG(QVariant, defaultVal)
	);
}





void SlicingOptBackend::onOptionChanged(QString opName, QVariant newVal)
{
	int toInt = newVal.toInt();
	if (opName == "Resolution")
	{
		int x, y;
		resStringToInt(ResolutionStr[toInt], x, y);
		_config->setResolutionX(x);
		_config->setResolutionY(y);
	}
	else if (opName == "Layer height")
	{
		_config->layer_height = std::stof(std::string(LayerHeightStr[toInt]));
	}
	else if (opName == "Resin type")
	{
		_config->resin_type = (SlicingConfiguration::ResinType)toInt;
	}
	else if (opName == "Raft type")
	{
		_config->raft_type = (SlicingConfiguration::RaftType)toInt;
	}
	else if (opName == "Support type")
	{
		_config->support_type = (SlicingConfiguration::SupportType)toInt;

	}
	else if (opName == "Infill type")
	{
		_config->infill_type = (SlicingConfiguration::InFillType)toInt;

	}
	else if (opName == "Slicing mode")
	{
		_config->slicing_mode = (SlicingConfiguration::SlicingMode)toInt;

	}
	else if (opName == "Support minimum radius")
	{
		_config->support_radius_min = std::stof(std::string(SupportRadiusMin[toInt]));

	}
	else if (opName == "Support maximum radius")
	{
		_config->support_radius_max = std::stof(std::string(SupportRadiusMax[toInt]));
	}
	else if (opName == "Printer vendor")
	{
		_config->printer_vendor_type = (SlicingConfiguration::PrinterVendor)toInt;
	}
	else if (opName == "Slice image inversion")
	{
		_config->slice_invert = (SlicingConfiguration::Invert)toInt;
	}
	else if (opName == "Support density")
	{
		_config->supportDensity = toInt;
	}

}