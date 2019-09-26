#include "raft.h"

Raft::Raft(int type)
{
    Raft::type = type;
}

void Raft::generate(Slices& slices){
    switch (type) {
    case 0:
        break;
    case generalraft:
        generateGeneralRaft(slices);
        break;
    case kraft:
        generateKRaft(slices);
        break;
    }
}
