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
#define TRANSPARENT_RESIN 2
#define CASTABLE_RESIN 3

#define TEMPORARY_CONTRACTION_RATIO 0.98814
#define TRANSPARENT_CONTRACTION_RATIO 0.99504 //0.99009
#define CASTABLE_CONTRACTION_RATIO 0.99009

#include <stdio.h>
#include <math.h>
#include <QVector3D>
#include <QVariant>

class SlicingConfiguration : public QVariantMap
{
public:
    // configurations
    static constexpr int resolution_scale = 3;
    static constexpr int resolution = pow(10,resolution_scale); // resolution range from 1000 to 1
    static constexpr float max_buildsize_x = 1000000/resolution;
    static constexpr float vertex_inbound_distance = 0.002;//0.03;//(float)1/resolution; // resolution in mm (0.0001 and 0.0009 are same, 1 micron)

    char* slicing_mode = "uniform"; // uniform OR adaptive
    float layer_height = 0.1; // in mm
    float nozzle_width = 0; // in mm (diameter) , for printers with nozzles
    float wall_thickness = 2; // in mm
    float fill_thickness = 1; // in mm
    float support_density = 0.4;
    float infill_density = 0.3;

    float subdivision_radius = 0.85; // in mm
    float duplication_radius = 4; // in mm
    int cluster_size = 2000;

    float overhang_detect_diff_layer_height = 0.5; // in mm
    float default_support_radius = 900 ; // in microns
    float branching_threshold_radius = 4000; // in microns
    int branchable_overhang_point_cnt = 4;

    float overhang_threshold = layer_height/tan_theta;
    float overhang_poll_threshold = support_density * 100; // unit region as 1cm^2

    int resin_type = TEMPORARY_RESIN;
    float contraction_ratio = TEMPORARY_CONTRACTION_RATIO;

    int support_type = 2;
    int infill_type = 1;
    int raft_type = 2;

    // raft settings
    float raft_thickness = 3;
    int raft_base_radius = 3000;
    int raft_offset_radius = 1800; // in microns

    // set configuration
    QVector3D origin;
    int resolution_x = 1280;
    int resolution_y = 800;
    int bed_x = 131.2; // in mm
    int bed_y = 82; // in mm
    float pixel_per_mm = float(resolution_x)/float(bed_x);
};

extern SlicingConfiguration* scfg;

#endif // CONFIGURATION_H
