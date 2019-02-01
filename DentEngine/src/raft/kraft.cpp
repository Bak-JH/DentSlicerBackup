#include "kraft.h"


KRaft::KRaft()
{

}

void generateKRaft(Slices& slices){
    ClipperOffset co;

    Slice raft_slice;
    Slice first_slice = slices[0];
    Slice first_offset_slice;

    co.AddPaths(first_slice.outershell, jtRound, etClosedPolygon);
    co.Execute(first_offset_slice.outershell, scfg->raft_base_radius);

    int layer_count = scfg->raft_thickness/scfg->layer_height;

    for (int first_layer_idx=0; first_layer_idx*scfg->layer_height< scfg->raft_thickness; first_layer_idx ++){
        slices.insert(slices.begin(), first_slice);
    }

    for (int layer_idx=0; layer_idx*scfg->layer_height<scfg->raft_thickness; layer_idx ++){
        qDebug() << "generating raft " << layer_idx << layer_count << layer_idx*scfg->layer_height;

        raft_slice.outershell.clear();

        int raft_offset = abs(layer_idx-layer_count/2)*(scfg->raft_offset_radius*2/layer_count);

        if (raft_offset == 0){ // if offset is 0, errors
            raft_slice.outershell = first_offset_slice.outershell;
            //raft_slice.outershell.push_back(slices.raft_points);
        } else {
            co.AddPaths(first_offset_slice.outershell, jtRound, etClosedPolygon);
            co.Execute(raft_slice.outershell, raft_offset);
        }
        slices.insert(slices.begin(), raft_slice);
    }
    /*convexHull(slices);

    int layer_count = scfg->raft_thickness/scfg->layer_height;

    for (int layer_idx=0; layer_idx*scfg->layer_height<scfg->raft_thickness; layer_idx ++){
        qDebug() << "generating raft " << layer_idx << layer_count << layer_idx*scfg->layer_height;

        Slice raft_slice;

        int raft_offset = abs(layer_idx-layer_count/2)*(scfg->raft_offset_radius*2/layer_count);

        if (raft_offset == 0){ // if offset is 0, errors
            raft_slice.outershell.push_back(slices.raft_points);
        } else {
            co.AddPath(slices.raft_points, jtRound, etClosedPolygon);
            co.Execute(raft_slice.outershell, raft_offset);
        }

        //raft_slice.outershell.erase(raft_slice.outershell.begin()+1);
        //raft_slice.outershell.push_back();
        slices.insert(slices.begin(), raft_slice);

    }*/

    qDebug() << "slices size :" << slices.size();
}
