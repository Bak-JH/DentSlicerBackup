#ifndef CONFIGURATION_H
#define CONFIGURATION_H
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

#define TEMPORARY_CONTRACTION_RATIO 1.01//1.005//0.99502//0.98814
#define CLEAR_CONTRACTION_RATIO 0.99504 //0.99009
#define CASTABLE_CONTRACTION_RATIO 0.99009

#include <stdio.h>
#include <math.h>
#include <QVector3D>
#include <QVariant>



class SlicingConfiguration
{
public:
	enum class SupportType
	{
		None = 0,
		KBranch,
		General
	};
	enum class InFillType
	{
		None = 0,
		Full,
		General
	};

	enum class RaftType
	{
		None = 0,
		General
	};
    // configurations
    static constexpr int resolution_scale = 3;
    static constexpr int resolution = 1000;//pow(10,resolution_scale); // resolution range from 1000 to 1
    static constexpr float max_buildsize_x = 1000.0f;//1000000/resolution;
    static constexpr float vertex_inbound_distance = 0.002f;//0.03;//(float)1/resolution; // resolution in mm (0.0001 and 0.0009 are same, 1 micron)
	static constexpr float vertex_3D_distance = 0.0034f;

    const char* slicing_mode = "uniform"; // uniform OR adaptive
    float layer_height = 0.1f; // in mm
    float nozzle_width = 0.0f; // in mm (diameter) , for printers with nozzles
    float wall_thickness = 2.0f; // in mm
    float fill_thickness = 1.0f; // in mm
    float support_density = 0.4f;
    float infill_density = 0.3f;

    int resin_type = TEMPORARY_RESIN;
    float contraction_ratio = TEMPORARY_CONTRACTION_RATIO;

	SupportType support_type = SupportType::General;
	InFillType infill_type = InFillType::Full;
	RaftType raft_type = RaftType::General;

    // raft settings
    float raft_thickness = 2.0f; // in mm
    float raft_base_radius = 5.0f; // in mm
    float raft_offset_radius = 4.0f; // in mm

    // support settings
    float support_radius_max = 1.5f;
    float support_radius_min = 0.3f;
    float support_base_height = 2.0f;

    // bed configuration
    QVector3D origin;

    /* // settings for capsule 3D Printer
    int resolution_x = 2560;
    int resolution_y = 1440;

    float bed_x = 120.96; // in mm
    float bed_y = 68.04; // in mm*/

    // settings for vittro plus
    int resolution_x = 1920;
    int resolution_y = 1080;

    float bed_x = 124.8f; // in mm
    float bed_y = 70.2f; // in mm

    /*// settings for vittro
    int resolution_x = 1280;
    int resolution_y = 800;

    float bed_x = 138; // in mm
    float bed_y = 90; // in mm*/

	size_t bed_margin_x = 1;
	size_t bed_margin_y = 1;

    float pixel_per_mm = float(resolution_x)/float(bed_x);

	void set(const QVariantMap& map);
};
extern SlicingConfiguration* scfg;

#endif // CONFIGURATION_H
