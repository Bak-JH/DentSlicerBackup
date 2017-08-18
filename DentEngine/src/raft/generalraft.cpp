#include "generalraft.h"

GeneralRaft::GeneralRaft()
{

}

void generateGeneralRaft(Slices& slices){

    for (int layer_idx=0; layer_idx*cfg->layer_height<cfg->raft_thickness; layer_idx ++){
        qDebug() << "generating raft " << layer_idx << layer_idx*cfg->layer_height;

        Slice raft_slice;
        slices.insert(slices.begin(), raft_slice);

    }

    qDebug() << "slices size :" << slices.size();
}
