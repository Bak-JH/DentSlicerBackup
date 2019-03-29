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
        OverhangPoint* overhang_point = slices.overhang_points[op_idx];
        for (int op2_idx=0; op2_idx<slices.overhang_points.size(); op2_idx++){

            float distance = pointDistance(overhang_point->position, slices.overhang_points[op2_idx]->position);
            if (distance == 0)
                continue;

            // if branchable overhang points has less than n value, push back
            if (overhang_point->branchable_overhang_points.size()==0){
                BranchableOverhangPoint* bop = new BranchableOverhangPoint();
                bop->overhangpoint = slices.overhang_points[op2_idx];
                bop->distance = distance;
                overhang_point->branchable_overhang_points.push_back(bop);
            }

            BranchableOverhangPoint* max_bop = overhang_point->branchable_overhang_points[0];
            int max_bop_idx = 0;

            // find biggest branchable overhang point
            for (int i=0; i<overhang_point->branchable_overhang_points.size(); i++){
                BranchableOverhangPoint* bop = overhang_point->branchable_overhang_points.at(i);//(*bop_it);
                if (bop->distance > max_bop->distance){
                    max_bop = bop;
                    max_bop_idx = i;
                }
            }

            // compare current bop with max_bop so that change branchable overhang points vector
            if (distance<=max_bop->distance && slices.overhang_points[op2_idx]->branchable \
                    && slices.overhang_points[op2_idx]->branching_cnt<=scfg->branchable_overhang_point_cnt){
                overhang_point->branchable_overhang_points.erase(overhang_point->branchable_overhang_points.begin() + max_bop_idx);
                BranchableOverhangPoint* bop = new BranchableOverhangPoint();
                bop->overhangpoint = slices.overhang_points[op2_idx];
                bop->distance = distance;
                overhang_point->branchable_overhang_points.push_back(bop);
            }
        }
    }

    // process branching
    //for (int layer_idx=slices.size()-1; layer_idx>=0; layer_idx--){
    for (int layer_idx=0; layer_idx>=0; layer_idx--){
        qDebug() << "branching layers " << layer_idx << "/" << slices.size();
        Slice& slice = slices[layer_idx];
        Paths circle_paths;
        float radius;


        for (int op_idx=0; op_idx<slices.overhang_points.size(); op_idx++){

            OverhangPoint* overhang_point = slices.overhang_points[op_idx];

            // if overhang point's position height is lower than current layer height or overhang ended
            if (overhang_point->position.Z <= (layer_idx-3)*scfg->layer_height*scfg->resolution || overhang_point->position.Z == -1)
            {
                //qDebug() << "overhang ended or not started yet : " << op_idx;
                continue;
            }

            // just routed
            if (overhang_point->height < (2-0.8)){
                goto stem;
            }

            if (overhang_point->branchable){
                float min_distance = 9999999;
                BranchableOverhangPoint* min_bop = NULL;

                for (BranchableOverhangPoint* bop : overhang_point->branchable_overhang_points){
                    if (min_distance >= (*bop).distance && bop->overhangpoint->position.Z >= layer_idx*scfg->layer_height*scfg->resolution){
                        min_distance = (*bop).distance;
                        min_bop = bop;
                    }
                }

                if (min_bop == NULL || min_distance >= scfg->branching_threshold_radius)
                    goto stem;


                OverhangPoint* target_stem_point = min_bop->overhangpoint;

                // found available branch

                // change branch status
                overhang_point->branchable = false;
                overhang_point->branching_overhang_point = new OverhangPoint(overhang_point->position.X, overhang_point->position.Y, overhang_point->position.Z);
                overhang_point->target_branching_overhang_point = target_stem_point;
                target_stem_point->branchable = false;
                target_stem_point->branching_cnt ++;

                float distance = min_distance;
                overhang_point->unit_move = (overhang_point->target_branching_overhang_point->position - overhang_point->position)*scfg->layer_height*scfg->resolution/distance;
                IntPoint total_move = overhang_point->position + overhang_point->unit_move;
                overhang_point->branchTo(IntPoint(total_move.X, total_move.Y, overhang_point->position.Z));
            } else if (overhang_point->target_branching_overhang_point != NULL) { // branching

                if (overhang_point->target_branching_overhang_point->position.Z == -1){
                    overhang_point->branchable = true;
                    overhang_point->target_branching_overhang_point = NULL;
                    overhang_point->branching_overhang_point = NULL;
                    //overhang_point.position = overhang_point.branching_overhang_point->position;
                    goto stem;
                }

                //unit_move = (overhang_point.target_branching_overhang_point->position - overhang_point.branching_overhang_point.position)*scfg->layer_height*1000/distance;
                float prev_distance = pointDistance(overhang_point->branching_overhang_point->position, overhang_point->target_branching_overhang_point->position);
                IntPoint total_move = overhang_point->branching_overhang_point->position + overhang_point->unit_move;
                overhang_point->branchTo(IntPoint(total_move.X, total_move.Y, overhang_point->position.Z));
                float distance = pointDistance(overhang_point->branching_overhang_point->position, overhang_point->target_branching_overhang_point->position);

                if (distance <= 2*scfg->layer_height*scfg->resolution){ // branch reached another stem
                    overhang_point->branchable = true;
                    overhang_point->target_branching_overhang_point->branchable = true;
                    overhang_point->target_branching_overhang_point = NULL;
                    overhang_point->branching_overhang_point = NULL;
                    overhang_point->unit_move = NULL;
                    // brancing done
                    //qDebug() << "branching done";
                }
            }

        stem:
            // route stem if no intersection
            if (overhang_point->height < (2-1.4))
                radius = scfg->default_support_radius*(overhang_point->height/2 + 0.7);
            else {
                radius = (overhang_point->height>30) ? scfg->default_support_radius*(overhang_point->height/30) : radius = scfg->default_support_radius;
            }

            // in the stem but not from initial stage
            if (overhang_point->branching_overhang_point != NULL){
                Path circledrew = drawCircle(overhang_point->branching_overhang_point, int(scfg->default_support_radius));

                if (checkInclusion(slice, overhang_point->branching_overhang_point)|| checkInclusion(slice, circledrew)){
                    overhang_point->branchable = true;
                    overhang_point->target_branching_overhang_point->branchable = true;
                    overhang_point->target_branching_overhang_point = NULL;
                    overhang_point->branching_overhang_point = NULL;
                    overhang_point->unit_move = NULL;
                } else{
                    circle_paths.push_back(circledrew);
                }
            }



            overhang_point->height += scfg->layer_height;

            Path circle = drawCircle(overhang_point, int(radius));
            if (checkInclusion(slice, circle) || checkInclusion(slice, overhang_point)) { // inclusion occured
                continue;
            } else {
                circle_paths.push_back(circle);
            }
        }

        slice.support = circle_paths;

        clpr.Clear();
        clpr.AddPaths(slice.outershell, ptSubject, true);
        clpr.AddPaths(circle_paths, ptClip, true);
        clpr.Execute(ctUnion, slice.outershell, pftEvenOdd, pftPositive);
    }


    for (int layer_idx=slices.size()-1; layer_idx>=0; layer_idx--){
        qDebug() << "generate circle supports : " << slices[layer_idx].support.size();
    }

    // predict weight containing infill

    // set base width

    // generate

    // offset it by half of nozzle_width

}



