#ifndef CONFIGURATION_H
#define CONFIGURATION_H

class Configuration{
public:
    // configurations
    int resolution = 1000; // resolution range from 1000 to 1
    float max_buildsize_x = 1000000/resolution;
    float vertex_inbound_distance = (float)1/resolution; // resolution in mm (0.0001 and 0.0009 are same, 1 micron)

    char* slicing_mode = "uniform"; // uniform OR adaptive
    float layer_height = 0.05; // in mm


    // set configuration
};

#endif // CONFIGURATION_H
