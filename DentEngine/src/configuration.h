#ifndef CONFIGURATION_H
#define CONFIGURATION_H

class Configuration{
public:
    // configurations
    int resolution = 1000; // resolution range from 1000 to 1
    float max_buildsize_x = 1000000/resolution;

    char* slicing_mode = "uniform"; // uniform OR adaptive
    float layer_height = 0.05; // in mm
    float nozzle_width = 0; // in mm , for pointwise printers
    float wall_thickness = 2; // in mm
    static constexpr float vertex_inbound_distance = 0.001;//(float)1/resolution; // resolution in mm (0.0001 and 0.0009 are same, 1 micron)

    int support_type = 1;
    int infill_type = 1;
    int raft_type = 1;
    // set configuration
};


#endif // CONFIGURATION_H
