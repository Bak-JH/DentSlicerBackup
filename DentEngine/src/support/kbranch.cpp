#include "kbranch.h"
#include <QVector3D>
#include <math.h>


KBranch::KBranch()
{

}


void generateKBranch(Slices& slices){
    Clipper clpr;


    // find top n nearest branch and save it as branchable overhang points
    for (int op_idx=0; op_idx<slices.overhang_points.size(); op_idx++){
        OverhangPoint& overhang_point = slices.overhang_points[op_idx];
        for (int op2_idx=0; op2_idx<slices.overhang_points.size(); op2_idx++){

            float distance = pointDistance(overhang_point.position, slices.overhang_points[op2_idx].position);

            if (distance == 0)
                continue;

            if (overhang_point.branchable_overhang_points.size()==0){
                BranchableOverhangPoint* bop = new BranchableOverhangPoint();
                bop->overhangpoint = &slices.overhang_points[op2_idx];
                bop->distance = distance;
                overhang_point.branchable_overhang_points.push_back(bop);
            }

            BranchableOverhangPoint* max_bop = overhang_point.branchable_overhang_points[0];
            vector<BranchableOverhangPoint*>::iterator max_bop_it = overhang_point.branchable_overhang_points.begin();

            //for (bop_idx=0; bop_idx<overhang_point.branchable_overhang_points.size(); bop_idx++){
            for (vector<BranchableOverhangPoint*>::iterator bop_it=overhang_point.branchable_overhang_points.begin(); bop_it != overhang_point.branchable_overhang_points.end(); ++bop_it){
            //for (BranchableOverhangPoint bop : overhang_point.branchable_overhang_points){
                BranchableOverhangPoint* bop = (*bop_it);
                if (bop->distance > max_bop->distance){
                    max_bop = bop;
                    max_bop_it = bop_it;
                }
            }

            if (distance<=max_bop->distance && slices.overhang_points[op2_idx].branchable \
                    && slices.overhang_points[op2_idx].branching_cnt<=cfg->branchable_overhang_point_cnt){
                overhang_point.branchable_overhang_points.erase(max_bop_it);
                BranchableOverhangPoint* bop = new BranchableOverhangPoint();
                bop->overhangpoint = &slices.overhang_points[op2_idx];
                bop->distance = distance; //pointDistance(bop->overhangpoint->position, overhang_point.position);
                overhang_point.branchable_overhang_points.push_back(bop);
            }
        }
    }

    // process branching
    for (int layer_idx=slices.size()-1; layer_idx>=0; layer_idx--){
        qDebug() << "branching layers " << layer_idx << "/" << slices.size();
        Slice& slice = slices[layer_idx];
        Paths circle_paths;

        for (int op_idx=0; op_idx<slices.overhang_points.size(); op_idx++){

            OverhangPoint& overhang_point = slices.overhang_points[op_idx];
            //qDebug() << overhang_point.position.Z << layer_idx*cfg->layer_height;
            // if overhang point's position height is lower than current layer height or overhang ended
            if (overhang_point.position.Z < layer_idx*cfg->layer_height*cfg->resolution || overhang_point.position.Z == -1)
                continue;

            float radius = cfg->default_support_radius;

            // just routed
            if (overhang_point.height<(1-0.2)){
                radius = cfg->default_support_radius*(overhang_point.height+0.2);
                goto stem;
            }


            if (overhang_point.branchable){

                float min_distance = 9999999;
                BranchableOverhangPoint* min_bop = NULL;

                for (BranchableOverhangPoint* bop : overhang_point.branchable_overhang_points){
                    if (min_distance >= (*bop).distance && bop->overhangpoint->position.Z >= layer_idx*cfg->layer_height*cfg->resolution){
                        min_distance = (*bop).distance;
                        min_bop = bop;
                    }
                }

                if (min_bop == NULL || min_distance >= cfg->branching_threshold_radius){
                    goto stem;
                }

                OverhangPoint* target_stem_point = min_bop->overhangpoint;

                // found available branch

                // change branch status
                overhang_point.branchable = false;
                overhang_point.branching_overhang_point = new OverhangPoint(overhang_point.position.X, overhang_point.position.Y, overhang_point.position.Z);
                overhang_point.target_branching_overhang_point = target_stem_point;
                target_stem_point->branchable = false;
                target_stem_point->branching_cnt ++;

                float distance = min_distance;
                overhang_point.unit_move = (overhang_point.target_branching_overhang_point->position - overhang_point.position)*cfg->layer_height*cfg->resolution/distance;
                IntPoint total_move = overhang_point.position + overhang_point.unit_move;
                overhang_point.branchTo(IntPoint(total_move.X, total_move.Y, overhang_point.position.Z));

            } else if (overhang_point.target_branching_overhang_point != NULL) { // branching

                if (overhang_point.target_branching_overhang_point->position.Z == -1){
                    overhang_point.branchable = true;
                    overhang_point.target_branching_overhang_point = NULL;
                    overhang_point.branching_overhang_point = NULL;
                    //overhang_point.position = overhang_point.branching_overhang_point->position;
                    goto stem;
                }

                //unit_move = (overhang_point.target_branching_overhang_point->position - overhang_point.branching_overhang_point.position)*cfg->layer_height*1000/distance;
                float prev_distance = pointDistance(overhang_point.branching_overhang_point->position, overhang_point.target_branching_overhang_point->position);
                IntPoint total_move = overhang_point.branching_overhang_point->position + overhang_point.unit_move;
                overhang_point.branchTo(IntPoint(total_move.X, total_move.Y, overhang_point.position.Z));
                float distance = pointDistance(overhang_point.branching_overhang_point->position, overhang_point.target_branching_overhang_point->position);

                if (prev_distance < distance){
                    qDebug() << "ironic" << prev_distance << distance;
                    qDebug() << "length of unit move" << sqrt(pow(overhang_point.unit_move.X,2)+pow(overhang_point.unit_move.Y,2));
                }
                if (distance <= cfg->default_support_radius){ // branch reached another stem
                    overhang_point.branchable = true;
                    overhang_point.target_branching_overhang_point->branchable = true;
                    overhang_point.target_branching_overhang_point = NULL;
                    overhang_point.branching_overhang_point = NULL;
                    overhang_point.unit_move = NULL;
                    // brancing done
                    //qDebug() << "branching done";
                }
            }

        stem:
            // route stem if no intersection

            if (checkInclusion(slice, overhang_point)){
                qDebug() << "reached end";
                overhang_point.position.Z = -1; // finish flag
                continue;
            }

            if (overhang_point.branching_overhang_point != NULL)
                circle_paths.push_back(drawCircle(*overhang_point.branching_overhang_point, int(radius)));
            /*else
                circle_paths.push_back(drawCircle(overhang_point, int(radius)));*/

            circle_paths.push_back(drawCircle(overhang_point, int(radius)));

            overhang_point.height += cfg->layer_height;
        }

        clpr.Clear();
        clpr.AddPaths(slice.outershell, ptSubject, true);
        clpr.AddPaths(circle_paths, ptClip, true);
        clpr.Execute(ctUnion, slice.outershell, pftNonZero, pftNonZero);
    }

    // predict weight containing infill

    // set base width

    // generate

    // offset it by half of nozzle_width

}



