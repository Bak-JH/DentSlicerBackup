#include "generalbranch.h"

GeneralBranch::GeneralBranch()
{

}

bool operator <(const OverhangPoint &a, const OverhangPoint &b){
    return a.position.Z < b.position.Z;
}


void generateGeneralBranch(Slices& slices){
    Clipper clpr;

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
            //if ((overhang_point.height % cfg->branching_threshold_radius) == 0)

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
}
