#include "generalraft.h"

GeneralRaft::GeneralRaft()
{

}

void generateGeneralRaft(Slices& slices){

    for (int layer_idx=0; layer_idx*scfg->layer_height<scfg->raft_thickness; layer_idx ++){
        qDebug() << "generating raft " << layer_idx << layer_idx*scfg->layer_height;

        Slice raft_slice;
        slices.insert(slices.begin(), raft_slice);

    }

    qDebug() << "slices size :" << slices.size();
}
