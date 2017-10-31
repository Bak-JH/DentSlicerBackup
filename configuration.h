#ifndef CONFIGURATION_H
#define CONFIGURATION_H
#include <QVariant>
#include <QVector3D>


class Configuration : public QVariantMap
{
public:
    Configuration();

    // for future clipping processes (like offsetting)
    static constexpr int resolution_scale = 3;
    static constexpr int resolution = pow(10,resolution_scale); // resolution range from 1000 to 1
    static constexpr float max_buildsize_x = 1000000/resolution;
    static constexpr float vertex_inbound_distance = 0.001;//(float)1/resolution; // resolution in mm (0.0001 and 0.0009 are same, 1 micron)

    QVector3D origin;
};

extern Configuration* cfg;

#endif // CONFIGURATION_H
