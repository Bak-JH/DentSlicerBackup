#include "kraft.h"


KRaft::KRaft()
{

}

void generateKRaft(Slices& slices){
    ClipperOffset co;
    convexHull(slices);

    int layer_count = cfg->raft_thickness/cfg->layer_height;

    for (int layer_idx=0; layer_idx*cfg->layer_height<cfg->raft_thickness; layer_idx ++){
        qDebug() << "generating raft " << layer_idx << layer_count << layer_idx*cfg->layer_height;

        Slice raft_slice;

        int raft_offset = abs(layer_idx-layer_count/2)*(cfg->raft_offset_radius*2/layer_count);

        qDebug() << slices.raft_points.size() << raft_offset;

        if (raft_offset == 0){ // if offset is 0, errors
            raft_slice.outershell.push_back(slices.raft_points);
        } else {
            co.AddPath(slices.raft_points, jtRound, etClosedPolygon);
            co.Execute(raft_slice.outershell, raft_offset);
        }

        qDebug() << raft_slice.outershell.size();
        //raft_slice.outershell.erase(raft_slice.outershell.begin()+1);
        //raft_slice.outershell.push_back();
        slices.insert(slices.begin(), raft_slice);

    }

    qDebug() << "slices size :" << slices.size();
}
