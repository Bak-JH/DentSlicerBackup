#include "kbranch.h"
#include <QVector3D>
#include <math.h>

#define PI 3.14159265

kbranch::kbranch()
{

}

void generateKbranch(Slices& slices){
    for (QVector3D overhang_position : slices.overhang_positions){
        //drawCircle(overhang_position);
    }
    // predict weight containing infill

    // set base width

    // generate

    // offset it by half of nozzle_width
}


void kbranch::drawCircle(QVector3D overhang_position){
    //Slice slice = slices[overhang_position.z()/cfg->layer_height];
    vector<QVector3D> circle;
    int circle_resolution = 8;
    float circle_radius = cfg->default_support_radius;
    float unit_angle = PI*360/circle_resolution;
    float angle = 0;

    float root2 = 1.414213562373;

    for (int i=0; i<circle_resolution; i++){
        angle += unit_angle;
        QVector3D circle_point = overhang_position + QVector3D(circle_radius*sin(angle), circle_radius*cos(angle), overhang_position.z());//QVector3D(circle_radius/root2, circle_radius/root, overhang_position.z());
        circle.push_back(circle_point);
    }

}
