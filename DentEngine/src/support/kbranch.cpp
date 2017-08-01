#include "kbranch.h"
#include <QVector3D>
#include <math.h>

#define PI 3.14159265

kbranch::kbranch()
{

}

bool operator <(const OverhangPoint &a, const OverhangPoint &b){
    return a.position.Z < b.position.Z;
}

void generateKbranch(Slices& slices){
    Clipper clpr;


    for (int layer_idx=slices.size()-1; layer_idx>=0; layer_idx--){
        Slice& slice = slices[layer_idx];

        for (int op_idx=0; op_idx<slices.overhang_points.size(); op_idx++){
            OverhangPoint overhang_point = slices.overhang_points[op_idx];

            if (overhang_point.height<2)
                continue;

            if (overhang_point.branchable){
                OverhangPoint* target_overhang_point;
                float min_distance=9999;
                for (int op2_idx=0; op2_idx<slices.overhang_points.size(); op2_idx++){
                    float distance = pointDistance(overhang_point.position, slices.overhang_points[op2_idx].position);

                    if (distance<=min_distance){
                        target_overhang_point = &slices.overhang_points[op2_idx];
                        min_distance = distance;
                    }
                }

                // need to fix
                if (pointDistance(overhang_point.position, target_overhang_point->position) <= 2*cfg->duplication_radius*1000){
                    overhang_point.branchable = false;
                    overhang_point.branching_overhang_point = target_overhang_point;
                    target_overhang_point->branchable = false;
                    target_overhang_point->branching_cnt ++;
                }

                qDebug() << "min distance = " << min_distance;
            } else if (overhang_point.branching_overhang_point != NULL) { // move overhang position to target op
                IntPoint unit_move = (overhang_point.branching_overhang_point->position - overhang_point.position)/cfg->layer_height*1000;


                //overhang_point.move (overhang_point.position + overhang_point.brancing_overhang_point->position)
                if (pointDistance(overhang_point.position, overhang_point.branching_overhang_point->position) <= cfg-> default_support_radius) {// branch reached another stem
                    overhang_point.branching_overhang_point = NULL;
                    overhang_point.branchable = true;
                    overhang_point.branching_overhang_point->branchable = true;

                    // brancing done
                }

            } else { // do not branch, route stem
                Paths circle_paths;
                circle_paths.push_back(drawCircle(overhang_point, int(cfg->default_support_radius)));
                clpr.Clear();
                clpr.AddPaths(slice.outershell, ptSubject, true);
                clpr.AddPaths(circle_paths, ptClip, true);
                clpr.Execute(ctUnion, slice.outershell, pftNonZero, pftNonZero);
                overhang_point.height += cfg->layer_height;
            }

            if (overhang_point.branching_cnt >= 3){
                Paths circle_paths;
                circle_paths.push_back(drawCircle(overhang_point, int(cfg->default_support_radius)));
                clpr.Clear();
                clpr.AddPaths(slice.outershell, ptSubject, true);
                clpr.AddPaths(circle_paths, ptClip, true);
                clpr.Execute(ctUnion, slice.outershell, pftNonZero, pftNonZero);
                overhang_point.height += cfg->layer_height;
            }
        }
    }

    // sort overhang_points according to its z height
    sort(slices.overhang_points.begin(), slices.overhang_points.end());

    for (int op_idx=0; op_idx<slices.overhang_points.size(); op_idx ++){
        qDebug() << "generating support " << op_idx << "/" << slices.overhang_points.size();

        OverhangPoint overhang_point = slices.overhang_points[op_idx];
        //drawCircle(overhang_point);
        int slice_idx = int(overhang_point.position.Z/(Configuration::resolution)/cfg->layer_height);

        if (slice_idx+1 > slices.size()-1 || slice_idx == 1)
            continue;

        // draw pin point
        for (int layer_idx = slice_idx+1; layer_idx > slice_idx-1; layer_idx--){
            // qDebug() << layer_idx << slice_idx << "asdfasdf" << cfg->default_support_radius*(slice_idx-layer_idx+2)/3;

            Slice& slice = slices[layer_idx];

            Paths circle_paths;
            circle_paths.push_back(drawCircle(overhang_point, int(cfg->default_support_radius*(slice_idx-layer_idx+2)/3)));
            clpr.Clear();
            clpr.AddPaths(slice.outershell, ptSubject, true);
            clpr.AddPaths(circle_paths, ptClip, true);
            clpr.Execute(ctUnion, slice.outershell, pftNonZero, pftNonZero);
            overhang_point.height += cfg->layer_height;
        }


        // draw stem
        while (!checkInclusion(slices[slice_idx+1], overhang_point) && (slice_idx != 0)){

            //qDebug()
            //if ((overhang_point.height % cfg->branching_threshold) == 0)

            Slice& slice = slices[slice_idx];

            Paths circle_paths;
            circle_paths.push_back(drawCircle(overhang_point, int(cfg->default_support_radius)));
            clpr.Clear();
            clpr.AddPaths(slice.outershell, ptSubject, true);
            clpr.AddPaths(circle_paths, ptClip, true);
            clpr.Execute(ctUnion, slice.outershell, pftNonZero, pftNonZero);
            overhang_point.height += cfg->layer_height;

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
        vector<OverhangPoint>::iterator opit;
        for (opit = slices.overhang_points.begin() ; opit != slices.overhang_points.end();){
            OverhangPoint overhang_point = (*opit);
            for (Path contour : slice.outershell){
                if (PointInPolygon(overhang_point, contour)){
                    slices.overhang_points.erase(opit);
                    break;
                }
            }

            // else part
            ++opit;
            overhang_point.height += cfg->layer_height;
            clpr.Clear();
            clpr.AddPaths(slice.outershell, ptSubject, true);
            clpr.AddPaths(support.drawCircle(), ptClip, true);
            clpr.Execute(ctUnion, slice.outershell, pftNonZero, pftNonZero);
        }
        for (OverhangPoint overhang_point : slices.overhang_points){

        }
    }*/
}

bool checkInclusion(Slice& slice, OverhangPoint overhang_point){
    for (Path contour : slice){
        if (PointInPolygon(overhang_point.position, contour)){
            return true;
        }
    }
    return false;

}

Path drawCircle(OverhangPoint overhang_point, int radius){
    //Slice slice = slices[overhang_point.z()/cfg->layer_height];
    Path circle;
    int circle_resolution = 8;
    float circle_radius = radius; //cfg->default_support_radius;
    float unit_angle = PI*2/circle_resolution;
    float angle = PI*2;

    for (int i=0; i<circle_resolution; i++){
        angle += unit_angle;
        IntPoint circle_point = overhang_point.position + IntPoint(int(circle_radius*cos(angle)), int(circle_radius*sin(angle)), overhang_point.position.Z);
        circle.push_back(circle_point);
    }
    return circle;
}
