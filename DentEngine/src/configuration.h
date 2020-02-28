#pragma once
//#define tan_theta 0.1
//#define tan_theta 0.57735026918962576450914878050196 // tan(30)
//#define tan_theta 0.83909963117728 // tan(40)
#define tan_theta 1 // tan(45)
//#define tan_theta 1.1917535925942099587053080718604 // tan(50)
//#define tan_theta 1.4281480067421145021606184849985 // tan(55)
//#define tan_theta 2.1445069205095586163562607910459 // tan(65)
//#define tan_theta 2.7474774194546222787616640264977 // tan(70)
//#define tan_theta 5.671281819617709530994418439864 // tan(80)
//#define tan_theta 11.430052302761343067210855549163 // tan(85)
//#define tan_theta 100



/*
 * resin_type : 1 - temporarycrown, 2 - transparent, 3 - castable
 * contraction_ratio : 1 - 0.98522, 2 - 0.99009, 3 -
 */

#define TEMPORARY_RESIN 1
#define CLEAR_RESIN 2
#define CASTABLE_RESIN 3

#define TEMPORARY_CONTRACTION_RATIO 1.00//1.005//0.99502//0.98814
#define CLEAR_CONTRACTION_RATIO 0.99504 //0.99009
#define CASTABLE_CONTRACTION_RATIO 0.99009

#include <stdio.h>
#include <math.h>
#include <QVector3D>
#include <QVariant>
#include "Bounds3D.h"


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

constexpr std::array<std::string_view, 3> LayerHeightStr{
	"0.1","0.2","0.05"
};


constexpr std::array<std::string_view, 3> ResinTypeStr{
	"Temporary",
	"Clear",
	"Castable"
};

class SlicingConfiguration
{
public:
	enum class SupportType: uint8_t
	{
		None = 0,
		Vertical = 1
	};
	enum class InFillType : uint8_t
	{
		None = 0,
		Full = 1,
		General = 2
	};
	enum class SlicingMode : uint8_t
	{
		Uniform = 0,
		Adaptive = 1
	};

	enum class ResinType : uint8_t
	{
		Temporary = 0,
		Clear = 1,
		Castable = 2
	};
	enum class Invert : uint8_t
	{
		InvertXAxis = 0,
		NoInversion
	};
	SlicingConfiguration();
	float raft_base_radius()const;

    // configurations
    float layer_height = 0.1f; // in mm

	Invert slice_invert = Invert::InvertXAxis;
	SlicingMode slicing_mode = SlicingMode::Uniform; // uniform OR adaptive
	ResinType resin_type = ResinType::Temporary;
	SupportType support_type = SupportType::Vertical;
	InFillType infill_type = InFillType::Full;

    // raft settings
    float raft_thickness = 1.0f; // in mm

    // support settings
    float support_radius_max = 1.0f;
    float support_radius_min = 0.25f;
    float support_base_height = 2.0f;
	size_t supportDensity = 50;

private:

};
extern SlicingConfiguration* scfg;

