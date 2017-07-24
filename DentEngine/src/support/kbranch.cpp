#include "kbranch.h"
#include <QVector3D>
#include <math.h>

#define PI 3.14159265

kbranch::kbranch()
{

}

void generateKbranch(Slices& slices){
    Clipper clpr;
    for (int op_idx=0; op_idx<slices.overhang_positions.size(); op_idx ++){
        qDebug() << "generating support " << op_idx << "/" << slices.overhang_positions.size();

        OverhangPosition overhang_position = slices.overhang_positions[op_idx];
        //drawCircle(overhang_position);
        int slice_idx = int(overhang_position.Z/(Configuration::resolution)/cfg->layer_height);
        while (!checkInclusion(slices[slice_idx], overhang_position) && (slice_idx != 0)){
            Slice& slice = slices[slice_idx];
            //qDebug() << "support generation on " << slice_idx << slice.outershell.size() << slice.outershell[slice.outershell.size()-1].size();

            Paths temp_paths;
            temp_paths.push_back(drawCircle(overhang_position));
            clpr.Clear();
            clpr.AddPaths(slice.outershell, ptSubject, true);
            clpr.AddPaths(temp_paths, ptClip, true);
            clpr.Execute(ctUnion, slice.outershell, pftNonZero, pftNonZero);
            //clpr.Execute(ctUnion, slice.outershell, pftNonZero, pftNonZero);

            //qDebug() << "after union" << result_paths.size() << result_paths[0].size();
            //qDebug() << "after union" << slice.outershell.size() << slice.outershell[slice.outershell.size()-1].size();
            slice_idx --;
        }
    }
    // predict weight containing infill

    // set base width

    // generate

    // offset it by half of nozzle_width

    // generate support
    /*for (int slice_idx=slices.size()-1; slice_idx>=0; slice_idx--){
        Slice slice = slices[slice_idx];
        vector<OverhangPosition>::iterator opit;
        for (opit = slices.overhang_positions.begin() ; opit != slices.overhang_positions.end();){
            OverhangPosition overhang_position = (*opit);
            for (Path contour : slice.outershell){
                if (PointInPolygon(overhang_position, contour)){
                    slices.overhang_positions.erase(opit);
                    break;
                }
            }

            // else part
            ++opit;
            overhang_position.height += cfg->layer_height;
            clpr.Clear();
            clpr.AddPaths(slice.outershell, ptSubject, true);
            clpr.AddPaths(support.drawCircle(), ptClip, true);
            clpr.Execute(ctUnion, slice.outershell, pftNonZero, pftNonZero);
        }
        for (OverhangPosition overhang_position : slices.overhang_positions){

        }
    }*/
}

bool checkInclusion(Slice& slice, OverhangPosition overhang_position){
    for (Path contour : slice){
        if (PointInPolygon(overhang_position, contour)){
            return true;
        }
    }
    return false;

}

Path drawCircle(OverhangPosition overhang_position){
    //Slice slice = slices[overhang_position.z()/cfg->layer_height];
    Path circle;
    int circle_resolution = 8;
    float circle_radius = cfg->default_support_radius;
    float unit_angle = PI*2/circle_resolution;
    float angle = PI*2;

    for (int i=0; i<circle_resolution; i++){
        angle += unit_angle;
        IntPoint circle_point = overhang_position + OverhangPosition(int(circle_radius*cos(angle)), int(circle_radius*sin(angle)), overhang_position.Z);
        circle.push_back(circle_point);
    }
    return circle;
}
