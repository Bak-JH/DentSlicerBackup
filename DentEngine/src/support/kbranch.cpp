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
        qDebug() << "branching layers " << layer_idx << "/" << slices.size();
        Slice& slice = slices[layer_idx];

        for (int op_idx=0; op_idx<slices.overhang_points.size(); op_idx++){
            OverhangPoint& overhang_point = slices.overhang_points[op_idx];

            /*qDebug() << overhang_point.position.Z << layer_idx*cfg->layer_height;
            if (overhang_point.position.Z > layer_idx*cfg->layer_height)
                continue;*/

            if (overhang_point.height<1)
                goto stem;


            if (overhang_point.branchable){
                float min_distance=9999999;
                OverhangPoint* target_stem_point;

                // find nearest branch
                for (int op2_idx=0; op2_idx<slices.overhang_points.size(); op2_idx++){
                    float distance = pointDistance(overhang_point.position, slices.overhang_points[op2_idx].position);
                    if (distance == 0)
                        continue;
                    if (distance<=min_distance && slices.overhang_points[op2_idx].branchable \
                            && slices.overhang_points[op2_idx].branching_cnt<4){
                        target_stem_point = &slices.overhang_points[op2_idx];
                        min_distance = distance;
                    }
                }

                // found available branch
                if (min_distance == 9999999){
                    goto stem;
                }

                // change branch status
                if (min_distance <= cfg->branching_threshold){
                //if (min_distance <= 100*cfg->support_density*cfg->duplication_radius*1000){ // select only near stems
                    overhang_point.branchable = false;
                    overhang_point.branching_overhang_point = target_stem_point;
                    target_stem_point->branchable = false;
                    target_stem_point->branching_cnt ++;
                }

                float distance = min_distance;

                IntPoint unit_move = (overhang_point.branching_overhang_point->position - overhang_point.position)*cfg->layer_height*1000/distance;
                overhang_point.moveTo(overhang_point.position + unit_move);

            } else if (overhang_point.branching_overhang_point != NULL) { // branching

                float distance = pointDistance(overhang_point.position, overhang_point.branching_overhang_point->position);
                IntPoint unit_move = (overhang_point.branching_overhang_point->position - overhang_point.position)*cfg->layer_height*1000/distance;
                overhang_point.moveTo(overhang_point.position + unit_move);

                if (distance <= cfg->default_support_radius){ // branch reached another stem
                    overhang_point.branchable = true;
                    overhang_point.branching_overhang_point->branchable = true;
                    overhang_point.branching_overhang_point = NULL;
                    // brancing done
                    //qDebug() << "branching done";
                }

            }

        stem:
            // route stem if no intersection

            // it is too heavy
            if (checkInclusion(slice, overhang_point))
                continue;
            Paths circle_paths;
            circle_paths.push_back(drawCircle(overhang_point, int(cfg->default_support_radius)));
            clpr.Clear();
            clpr.AddPaths(slice.outershell, ptSubject, true);
            clpr.AddPaths(circle_paths, ptClip, true);
            clpr.Execute(ctUnion, slice.outershell, pftNonZero, pftNonZero);
            overhang_point.height += cfg->layer_height;
        }
    }

    /*// sort overhang_points according to its z height
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
    }*/


    // predict weight containing infill

    // set base width

    // generate

    // offset it by half of nozzle_width

}

// if slice contains overhang_point in somewhere
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
