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

constexpr std::array<std::string_view, 3> BedNumberStr{
	"1",
	"2",
	"3"

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
void SlicingOptBackend::addOptionDialog(QString opName, std::array<std::string_view, arraySizeType> content, int defaultIdx)
{
	QStringList enumStrings;
	for (auto& each : content)
	{
		enumStrings.push_back(each.data());
	}
	QMetaObject::invokeMethod(_qmlManager->ltso, "addOptionElement",
		Q_ARG(QVariant, opName),
		Q_ARG(QVariant, enumStrings),
		Q_ARG(QVariant, defaultIdx)
	);
}

void SlicingOptBackend::createSlicingOptControls()
{

	addOptionDialog(QString("Resolution"), ResolutionStr, 0);
	addOptionDialog(QString("Layer height"), LayerHeightStr, 0);
	addOptionDialog(QString("Bed number"), BedNumberStr, 0);
	addOptionDialog(QString("Resin type"), ResinTypeStr, 0);
	addOptionDialog(QString("Raft type"), RaftTypeStr, 1);
	addOptionDialog(QString("Support type"), SupportTypeStr, 1);
	addOptionDialog(QString("Infill type"), InfillTypeStr, 1);
	addOptionDialog(QString("Slicing mode"), SlicingModeStr, 0);
	addOptionDialog(QString("Support minimum radius"), SupportRadiusMin, 2);
	addOptionDialog(QString("Support maximum radius"), SupportRadiusMax, 3);
	addOptionDialog(QString("Printer vendor"), PrinterVendorType, 0);
	addOptionDialog(QString("Slice image inversion"), SliceInvertStr, 0);

	QObject::connect(_qmlManager->ltso, SIGNAL(optionChanged(QString, int)), this, SLOT(onOptionChanged(QString, int)));

}

void SlicingOptBackend::updateConfig()
{
}


void SlicingOptBackend::onOptionChanged(QString opName, int newIndex)
{
	if (opName == "Resolution")
	{
		int x, y;
		resStringToInt(ResolutionStr[newIndex], x, y);
		_config->setResolutionX(x);
		_config->setResolutionY(y);
	}
	else if (opName == "Layer height")
	{
		_config->layer_height = std::stof(std::string(LayerHeightStr[newIndex]));
	}
	else if (opName == "Bed number")
	{
		//?
	}
	else if (opName == "Resin type")
	{
		_config->resin_type = (SlicingConfiguration::ResinType)newIndex;
	}
	else if (opName == "Raft type")
	{
		_config->raft_type = (SlicingConfiguration::RaftType)newIndex;
	}
	else if (opName == "Support type")
	{
		_config->support_type = (SlicingConfiguration::SupportType)newIndex;

	}
	else if (opName == "Infill type")
	{
		_config->infill_type = (SlicingConfiguration::InFillType)newIndex;

	}
	else if (opName == "Slicing mode")
	{
		_config->slicing_mode = (SlicingConfiguration::SlicingMode)newIndex;

	}
	else if (opName == "Support minimum radius")
	{
		_config->support_radius_min = std::stof(std::string(SupportRadiusMin[newIndex]));

	}
	else if (opName == "Support maximum radius")
	{
		_config->support_radius_max = std::stof(std::string(SupportRadiusMax[newIndex]));
	}
	else if (opName == "Printer vendor")
	{
		_config->printer_vendor_type = (SlicingConfiguration::PrinterVendor)newIndex;
	}
	else if (opName == "Slice image inversion")
	{
		_config->slice_invert = (SlicingConfiguration::Invert)newIndex;
	}

}