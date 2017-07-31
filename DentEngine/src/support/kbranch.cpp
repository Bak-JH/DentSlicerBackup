#include "kbranch.h"
#include <QVector3D>
#include <math.h>

#define PI 3.14159265

kbranch::kbranch()
{

}

bool operator <(const OverhangPosition &a, const OverhangPosition &b){
    return a.Z < b.Z;
}

float pointDistance(IntPoint A, IntPoint B){
    return sqrt(pow(A.X-B.X, 2)+pow(A.Y-B.Y,2));
}

float pointDistance(QVector3D A, QVector3D B){
    return sqrt(pow(A.x()-B.x(), 2)+pow(A.y()-B.y(),2)+pow(A.z()-B.z(),2));
}

void generateKbranch(Slices& slices){
    Clipper clpr;


    for (int layer_idx=slices.size()-1; layer_idx>=0; layer_idx--){
        Slice& slice = slices[layer_idx];

        for (int op_idx=0; op_idx<slices.overhang_positions.size(); op_idx++){
            OverhangPosition overhang_position = slices.overhang_positions[op_idx];

            if (overhang_position.height<2)
                continue;

            if (overhang_position.branchable){
                OverhangPosition& target_overhang_position;
                float min_distance=9999;
                for (int op2_idx=0; op2_idx<slices.overhang_positions.size(); op2_idx++){
                    if (pointDistance(overhang_position, slices.overhang_positions[op2_idx])<=min_distance)
                        target_overhang_position = slices.overhang_positions[op2_idx];
                }
                if (pointDistance(overhang_position, target_overhang_position) <= 2*cfg->duplication_radius*1000){
                    overhang_position.branchable = false;
                    overhang_position.branching_overhang_position = target_overhang_position;
                    target_overhang_position.branchable = false;
                }
                qDebug() << "min distance = " << min_distance;
            } else if (overhang_position.branching_overhang_position != NULL) { // move overhang position to target op

            } else { // do not branch, route stem
                Paths circle_paths;
                circle_paths.push_back(drawCircle(overhang_position, int(cfg->default_support_radius)));
                clpr.Clear();
                clpr.AddPaths(slice.outershell, ptSubject, true);
                clpr.AddPaths(circle_paths, ptClip, true);
                clpr.Execute(ctUnion, slice.outershell, pftNonZero, pftNonZero);
                overhang_position.height += cfg->layer_height;
            }
        }
    }

    // sort overhang_positions according to its z height
    sort(slices.overhang_positions.begin(), slices.overhang_positions.end());

    for (int op_idx=0; op_idx<slices.overhang_positions.size(); op_idx ++){
        qDebug() << "generating support " << op_idx << "/" << slices.overhang_positions.size();

        OverhangPosition overhang_position = slices.overhang_positions[op_idx];
        //drawCircle(overhang_position);
        int slice_idx = int(overhang_position.Z/(Configuration::resolution)/cfg->layer_height);

        if (slice_idx+1 > slices.size()-1 || slice_idx == 1)
            continue;

        // draw pin point
        for (int layer_idx = slice_idx+1; layer_idx > slice_idx-1; layer_idx--){
            // qDebug() << layer_idx << slice_idx << "asdfasdf" << cfg->default_support_radius*(slice_idx-layer_idx+2)/3;

            Slice& slice = slices[layer_idx];

            Paths circle_paths;
            circle_paths.push_back(drawCircle(overhang_position, int(cfg->default_support_radius*(slice_idx-layer_idx+2)/3)));
            clpr.Clear();
            clpr.AddPaths(slice.outershell, ptSubject, true);
            clpr.AddPaths(circle_paths, ptClip, true);
            clpr.Execute(ctUnion, slice.outershell, pftNonZero, pftNonZero);
            overhang_position.height += cfg->layer_height;
        }


        // draw stem
        while (!checkInclusion(slices[slice_idx+1], overhang_position) && (slice_idx != 0)){

            //qDebug()
            //if ((overhang_position.height % cfg->branching_threshold) == 0)

            Slice& slice = slices[slice_idx];

            Paths circle_paths;
            circle_paths.push_back(drawCircle(overhang_position, int(cfg->default_support_radius)));
            clpr.Clear();
            clpr.AddPaths(slice.outershell, ptSubject, true);
            clpr.AddPaths(circle_paths, ptClip, true);
            clpr.Execute(ctUnion, slice.outershell, pftNonZero, pftNonZero);
            overhang_position.height += cfg->layer_height;

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

Path drawCircle(OverhangPosition overhang_position, int radius){
    //Slice slice = slices[overhang_position.z()/cfg->layer_height];
    Path circle;
    int circle_resolution = 8;
    float circle_radius = radius; //cfg->default_support_radius;
    float unit_angle = PI*2/circle_resolution;
    float angle = PI*2;

    for (int i=0; i<circle_resolution; i++){
        angle += unit_angle;
        IntPoint circle_point = overhang_position + OverhangPosition(int(circle_radius*cos(angle)), int(circle_radius*sin(angle)), overhang_position.Z, cfg->default_support_radius);
        circle.push_back(circle_point);
    }
    return circle;
}
