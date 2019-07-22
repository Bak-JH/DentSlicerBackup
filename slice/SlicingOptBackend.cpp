#include "SlicingOptBackend.h"
#include "../qmlmanager.h"
#include "../DentEngine/src/configuration.h"
#include <regex>

//constexpr for mapping enum to string and vise versa
//TODO: c++20 static reflection when it becomes available
constexpr std::array<std::string_view, 3> SupportTypeStr{
	"None"		,
	"K-Branch"	,
	"General"	
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
	addOptionDialog(QString("Resolution"), ResolutionStr, 0);
	addOptionDialog(QString("Resolution"), ResolutionStr, 0);
	addOptionDialog(QString("Resolution"), ResolutionStr, 0);
	addOptionDialog(QString("Resolution"), ResolutionStr, 0);
	addOptionDialog(QString("Resolution"), ResolutionStr, 0);


	//                LeftTabSlicingOptionElement{
//                    id:option_resolution
//                    columnName: "Resolution"
//                    columnContents: ["1920*1080", "1280*800", "1024*768"]
//                }

//                Item{width:parent.width;height:2}//spacer

//                LeftTabSlicingOptionElement{
//                    id:option_layer_height
//                    columnName: "Layer height"
//                    columnContents: ["0.1","0.2","0.05"]
//                }

//                Item{width:parent.width;height:2}//spacer

//                LeftTabSlicingOptionElement{
//                    id:option_bed_number
//                    columnName: "Bed number"
//                    columnContents: ["1","2","3"]
//                }

//                Item{width:parent.width;height:2}//spacer

//                LeftTabSlicingOptionElement{
//                    id:option_resin_type
//                    columnName: "Resin Type"
//                    columnContents: ["Temporary","Clear","Castable"]
//                }

//                Item{width:parent.width;height:2}//spacer

//                LeftTabSlicingOptionElement{
//                    id:option_raft
//                    columnName: "Raft"
//                    columnContents: ["general","none"]

//                    Item{width:parent.width;height:2}//spacer

//                }
//                LeftTabSlicingOptionElement2{
//                    columnName: "Support"
//                    columnText: "Type"
//                }

//                LeftTabSlicingOptionElement{
//                    id:option_support
//                    columnName: ""
//                    columnContents: ["k-branch","general", "none"]
//                }
//                LeftTabSlicingOptionElement2{
//                    columnName: ""
//                    columnText: "Density"
//                }
//                LeftTabSlicingOptionElement3{
//                    id:option_support_density
//                }

//                Item{width:parent.width;height:2}//spacer

//                LeftTabSlicingOptionElement2{
//                    columnName: "Infill"
//                    columnText: "Type"
//                }
//                LeftTabSlicingOptionElement{
//                    id:option_infill
//                    columnName: ""
//                    columnContents: ["general","full","none"]
//                }
//                LeftTabSlicingOptionElement2{
//                    columnName: ""
//                    columnText: "Density"
//                }
//                LeftTabSlicingOptionElement3{
//                    id:option_infill_density
//                }

}

void SlicingOptBackend::updateConfig()
{
}
